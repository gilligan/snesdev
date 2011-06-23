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
