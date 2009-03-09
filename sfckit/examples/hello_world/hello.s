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

        lda #$01
        sta $2105
        sta $212c
        lda #$0f
        sta $2100


main_loop: 
jmp main_loop

nmi:
        nop
        nop
        nop
        rti

color_test:
        .byte $ff,$7f
        .byte $ff,$7f

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

