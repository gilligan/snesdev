;
; includes
;
.include "std.inc"

        .a8
        .i16
        .smart

;
; function exports
;
.global bzero
.global print_memory
.global dump_mem_ascii
.global puts
.global __byte_to_ascii
.global printf

;
; variable exports
;
.global hex_ascii


.SEGMENT "BSS"

        hex_ascii: .res 2

.SEGMENT "CODE"


; printf(u16* str,...)
;
; prints the format string [str] interpreting the following
; syntax:
;           %P : print hex *word
;           %p : print hex *byte
;           %X : print hex word
;           %x : print hex byte
;
.proc printf

        str = ARG_1
        PROC_PROLOGUE


                ldy #$0000
                ldx #$0000

parse_string:
                lda (str),y
                bne :+
                jmp end_of_string
:
                cmp #'%'
                beq check_format

                sta $2118
                stz $2119
                iny
                bra parse_string

check_format:
                iny
                lda (str),y         ; get format specifier
                bne :+              ; continue if != 0
                jmp end_of_string   ; hit '\0' , exit!
:

                cmp 'P'             ; print *word ?
                beq print_word_ptr

                cmp 'p'             ; print *byte ?
                beq print_byte_ptr

                cmp 'X'
                beq print_word

                cmp 'x'
                beq print_byte

                iny                 ; invalid format specifier
                bra parse_string    ; just skip it!


print_word:
                rep #$38
                .a16
                lda ARG_2,x
                xba
                pha
                sep #$20
                .a8

                pla
                jsr __print_byte
                pla
                jsr __print_byte

                inx
                inx
                iny
                bra parse_string

print_byte:
                lda ARG_2,x
                jsr __print_byte
                
                inx
                inx
                iny
                bra parse_string

print_word_ptr:
                rep #$38
                .a16
                lda (ARG_2,x)
                swa
                pha
                sep #$20
                .a8

                pla
                jsr __print_byte
                pla
                jsr __print_byte

                inx
                inx
                iny
                bra parse_string

print_byte_ptr:
                
                lda (ARG_2,x)
                jsr __print_byte

                inx
                inx
                iny
                bra parse_string

;                
; ------------------------------
;
__print_byte:

                pha                 ; save input

                lsr
                lsr
                lsr
                lsr                 ; get first digit

                cmp #10
                blt val_below_10
                clc
                adc #55
                jmp :+

val_below_10:

                clc
                adc #48
:
                sta $2118
                stz $2119

                pla                 ; restore input
                and #$0f            ; get second digit

                cmp #10
                blt val2_below_10

                clc
                adc #55
                jmp :+

val2_below_10:

                clc
                adc #48
:
                sta $2118
                stz $2119

                rts


end_of_string:

        PROC_EPILOGUE

.endproc


; bzero(u16* dst, u16 len)
;
; sets the first [len] bytes of the memory
; region starting at [dst] to 0
.proc bzero

        src = ARG_1
        len = ARG_2

        PROC_PROLOGUE

        ldy #$0000
        lda #$00
zloop:
            sta (src),y
            iny
            tya
            cmp (len)
            bne zloop

        PROC_EPILOGUE
        
.endproc



; puts(u16* src)
;
; prints ascii characters at [src] up
; to the first '\0'
.proc puts

        src = ARG_1

        PROC_PROLOGUE

        ldy #$0000
puts_loop:
        lda (src),y
        beq str_end
        sta VMDATAL
        stz VMDATAH
        iny
        jmp puts_loop
str_end:

        PROC_EPILOGUE

.endproc

; dump_mem_ascii(u16* src, u16* dst, u16 len)
;
; reads [len] bytes of data from [src] and stores
; corresponding ascii representation at [dst]
;
.proc dump_mem_ascii

        src = ARG_1
        dst = ARG_2
        len = ARG_3

        PROC_PROLOGUE

        ldy #$0000
        ldx #$0000

dump_loop:


        lda (src),y
        iny                  ; move index
        jsr __byte_to_ascii

        phy                  ; push src index

        txy                  ; y = dst index
        lda hex_ascii
        sta (dst),y          ; store first digit
        iny
        lda hex_ascii+1
        sta (dst),y          ; store second digit
        iny
        tyx

        ply                  ; pop src index

        tya
        cmp (len)
        bne dump_loop

        PROC_EPILOGUE
.endproc

;
; converts the byte in A to
; the corresponding ascii value
; IN:  A (byte value)
; OUT: 2 byte ascii representation in hex_ascii (zp)
;
.proc __byte_to_ascii

        phx                 ; save all registers 
        phy                 ; except A
        php
        phd

        tax                 ; save byte in X

        lsr
        lsr
        lsr
        lsr                 ; get first digit

        cmp #10
        blt val_below_10

        clc
        adc #55
        jmp :+

val_below_10:

        clc
        adc #48
:
        sta hex_ascii

        txa                 ; restore byte from X
        and #$0f            ; get second digit

        cmp #10
        blt val2_below_10

        clc
        adc #55
        jmp :+

val2_below_10:

        clc
        adc #48
:
        sta hex_ascii+1

        pld                 ; restore registers
        plp
        ply
        plx

        rtl

.endproc



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
        sbc #9
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

       pha
       clc
       sbc #9
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

