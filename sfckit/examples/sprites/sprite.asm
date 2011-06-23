.include "header.inc"
.include "std.inc"

.global main
.global mem_pool

.SEGMENT "ZEROPAGE"

        ; careful with zeropage!
        ; it is used to access arguments
        ; on stack in subroutines !

.SEGMENT "BSS"

        mem_pool:    .res 64  

.SEGMENT "HRAM" : FAR
.SEGMENT "HRAM2" : FAR

.SEGMENT "HDATA" : FAR

    oam_data:
            .repeat 128
            .byte 0,-32,0,0
            .endrepeat

            .repeat 32
            .byte 0
            .endrepeat

.SEGMENT "XCODE"

.SEGMENT "CODE"

main:
        .a8
        .i16
        .smart

        ; 
        ; set OBJ properties
        ;

        lda #OBSEL_32_64 | OBSEL_BASE($6000)
        sta OBSEL

        ;
        ; copy tiles & cgdata & OAM
        ;

        SET_VRAM_ADDR($6000)
        call g_dma_tag_0,chr_dma

        load_pal sprite_pal, $80, 16

        lda #100
        sta oam_data+OAM_XPOS
        sta oam_data+OAM_YPOS

        SET_OAM_ADDR($0000)
        call g_dma_tag_0,oam_dma


        ;
        ; config & enable display
        ;

        lda #MODE_0
        sta BGMODE

        lda #TM_OBJ
        sta TM

        lda #$0f
        sta INIDISP


        ;
        ; enable NMI/joypad reading
        ;

        lda #NMI_ON | AUTOJOY_ON
        sta NMITIMEN
        cli

main_loop: 

        jmp main_loop

quit:



brk_handler:
irq_handler:
        rti


nmi:

        phb
        pha
        phx
        phy
        phd



        jsr process_joypads


        lda joy1_held+1

        cmp BUTTON_RIGHT
        beq move_right

        cmp BUTTON_LEFT
        beq move_left

        cmp BUTTON_UP
        beq move_up

        cmp BUTTON_DOWN
        beq move_down

        jmp control_done

move_right:
        lda oam_data+OAM_XPOS
        inc
        sta oam_data+OAM_XPOS
        jmp control_done

move_left:
        lda oam_data+OAM_XPOS
        dec
        sta oam_data+OAM_XPOS
        jmp control_done

move_up:
        lda oam_data+OAM_YPOS
        dec
        sta oam_data+OAM_YPOS
        jmp control_done

move_down:
        lda oam_data+OAM_YPOS
        inc
        sta oam_data+OAM_YPOS

control_done:


        call g_dma_tag_0,oam_dma


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
oam_dma: build_dma_tag 0,$04,.loword(oam_data),^oam_data,OAM_SIZE
chr_dma: build_dma_tag 1,$18,.loword(char_data),^char_data,char_data_end-char_data

char_data:
.incbin "sprite.pic"
char_data_end:

sprite_pal:
.incbin "sprite.clr"
sprite_pal_end:
