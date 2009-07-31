.include "header.inc"
.include "regs.inc"
.include "std.inc"

.global main
.global mem_pool
.global DEBUG_FONT_DMA,brk_info,debug_colors

;
; use font1 for debug output
;
DEBUG_FONT_DMA := font1_dma


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

main:
        .a8
        .i16
        .smart


        ; 
        ; set BG properties
        ;

        ldx BG1_CHR_ADDR($0000)
        stx BG12NBA
        ldx BG3_CHR_ADDR($3000)
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
        CALL(g_dma_tag_2,font1_dma)
        ldx #$3000
        stx VMADDL
        CALL(g_dma_tag_2,font2_dma)
        lda #$00
        sta $2121
        CALL(g_dma_tag_2,cgdata_dma)


        ;
        ; print some text
        ;


        ldx #$1000
        stx VMADDL
        PUTS("hello:bg1")

        ldx #$1800+(32*2)
        stx VMADDL
        PUTS("hello:bg2")

        ldx #$2000+(2*32*2)
        stx VMADDL
        PUTS("hello:bg3")

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
; screen setup and dma transfers
;
DMA_TRANSFER_TAG(font1_dma,1,$18,font1,^font1,font_end1-font1)
DMA_TRANSFER_TAG(font2_dma,1,$18,font2,^font2,font_end2-font2)
DMA_TRANSFER_TAG(cgdata_dma,0,$22,colors,^colors,colors_end-colors)

;
; graphics data 
;

hello_world1:
        .asciiz "SFCKIT: HELLO WORLD1!"
hello_world2:
        .asciiz "SFCKIT: HELLO WORLD2!"
hello_world3:
        .asciiz "SFCKIT: HELLO WORLD3!"

colors:
        .byte $00,$00
        .byte $ff,$7f
        .byte $00,$00
        .byte $ff,$7f
colors_end:

brk_info:
        .asciiz "! %p:%P (A:%p X:%P Y:%P)"

debug_colors:
        .byte $00,$00
        .byte $ff,$7f
        .byte $00,$00
        .byte $ff,$7f


font1: .incbin "font4bpp.bin"
font_end1:

font2: .incbin "font2bpp.bin"
font_end2:
