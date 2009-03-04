.segment "HEADER"

        .res $15,0
        .byte $20
        .byte 1,8,3,0,0,0
        .word $AAAA,$5555

.segment "VECTORS"
        .res $1C
        .word reset   ; reset

