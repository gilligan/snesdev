.include "header.inc"
.include "regs.inc"
.include "std.inc"
.include "link.inc"

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
        call g_dma_tag_2, font1_dma
        ldx #$3000
        stx VMADDL
        call g_dma_tag_2,font2_dma
        lda #$00
        sta $2121
        call g_dma_tag_2,cgdata_dma


        ;
        ; print some text
        ;


        ldx #$1000
        stx VMADDL
        puts "hello:bg1"

        ldx #$1800+(32*2)
        stx VMADDL
        puts "hello:bg2"

        ldx #$2000+(2*32*2)
        stx VMADDL
        call printf,str_hello

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

        ;
        ; enable NMI/joypad reading
        ;

        lda #$81
        sta $4200
        cli


        ;
        ; test debug macro output
        ;

        lda #$22
        ldx #$aab0
        DEBUG
        inc 
        inx
        DEBUG
        ldy #$ffff
        DEBUG

        ;
        ; test putc
        ;
        lda #$ff
        sta $3803



        ;; print a string
        ;;

        pea string
        jsr l_puts
        plx


        ;; dump some memory
        ;;

        ldx #512
        phx
        ldx #$9938
        phx
        jsr l_dump_mem
        plx
        plx


main_loop: 


        ;; hex dump some data
        ;; 

;        ldx #500
;hexdump:
;        lda $008000,x
;        sta $3800
;        dex
;        bne hexdump
;        lda #$01
;        sta $3802

        jmp main_loop

quit:

string:
        .byte "hello world",$0a,0

        ;brk_handler = __link_debug_handler

brk_handler:

        link_dump_registers
        link_tx_registers

        lda register_data+debug_data::reg_a
        ldx register_data+debug_data::reg_x
        ldy register_data+debug_data::reg_y


        rti

irq_handler:

        ;jmp __debug_handler
        ;jmp __link_debug_handler

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
