.include "header.inc"
.include "regs.inc"
.include "std.inc"

.SEGMENT "BANK0"

reset:
        sei
        clc
        xce           ; native mode

        native_init
        init_snes

        .a8
        .i16

        load_pal color_test,$00,2

        ; BG1 char @ $0000
        ; BG1 map  @ $1000
        ldx #$0000
        stx BG12NBA
        ldx #($1000>>8)
        stx BG1SC

        ; copy font tiles
        ldx #$0000
        stx VMADDL
        g_dma_transfer font, $18, font_end-font

        ldx #$1000
        stx VMADDL
        print_string hello_world

        ; set mode 0
        ; enable bg1
        lda #$01
        sta BGMODE
        sta TM

        ; full brightness
        lda #$0f
        sta INIDISP


main_loop: 
jmp main_loop

nmi:
        nop
        nop
        nop
        rti

hello_world:
        .asciiz "SFCKIT: HELLO WORLD!"

color_test:
        .byte $00,$00
        .byte $ff,$7f

font:
.incbin "font.bin"
font_end:

.segment "BANK1"
        nop
.segment "BANK2"
        nop
.segment "BANK3"
        nop
.segment "BANK4"
        nop
.segment "BANK5"
        nop
.segment "BANK6"
        nop
.segment "BANK7"
        nop

