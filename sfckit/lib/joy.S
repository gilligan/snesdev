.include "regs.inc"
.include "std.inc"

        .a8
        .i16
        .smart

        .global process_joypads
        .global joy1_prev, joy1_held, joy1_down
        .global joy2_prev, joy2_held, joy2_down

.SEGMENT "BSS"

        ignore_joy1: .res 1
        ignore_joy2: .res 1

        joy1_prev: .res 2
        joy1_held: .res 2
        joy1_down: .res 2
        joy2_prev: .res 2
        joy2_held: .res 2
        joy2_down: .res 2

.SEGMENT "STDLIB"



;
; macro to store joypad button status
; code by: mukunda
;
.macro readjoy prev, held, down, reg
	lda	reg		; read joypad register
	bit	#0Fh		; catch non-joypad input
	beq	:+		; (bits 0-3 should be zero)
	lda	#0		;-------------------------------------
:	sta	held		; store 'held' state
	eor	prev		; compute 'down' state from bits that
	and	held		; have changed from 0 to 1
	sta	down		;
.endmacro			;


;
; processes joypad inputs and
; stores info in ram
;
; CAUTION: Do not call this outside of NMI
;          since its fiddling with 8/16bit accu 
;
.proc process_joypads

        PROC_PROLOGUE

	ldy	joy1_held	; copy joy states
	sty	joy1_prev	;
	ldy	joy2_held	;
	sty	joy2_prev	;-------------------------------------
:	lda	SLHV	        ; wait until past lines 224,225
	lda	OPHCT	        ;
	cmp	#224		;
	beq	:-		;
	cmp	#225		;
	beq	:-		;-------------------------------------
:	lda	HVBJOY	        ; wait until joypads are ready
	lsr			;
	bcs	:-		;-------------------------------------
	rep	#20h		; read joypads


;        lda ignore_joy1
;        beq skip_joy1
	readjoy	joy1_prev, joy1_held, joy1_down, JOY1L
;skip_joy1:

;        lda ignore_joy2
;        beq skip_joy2
	readjoy	joy2_prev, joy2_held, joy2_down, JOY2L
skip_joy2:

	sep	#20h		; return

        PROC_EPILOGUE

.endproc

