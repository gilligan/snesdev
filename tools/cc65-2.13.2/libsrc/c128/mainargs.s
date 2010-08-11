; mainargs.s
;
; Ullrich von Bassewitz, 2003-03-07
; Based on code from Stefan A. Haubenthal, <polluks@web.de>
; 2003-05-18, Greg King
; 2004-04-28, 2005-02-26, Ullrich von Bassewitz
;
; Scan a group of arguments that are in BASIC's input-buffer.
; Build an array that points to the beginning of each argument.
; Send, to main(), that array and the count of the arguments.
;
; Command-lines look like these lines:
;
; run
; run : rem
; run:rem arg1 " arg 2 is quoted "  arg3 "" arg5
;
; "run" and "rem" are entokenned; the args. are not.  Leading and trailing
; spaces outside of quotes are ignored.
;
; TO-DO:
; - The "file-name" might be a path-name; don't copy the directory-components.
; - Add a control-character quoting mechanism.

	.constructor	initmainargs, 24
	.import		__argc, __argv

	.include	"c128.inc"


MAXARGS	 = 10                   ; Maximum number of arguments allowed
REM	 = $8f			; BASIC token-code
NAME_LEN = 16			; maximum length of command-name


; Get possible command-line arguments. Goes into the special INIT segment,
; which may be reused after the startup code is run

.segment        "INIT"

initmainargs:

; Assume that the program was loaded, a moment ago, by the traditional LOAD
; statement.  Save the "most-recent filename" as argument #0.
; Because the buffer, that we're copying into, was zeroed out,
; we don't need to add a NUL character.
;
	ldy	FNAM_LEN
	cpy	#NAME_LEN + 1
	bcc	L1
	ldy	#NAME_LEN - 1	; limit the length
L0:     lda     #FNAM           ; Load vector address for FETCH routine
        ldx     FNAM_BANK       ; Load bank for FETCH routine
        jsr     INDFET          ; Load byte from (FETVEC),y
	sta	name,y          ; Save byte from filename
L1:	dey
	bpl	L0
	inc	__argc	   	; argc always is equal to, at least, 1

; Find the "rem" token.
;
	ldx	#0
L2:	lda	BASIC_BUF,x
       	beq    	done   	       	; no "rem," no args.
	inx
	cmp	#REM
	bne	L2
	ldy	#1 * 2

; Find the next argument

next:   lda     BASIC_BUF,x
        beq     done            ; End of line reached
        inx
        cmp     #' '            ; Skip leading spaces
        beq     next            ;

; Found start of next argument. We've incremented the pointer in X already, so
; it points to the second character of the argument. This is useful since we
; will check now for a quoted argument, in which case we will have to skip this
; first character.

found:  cmp     #'"'            ; Is the argument quoted?
        beq     setterm         ; Jump if so
        dex                     ; Reset pointer to first argument character
        lda     #' '            ; A space ends the argument
setterm:sta     term            ; Set end of argument marker

; Now store a pointer to the argument into the next slot. Since the BASIC
; input buffer is located at the start of a RAM page, no calculations are
; necessary.

        txa                     ; Get low byte
	sta	argv,y		; argv[y]= &arg
	iny
	lda	#>BASIC_BUF
	sta	argv,y
	iny
        inc     __argc          ; Found another arg

; Search for the end of the argument

argloop:lda     BASIC_BUF,x
        beq     done
        inx
        cmp     term
        bne     argloop

; We've found the end of the argument. X points one character behind it, and
; A contains the terminating character. To make the argument a valid C string,
; replace the terminating character by a zero.

        lda     #0
        sta     BASIC_BUF-1,x

; Check if the maximum number of command line arguments is reached. If not,
; parse the next one.

        lda     __argc          ; Get low byte of argument count
        cmp     #MAXARGS        ; Maximum number of arguments reached?
        bcc     next            ; Parse next one if not

; (The last vector in argv[] already is NULL.)

done:	lda	#<argv
	ldx	#>argv
	sta	__argv
	stx	__argv + 1
	rts

; These arrays are zeroed before initmainargs is called.
; char	name[16+1];
; char* argv[MAXARGS+1]={name};
;
.bss
term:	.res	1
name:	.res	NAME_LEN + 1

.data
argv:   .addr   name
        .res   	MAXARGS * 2

