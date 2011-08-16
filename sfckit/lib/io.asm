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
.global printf
.global s_puts
.global s_putsl
.global l_puts
.global l_dump_mem
.global __byte_to_ascii

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

; l_puts(u16* src)
;
; prints ascii characters at [src] up
; to the first '\0' on chip-link output

.proc l_puts

        src = ARG_1
        PROC_PROLOGUE

        ldy #$0000
puts_loop:
        lda (src),y
        beq puts_done
        sta $3800
        iny
        jmp puts_loop
puts_done:
        stz $3802

        PROC_EPILOGUE
.endproc


; s_puts(u16* src)
;
; prints ascii characters at [src] up
; to the first '\0' on serial
.proc s_puts

        src = ARG_1

        PROC_PROLOGUE

        ldy #$0000
puts_loop:
        lda (src),y
        beq str_end
        jsr write_serial_57600
        iny
        jmp puts_loop
str_end:

        PROC_EPILOGUE

.endproc

; s_putsl(u16* src)
;
; prints ascii characters at [src] up
; to the first '\0' on serial adding "\n"
.proc s_putsl

        src = ARG_1

        PROC_PROLOGUE

        ldy #$0000
puts_loop:
        lda (src),y
        beq str_end
        jsr write_serial_57600
        iny
        jmp puts_loop
str_end:
        lda #13
        jsr write_serial_57600
        lda #10
        jsr write_serial_57600

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


.proc l_dump_mem

        start_ofs = ARG_1
        data_len  = ARG_2

        PROC_PROLOGUE

        ldy #$0000
        :
        lda (start_ofs),y
        sta $3800
        iny
        cpy data_len
        bne :-

        lda #$01
        sta $3802

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

; Serial in/out at 57600 bps for PAL SNES

; Which controller to receive data on. Defaults to second.
; Use $4016 for first.
.ifndef SERIAL_PORT
	SERIAL_PORT = $4017
.endif

; All routines require that A be in 8-bit mode (sep #$20)

; Initializes serial output
; Preserved: X, Y
init_serial_57600:
	
	; Enable H IRQ
	lda #147
	sta $4207	; HTIMEL
	stz $4208	; HTIMEH
	lda #$10
	sta $4200	; NMITIMEN
	
	; Keep in idle state for a while, to avoid junk
	; byte at beginning.
	lda #130 | 1
	sta $4016
:	dec
	bne :-
	
	rts

; Sends byte
; In: A = value
; Preserved: X, Y
write_serial_57600:	; 46 JSR
	bit $4211	; TIMEUP
	wai
	
	stz $4016	; 36 start bit
	
	sec		; 50 stop bit at end
	bit $4016
	
	; 357.5 clocks per bit
@next:	pha		; 272
	lda #5		; A*36
:	dec a
	bne :-
	pla
	ora #0
	ora #0
	
	sta $4016	; 86	
	ror a
	clc
	bne @next
			; -6
	
	; Be sure stop bit lasts long enough
	phd
	pld
	phd
	pld
	phd
	pld
	
	rts		; 42

; Waits for and receives byte
; Out: A = value
; Preserved: X, Y
read_serial_57600:	; 46 JSR
	; Wait for start bit
	lda #1
:	bit SERIAL_PORT	; wait for idle
	bne :-
:	bit SERIAL_PORT	; 36 wait for start
	beq :-		; 22
	
	; average 45 clocks after start bit
	; need 490 delay to first bit
	; need 210 more
	
	phd
	pld
	phd
	pld
	xba
	xba
	sep #0
	
	; Read 8 data bits into A
	lda #$80	; A serves as counter AND buffer
:	phd		; 206 delay
	pld
	phd
	pld
	pha
	pla
	bit <0
	pha		; 22
	lda #$01	; 16 read bit, inverted into C
	eor SERIAL_PORT	; 36
	lsr a		; 14
	pla		; 28
	ror a		; 14 put into top of A
	bcc :-		; 22 loop until initial bit 7 shifts out
	
	rts
