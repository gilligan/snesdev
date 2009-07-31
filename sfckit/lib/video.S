.include "std.inc"

.SEGMENT "CODE"

        .a8
        .i16
        .smart

        .global config_bg


; config_bg(bg_t* bg)
;
; writes BG configuration to registers according
; to info stored in [bg]
;
config_bg:

        bg = ARG_1

        PROC_PROLOGUE

        ldx #$0000
        txa
        xba
        lda #$00
        lda bg
        asl
        tax
        jmp (.loword(cfg_jmp_table),x)

conf_bg12:
        ldy #$0001
        
        

conf_bg34:

cfg_jmp_table:
        .word .loword(conf_bg12)
        .word .loword(conf_bg34)


        PROC_EPILOGUE



