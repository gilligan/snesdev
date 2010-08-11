;
; Startup code for cc65 (ATARI version)
;
; Contributing authors:
;	Mark Keates
;	Freddy Offenga
;	Christian Groessler
;

	.export		_exit
        .export         __STARTUP__ : absolute = 1      ; Mark as startup
	.constructor	initsp, 26

	.import		initlib, donelib, callmain
       	.import	       	zerobss, pushax
	.import		_main, __filetab, getfd
	.import		__STARTUP_LOAD__, __ZPSAVE_LOAD__
	.import		__RESERVED_MEMORY__
.ifdef	DYNAMIC_DD
	.import		__getdefdev
.endif

        .include        "zeropage.inc"
	.include	"atari.inc"
        .include        "_file.inc"

; ------------------------------------------------------------------------
; EXE header

	.segment "EXEHDR"
	.word	$FFFF
	.word	__STARTUP_LOAD__
	.word	__ZPSAVE_LOAD__ - 1

; ------------------------------------------------------------------------
; Actual code

	.segment	"STARTUP"

	rts	; fix for SpartaDOS / OS/A+
		; they first call the entry point from AUTOSTRT and
		; then the load addess (this rts here).
		; We point AUTOSTRT directly after the rts.

; Real entry point:

; Save the zero page locations we need

       	ldx	#zpspace-1
L1:	lda	sp,x
	sta	zpsave,x
	dex
	bpl	L1

; Clear the BSS data

	jsr	zerobss

; setup the stack

	tsx
	stx	spsave

; report memory usage

	lda	APPMHI
	sta	appmsav			; remember old APPMHI value
	lda	APPMHI+1
	sta	appmsav+1

	sec
	lda	MEMTOP
	sbc	#<__RESERVED_MEMORY__
	sta	APPMHI			; initialize our APPMHI value
	lda	MEMTOP+1
	sbc	#>__RESERVED_MEMORY__
	sta	APPMHI+1

; Call module constructors

	jsr	initlib
.ifdef	DYNAMIC_DD
	jsr	__getdefdev
.endif

; set left margin to 0

	lda	LMARGN
	sta	old_lmargin
	lda	#0
	sta	LMARGN

; set keyb to upper/lowercase mode

	ldx	SHFLOK
	stx	old_shflok
	sta	SHFLOK

; Initialize conio stuff

	lda	#$FF
	sta	CH

; set stdio stream handles

	lda	#0
	jsr	getfd
       	sta    	__filetab + (0 * .sizeof(_FILE)); setup stdin
	lda	#0
	jsr	getfd
	sta	__filetab + (1 * .sizeof(_FILE)); setup stdout
	lda	#0
	jsr	getfd
	sta	__filetab + (2 * .sizeof(_FILE)); setup stderr

; Push arguments and call main

	jsr	callmain

; Call module destructors. This is also the _exit entry.

_exit:	jsr	donelib		; Run module destructors

; Restore system stuff

	ldx	spsave
	txs	       		; Restore stack pointer

; restore left margin

	lda	old_lmargin
	sta	LMARGN

; restore kb mode

	lda	old_shflok
	sta	SHFLOK

; restore APPMHI

	lda	appmsav
	sta	APPMHI
	lda	appmsav+1
	sta	APPMHI+1

; Copy back the zero page stuff

	ldx	#zpspace-1
L2:	lda	zpsave,x
	sta	sp,x
	dex
	bpl	L2

; turn on cursor

	inx
	stx	CRSINH

; Back to DOS

	rts

; *** end of main startup code

; setup sp

.segment        "INIT"

initsp:
	lda	APPMHI
	sta	sp
	lda	APPMHI+1
	sta	sp+1
	rts

.segment        "ZPSAVE"

zpsave:	.res	zpspace

	.bss

spsave:		.res	1
appmsav:	.res	1
old_shflok:	.res	1
old_lmargin:	.res	1

	.segment "AUTOSTRT"
	.word	RUNAD			; defined in atari.h
	.word	RUNAD+1
	.word	__STARTUP_LOAD__ + 1
