
; X = memory addr
; Y = number of bytes
.proc print_memory

        lda $00,x

        lsr
        lsr
        lsr
        lsr ; get first digit

        clc
        pha
        sbc #11
        bmi val1_below_10

        pla
        clc
        adc #55
        jmp val1_done

val1_below_10:
        
        pla
        clc
        adc #48

val1_done:

       sta VMDATAL
       stz VMDATAH 

       lda $00,x
       and #$0f ; get second digit

       clc
       pha
       sbc #11
       bmi val2_below_10

       pla
       clc
       adc #55
       jmp val2_done

val2_below_10:
       pla
       clc
       adc #48

val2_done:

      sta VMDATAL
      stz VMDATAH

      inx
      dey
      bne print_memory

      rts
.endproc

