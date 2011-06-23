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


        ; 
        ; set BG properties
        ;

        ;ldx BG_MAP_ADDR($0000)
        ldx #($0000>>BG_MAP_SHIFT)
        stx BG1SC
        ;ldx #BG1_CHR_ADDR($1000)
        ldx #($1000>>BG1_CHR_SHIFT)
        stx BG12NBA

        ;
        ; copy font & cgram data
        ;

        ldx #$1000
        stx VMADDL
        call g_dma_tag_2,bg_char_dma

        ldx #$0000
        stx VMADDL
        call g_dma_tag_2,bg_map_dma

        lda #$00
        sta $2121
        call g_dma_tag_2,bg_pal_dma

        ;
        ; config & enable display
        ;

        lda #$01
        sta BGMODE

        lda #(1<<0)               ;
        sta TM                    ; BG1 = main screen

        lda #(1<<0)               ;
        sta TS                    ; BG2 = sub screen

        lda #$0f
        sta INIDISP

        lda #$02
        sta $4300
        lda #$0d
        sta $4301          ; set destination register
        ldx #.LOWORD(hdma_table)
        stx $4302          ; set source address
        lda #^hdma_table
        sta $4304          ; set source bank
        lda #$01
        sta $420c


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

        pld
        ply
        plx
        pla
        plb

        rti


;
; screen setup and dma transfers
;
bg_char_dma: build_dma_tag 1,$18,bg_tiles,^bg_tiles,bg_tiles_end-bg_tiles
bg_map_dma:  build_dma_tag 1,$18,bg_map,^bg_map,bg_map_end-bg_map
bg_pal_dma:  build_dma_tag 0,$22,bg_pal,^bg_pal,bg_pal_end-bg_pal


hdma_table: 
        .byte 3 
 .word 1    ; This makes the cool wavey BG effect.
        .byte 4 
 .word 2
        .byte 6 
 .word 3
        .byte 7 
 .word 4
        .byte 6 
 .word 3
        .byte 4 
 .word 2
        .byte 3 
 .word 1
        .byte 2 
 .word 0
        .byte 3 
 .word 511
        .byte 4 
 .word 510
        .byte 6 
 .word 509
        .byte 7 
 .word 508
        .byte 6 
 .word 509
        .byte 4 
 .word 510
        .byte 3 
 .word 511
        .byte 2 
 .word 0
        .byte 4 
 .word 2
        .byte 6 
 .word 3
        .byte 7 
 .word 4
        .byte 6 
 .word 3
        .byte 4 
 .word 2
        .byte 3 
 .word 1
        .byte 2 
 .word 0
        .byte 3 
 .word 511
        .byte 4 
 .word 510
        .byte 6 
 .word 509
        .byte 7 
 .word 508
        .byte 6 
 .word 509
        .byte 4 
 .word 510
        .byte 3 
 .word 511
        .byte 2 
 .word 0
        .byte 3 
 .word 1
        .byte 4 
 .word 2
        .byte 6 
 .word 3
        .byte 7 
 .word 4
        .byte 6 
 .word 3
        .byte 4 
 .word 2
        .byte 3 
 .word 1
        .byte 2 
 .word 0
        .byte 3 
 .word 511
        .byte 4 
 .word 510
        .byte 6 
 .word 509
        .byte 7 
 .word 508
        .byte 6 
 .word 509
        .byte 4 
 .word 510
        .byte 3 
 .word 511
        .byte 2 
 .word 0
        .byte 3 
 .word 1
        .byte 4 
 .word 2
        .byte 5 
 .word 3
        .byte 6 
 .word 4
        .byte 0

; graphics data 
;


.include "bg.gfx"
