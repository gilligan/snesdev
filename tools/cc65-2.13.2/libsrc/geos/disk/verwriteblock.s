
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char VerWriteBlock (struct tr_se *myTS, char *buffer);

	    .import popax, setoserror
	    .import gettrse
	    .export _VerWriteBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_VerWriteBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr VerWriteBlock
	jmp setoserror
