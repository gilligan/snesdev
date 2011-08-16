
;;
;; macros and defines for custom bsnes
;; chip link debugging chip/library
;;

.define REG_LINK_WR   $3800
.define REG_LINK_RD   $3801
.define REG_LINK_CMD  $3802
.define REG_LINK_PUTC $3803

.macro DEBUG
        brk
        nop
        nop
.endmacro

.struct debug_data
        status .byte
        pc_lo  .byte
        pc_hi  .byte
        db     .byte
        reg_a  .word
        reg_x  .word
        reg_y  .word
        reg_sp .word
.endstruct

.global register_data
.SEGMENT "BSS"
        register_data: .tag debug_data

.macro link_dump_registers
        sta register_data+debug_data::reg_a
        stx register_data+debug_data::reg_x
        sty register_data+debug_data::reg_y

        lda 1,S
        sta register_data+debug_data::status
        sta $3803
        lda 2,S
        sta register_data+debug_data::pc_lo
        sta $3803
        lda 3,S
        sta register_data+debug_data::pc_hi
        sta $3803
        lda 4,s
        sta register_data+debug_data::db

        tsx
        stx register_data+debug_data::reg_sp
.endmacro

.macro link_tx_registers
        ldx #$0000
        :
        lda register_data,x
        sta $3800
        inx
        cpx #.sizeof(debug_data)
        bne :-
        lda #$02
        sta $3802
.endmacro


