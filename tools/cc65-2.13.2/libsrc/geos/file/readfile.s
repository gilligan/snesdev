
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char ReadFile  (struct tr_se *myTS, char *buffer, int length);

	    .export _ReadFile
	    .import popax, setoserror
	    .import gettrse

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ReadFile:
	sta r2L
	stx r2H
	jsr popax
	sta r7L
	stx r7H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr ReadFile
	jmp setoserror
