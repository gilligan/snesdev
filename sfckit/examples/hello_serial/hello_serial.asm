.include "header.inc"
.include "regs.inc"
.include "std.inc"

.global main
.global mem_pool


.SEGMENT "ZEROPAGE"

        ; careful with zeropage!
        ; it is used to access arguments
        ; on stack in subroutines !

.SEGMENT "BSS"

        mem_pool:   .res 64  
        print_delay: .res 1

.SEGMENT "HRAM" : FAR
.SEGMENT "HRAM2" : FAR
.SEGMENT "HDATA" : FAR
.SEGMENT "XCODE"

.SEGMENT "CODE"

main:
        .a8
        .i16
        .smart

        sei


        ; 
        ; set BG properties
        ;

        ldx #BG1_CHR_ADDR($0000)
        stx BG12NBA
        ldx #BG3_CHR_ADDR($3000)
        stx BG34NBA

        ldx BG_MAP_ADDR($1000)
        stx BG1SC
        ldx BG_MAP_ADDR($1800)
        stx BG2SC
        ldx BG_MAP_ADDR($2000)
        stx BG3SC


        ;
        ; copy font & cgram data
        ;

        ldx #$0000
        stx VMADDL
        call g_dma_tag_2, font1_dma
        ldx #$3000
        stx VMADDL
        call g_dma_tag_2, font2_dma
        lda #$00
        sta $2121
        call g_dma_tag_2, cgdata_dma


        ;
        ; print some text
        ;


        ldx #$1000
        stx VMADDL
        puts "check your serial terminal"

        ;
        ; config & enable display
        ;

        lda #$01
        sta BGMODE
        lda #$07
        sta TM
        lda #$0f
        sta INIDISP

        lda #$01
        ldx #$02
        ldy #$03

        lda #$08
        sta print_delay

        ;
        ; enable NMI/joypad reading
        ;

        ;lda #$81
        ;sta $4200
        jsr init_serial_57600
        cli

main_loop: 
        nop
        nop
        jml $c00000+main_loop

quit:

irq_handler:
        lda $4211
        rti

brk_handler:

        jmp __debug_handler

nmi:

        phb
        pha
        phx
        phy
        phd

        lda $4210

        lda $00
        inc
        sta INIDISP
        cmp #16
        bne no_reset
        lda #$00
        s_putsl "hello"
 no_reset:
        sta $00

        pld
        ply
        plx
        pla
        plb

        rti

; Serial in/out at 57600 bps for PAL SNES

; Which controller to receive data on. Defaults to second.
; Use $4016 for first.
.ifndef SERIAL_PORT
	SERIAL_PORT = $4017
.endif

; All routines require that A be in 8-bit mode (sep #$20)

; Initializes serial output
; Preserved: X, Y
init_serial_57600:
	
	; Enable H IRQ
	lda #147
	sta $4207	; HTIMEL
	stz $4208	; HTIMEH
	lda #$90 ; #$10
        sta $4200	; NMITIMEN
	
	; Keep in idle state for a while, to avoid junk
	; byte at beginning.
	lda #130 | 1
;	sta $4016
;:	dec
;	bne :-
	
	rts

; Sends byte
; In: A = value
; Preserved: X, Y
write_serial_57600:	; 46 JSR
	bit $4211	; TIMEUP
	wai
	
	stz $4016	; 36 start bit
	
	sec		; 50 stop bit at end
	bit $4016
	
	; 357.5 clocks per bit
@next:	pha		; 272
	lda #5		; A*36
:	dec a
	bne :-
	pla
	ora #0
	ora #0
	
	sta $4016	; 86	
	ror a
	clc
	bne @next
			; -6
	
	; Be sure stop bit lasts long enough
	phd
	pld
	phd
	pld
	phd
	pld
	
	rts		; 42

; Waits for and receives byte
; Out: A = value
; Preserved: X, Y
read_serial_57600:	; 46 JSR
	; Wait for start bit
	lda #1
:	bit SERIAL_PORT	; wait for idle
	bne :-
:	bit SERIAL_PORT	; 36 wait for start
	beq :-		; 22
	
	; average 45 clocks after start bit
	; need 490 delay to first bit
	; need 210 more
	
	phd
	pld
	phd
	pld
	xba
	xba
	sep #0
	
	; Read 8 data bits into A
	lda #$80	; A serves as counter AND buffer
:	phd		; 206 delay
	pld
	phd
	pld
	pha
	pla
	bit <0
	pha		; 22
	lda #$01	; 16 read bit, inverted into C
	eor SERIAL_PORT	; 36
	lsr a		; 14
	pla		; 28
	ror a		; 14 put into top of A
	bcc :-		; 22 loop until initial bit 7 shifts out
	
	rts
;
; screen setup and dma transfers
;
font1_dma: build_dma_tag 1,$18,font1,^font1,font_end1-font1
font2_dma: build_dma_tag 1,$18,font2,^font2,font_end2-font2
cgdata_dma: build_dma_tag 0,$22,colors,^colors,colors_end-colors

;
; graphics data 
;

str_hello:
        .asciiz "SFCKIT: HELLO WORLD!"

colors:
        .byte $00,$00
        .byte $ff,$7f
        .byte $00,$00
        .byte $ff,$7f
colors_end:



font1: .incbin "font4bpp.bin"
font_end1:

font2: .incbin "font2bpp.bin"
font_end2:
