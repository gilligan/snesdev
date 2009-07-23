.include "header.inc"
.include "regs.inc"
.include "std.inc"

.SEGMENT "ZEROPAGE"

        ; careful with zeropage!
        ; it is used to access arguments
        ; on stack in subroutines !

.SEGMENT "BSS"

        bg1:     .tag bg_t
        bg2:     .tag bg_t
        bg3:     .tag bg_t
        display: .tag display_t

.SEGMENT "HIRAM" : FAR

.SEGMENT "BANK0"

reset:
        native_init
        init_snes

        .a8
        .i16
        .smart

        INIT_BG(bg1,#$0000,#$1000,#$00,#$0000,#$0000)

        ; load cgram data
        load_pal colors,$00,4

        ; BG1/BG2/BG3/BG4 char @ $0000
        ; BG1 map  @ $1000

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


        ; copy 4bpp font tiles
        ldx #$0000
        stx VMADDL
        FCALL(g_dma_tag_2,font1_dma)

        ; copy 2bpp font tiles
        ldx #$3000
        stx VMADDL
        FCALL(g_dma_tag_2,font2_dma)

        ; print text on BG1
        ldx #$1000
        stx VMADDL
        FCALL(puts,hello_world1)

        ; print text on BG2
        ldx #$1800+(32*2)
        stx VMADDL
        FCALL(puts,hello_world2)

        ; print text on BG3
        ldx #$2000+(2*32*2)
        stx VMADDL
        FCALL(puts,hello_world3)

        ; set mode 0
        ; enable bg1
        lda #$01
        sta BGMODE

        lda #$07
        sta TM

        ; full brightness
        lda #$0f
        sta INIDISP

        ; enable NMI and
        ; automatic joypad reading
        lda #$81
        sta $4200
        cli

        stz $0000
        stz $0001

main_loop: 

        jmp main_loop

quit:
        jsl __swc_return_to_bios


nmi:

        phb
        pha
        phx
        phy
        phd


        jsl process_joypads
        lda joy1_held
        beq :+

        inc $0000
        lda $0000
        sta BG1VOFS
:

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


;
; screen setup and dma transfers
;

DMA_TRANSFER_TAG(font1_dma,1,$18,font1,^font1,font_end1-font1)
DMA_TRANSFER_TAG(font2_dma,1,$18,font2,^font2,font_end2-font2)

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


font1: .incbin "font4bpp.bin"
font_end1:

font2: .incbin "font2bpp.bin"
font_end2:

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

