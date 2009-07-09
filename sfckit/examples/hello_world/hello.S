.include "header.inc"
.include "regs.inc"
.include "std.inc"

#define SADDR $00e000

.SEGMENT "BANK0"

reset:

        native_init
        init_snes

        lda #$01
        sta JOYSER1

        .a8
        .i16
        .smart


        load_pal colors,$00,2
        ldx BG_MAP_ADDR($1000)

        ; BG1 char @ $0000
        ; BG1 map  @ $1000

        ldx BG1_CHR_ADDR($0000)
        stx BG12NBA
        ldx BG_MAP_ADDR($1000)
        stx BG1SC

        ; copy font tiles

        ldx #$0000
        stx VMADDL
        g_dma_transfer font, $18, font_end-font
        print_string hello_world,$1000

        ; set mode 0
        ; enable bg1
        lda #$01
        sta BGMODE
        sta TM

        ; full brightness
        lda #$0f
        sta INIDISP

        lda #$80
        sta $4200
        cli

main_loop: 

        jsl serial_read_byte_noblock
        cmp #'q'
        beq quit

        jmp main_loop


quit:
        jsl swc_return_to_bios


nmi:

        phb
        pha
        phx
        phy
        phd

        inc $00
        lda $00
        sta BG1VOFS

        nop
        nop
        nop
        lda $4210

        pld
        ply
        plx
        pla
        plb

        rti

hello_world:
        .asciiz "SFCKIT: HELLO WORLD!"

colors:
        .byte $00,$00
        .byte $ff,$7f

font:
.incbin "font.bin"
font_end:

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

