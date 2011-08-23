
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

.SEGMENT "HRAM" : FAR
.SEGMENT "HRAM2" : FAR
.SEGMENT "HDATA" : FAR
.SEGMENT "XCODE"

.SEGMENT "CODE"

start:

        init_snes

        .a8
        .i16
        .smart


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
        stz VMAIN

        upload_to_vram font1,$0000,font_end1-font1
        upload_to_cgram colors,$00, colors_end-colors

        ;
        ; print some text
        ;


        ldx #$1000
        stx VMADDL
        puts "hello:bg1"

        ;
        ; config & enable display
        ;

        lda #$01
        sta BGMODE
        lda #$07
        sta TM
        lda #$0f
        sta INIDISP


        ;
        ; enable NMI/joypad reading
        ;

        lda #$81
        sta $4200
        cli

main_loop: 
        jmp main_loop

quit:


brk_handler:
irq_handler:

        jmp __debug_handler

nmi:

        phb
        pha
        phx
        phy
        phd

        .ifdef SWC

        jsr process_joypads
        lda joy1_held
        beq :+
                jsr __swc_return_to_bios
        :

        .endif

        lda $4210

        pld
        ply
        plx
        pla
        plb

        rti


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
