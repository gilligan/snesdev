.include "header.inc"
.include "regs.inc"
.include "std.inc"

.SEGMENT "BANK0"

reset:

        sei
        clc
        xce

        rep #$38
        .a8
        .i16
        ldx #$1fff    ; init stack
        txs
        phk
        plb
        lda #$0000
        tcd
        sep #$20

        .a8
        .i16

        lda #$80
        sta $2100

        init_snes

        lda #$80
        sta $2100

        stz $210b
        stz $2107

        ldx #$0000
        stx $2116

        ldx #$1000
@clrlp:
        ldy #$00
        sty $2118
        dex
        bne @clrlp

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

