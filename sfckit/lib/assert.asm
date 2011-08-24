.include "std.inc"
.a8
.i16
.smart

.global assert_mem_equal
.global assert_result

.segment "BSS"

        assert_result: .res 1

.segment "CODE"



; assert_mem_equal buffer1, buffer2, len
;
; @arg buffer1: addr of buffer
; @arg buffer2: addr of buffer
; @arg len:     buffer length
;
.proc assert_mem_equal

        buffer1 = ARG_1
        buffer2 = ARG_2
        len     = ARG_3

        PROC_PROLOGUE

        ldy len
        :
        dey
        bmi assertion_valid
        lda (buffer1),y
        cmp (buffer2),y
        beq :-

        ;
        ; assertion failed
        ;
        lda #$01
        sta assert_result
        jmp exit_proc


assertion_valid:
        ;
        ; assertion is valid
        ;
        stz assert_result

exit_proc:
        PROC_EPILOGUE
        
.endproc 
