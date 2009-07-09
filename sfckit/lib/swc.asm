.include "regs.inc"
.SEGMENT "STDLIB"

.smart

.global swc_return_to_bios

.define SWC_ENTRY_ADDR $00e4e0
.define SWC_RAM_BUFFER $7e0100

.proc swc_return_to_bios

        ldx #$0000
copy_to_ram:
        lda swc_boot_code,x
        sta SWC_RAM_BUFFER,x
        inx
        cpx #(swc_boot_code_end-swc_boot_code)
        bne copy_to_ram

        jmp SWC_RAM_BUFFER

swc_boot_code:

        lda #$ff
        sta $e004  ; switch to swc mode 0

        nop
        nop
        nop        ; wait some cycles

                   ; initialize SWC firmware
        sei
        clc
        xce
        phk
        plb
        rep #$20
        lda #$01ff
        tcs
        sep #$30
        stz $c008
        stz $420b
        stz $420c
        stz $420d
        lda #$80
        sta $2100
        lda #$01
        sta $4200
        jsl $10eb24
        jsl $19e7e8
        jsl $0df453

        jmp SWC_ENTRY_ADDR

swc_boot_code_end:

.endproc
