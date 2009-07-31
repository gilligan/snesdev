.include "std.inc"

.SEGMENT "CODE"

.global __serial_read_byte
.global __serial_read_byte_noblock
.global __serial_write_byte

;
; receive byte into A
; using serial connection over joypad port 2
; assumes 57600 8N1
; (code by blaarg!)
.proc __serial_read_byte

	; Wait for start bit
	lda #1
:       bit JOYSER1     ; wait for idle
	bne :-
:       bit JOYSER1     ; 36 wait for start
	beq :-          ; 22
	
	; Delay first read to middle of bit
	phd             ; 180 can reduce by up to 8 NOPs
	pld
	phd
	pld
	sep #0
	nop
	nop
	
	; Read 8 data bits into A
	lda #$80        ; A serves as counter AND buffer
:       phd             ; 212 delay
	pld
	phd
	pld
	phd
	pld
	nop
	pha             ; 22
	lda #$01        ; 16 read bit, inverted into C
	eor JOYSER1     ; 36
	lsr a           ; 14
	pla             ; 28
	ror a           ; 14 put into top of A
	bcc :-          ; 22 loop until initial bit 7 shifts out
	
	rtl
        
.endproc

.proc __serial_write_byte
	asl a           ; start bit=0
        phx
	
	; Send start bit (0), 8 data bits, stop bit (1)
	ldx #10
	; 366 master clocks per iter + ~10 for DRAM refresh
:       sta JOYSER1     ; 36
	ora #1          ; 16 stop bit=1
	ror a           ; 14
	
	phd             ; 264 delay
	pld             ; can adjust by +/-1 NOP
	phd
	pld
	phd
	pld
	phd
	pld
        nop
	
	dex             ; 14
	bne :-          ; 22
	
	plx
	rtl
.endproc


.proc __serial_read_byte_noblock

        lda #$00

        ldy #10
:       dey
        beq recv_timeout
	; Wait for start bit
	lda #1
        bit JOYSER1     ; wait for idle
	bne :-


        ldy #10
:       dey
        beq recv_timeout
        bit JOYSER1     ; 36 wait for start
	beq :-          ; 22

	
	; Delay first read to middle of bit
	phd             ; 180 can reduce by up to 8 NOPs
	pld
	phd
	pld
	sep #0
	nop
	nop
	
	; Read 8 data bits into A
	lda #$80        ; A serves as counter AND buffer
:       phd             ; 212 delay
	pld
	phd
	pld
	phd
	pld
	nop
	pha             ; 22
	lda #$01        ; 16 read bit, inverted into C
	eor JOYSER1     ; 36
	lsr a           ; 14
	pla             ; 28
	ror a           ; 14 put into top of A
	bcc :-          ; 22 loop until initial bit 7 shifts out
	
recv_timeout:

	rtl
        
.endproc


