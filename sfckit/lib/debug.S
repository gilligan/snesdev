.include "std.inc"

        .a8
        .i16
        .smart

.global __debug_handler
.global brk_info

.SEGMENT "BSS"

.SEGMENT "CODE"

.proc __debug_handler

        sei

 
        ;
        ; save registers
        ;
        sta mem_pool+4   ; A
        stx mem_pool+5   ; X
        sty mem_pool+7   ; Y
        pla              ; status reg
        sta mem_pool
        pla              ; pc (low)
        sta mem_pool+1
        inx
        pla              ; pc (high)
        sta mem_pool+2
        pla              ; db 
        sta mem_pool+3 

        lda #$8f
        sta INIDISP

        ldx BG1_CHR_ADDR($0000)
        stx BG12NBA
        ldx BG_MAP_ADDR($1000)
        stx BG1SC
        load_pal debug_colors,$00,4

        ldx #$0000
        stx VMADDL
        CALL(g_dma_tag_2,DEBUG_FONT_DMA)



        ; 
        ; print DB:PC / A,X,Y
        ;
        ldx #$1000+(32*2)
        stx VMADDL
        CALL(printf,brk_info,mem_pool+3,mem_pool+1,mem_pool+4,mem_pool+5,mem_pool+7)

        ;
        ; print first 64 bytes of stack
        ;
        ldx #$1000+(32*4)
        stx VMADDL
        CALL(puts,stack_str)

        ldx #$1000+(32*5)
        stx VMADDL
        ldx #$1fff
        ldy #64
        jsr print_memory


        ldx #$1000+(32*10)
        stx VMADDL
        CALL(printf,test_str,$12,$ee)

        stz NMITIMEN
        lda #$01
        sta BGMODE
        lda #$01
        sta TM
        lda #$0f
        sta INIDISP
                
brk_loop:
        bra brk_loop


.endproc

stack_str:
.asciiz "stack:"
zpage_str:
.asciiz "zpage:"
watch1_str:
.asciiz "watch1:"
test_str:
.asciiz "foo:%x%X"


