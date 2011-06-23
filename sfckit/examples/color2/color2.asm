.include "header.inc"
.include "regs.inc"
.include "std.inc"

.global main
.global mem_pool
.global DEBUG_FONT_DMA,brk_info,debug_colors


.SEGMENT "ZEROPAGE"

        ; careful with zeropage!
        ; it is used to access arguments
        ; on stack in subroutines !

.SEGMENT "BSS"

        mem_pool:   .res 64  
        scroll_x:   .res 1
        color:      .res 1

.SEGMENT "HRAM" : FAR
.SEGMENT "HRAM2" : FAR
.SEGMENT "HDATA" : FAR
.SEGMENT "XCODE"

.SEGMENT "CODE"

main:
        .a8
        .i16
        .smart

        stz scroll_x


        ; 
        ; set BG properties
        ;

        ;ldx BG_MAP_ADDR($0000)
        ldx #($0000>>BG_MAP_SHIFT)
        stx BG1SC
        ;ldx #(BG1_CHR_ADDR($1000) | BG2_CHR_ADDR($4000))
        ldx #($1000>>BG1_CHR_SHIFT | $4000>>BG2_CHR_SHIFT)
        stx BG12NBA

        ;
        ; copy font & cgram data
        ;

        ldx #$1000
        stx VMADDL
        call g_dma_tag_2,text_char_dma

        ldx #$0000
        stx VMADDL
        call g_dma_tag_2,text_map_dma

        lda #$00
        sta $2121
        call g_dma_tag_2,text_pal_dma

        ;
        ; config & enable display
        ;

        lda #$01
        sta BGMODE

        lda #(1<<0)               ;
        sta TM                    ; BG1 = main screen

        lda #$ff
        sta COLDATA
        lda #$ff
        sta COLDATA
        lda #$ff
        sta COLDATA

        lda #(1<<0)               ;
        sta TS                    ; BG2 = sub screen

        lda #$00                  ;
        sta CGWSEL                ; BG1

        lda #(1<<7|1<<0)
        sta CGADSUB

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
        bra brk_handler
irq_handler:
        bra irq_handler


nmi:

        phb
        pha
        phx
        phy
        phd

        lda $4210
        lda scroll_x
        sta BG1HOFS
        inc scroll_x

        lda color
        inc a
        cmp #32
        bne :+
        lda #$00
        :
        sta color

        lda #(1<<7)
        eor color
        sta COLDATA
        lda #(1<<6)
        eor color
        sta COLDATA
        lda #(1<<5)
        eor color
        sta COLDATA



        pld
        ply
        plx
        pla
        plb

        rti


;
; screen setup and dma transfers
;
text_char_dma: build_dma_tag 1,$18,text_tiles,^text_tiles,text_tiles_end-text_tiles
text_map_dma:  build_dma_tag 1,$18,text_map,^text_map,text_map_end-text_map
text_pal_dma:  build_dma_tag 0,$22,text_pal,^text_pal,text_pal_end-text_pal
;
; graphics data 
;


brk_info:
        .asciiz "! %p:%P (A:%p X:%P Y:%P)"

debug_colors:
        .byte $00,$00
        .byte $ff,$7f
        .byte $00,$00
        .byte $ff,$7f


.include "box.gfx"
.include "text.gfx"
