.include "regs.inc"
.include "std.inc"

.SEGMENT "STDLIB"

        .a8
        .i16
        .smart

; config_bg(bg_t* bg)
;
; writes BG configuration to registers according
; to info stored in [bg]
;
.macro config_bg

        bg = ARG_1

        PROC_PROLOGUE

        
        PROC_EPILOGUE
.endmacro
