
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char InsertRecord  (void);

	    .import setoserror
	    .export _InsertRecord

	    .include "../inc/jumptab.inc"

_InsertRecord:
	jsr InsertRecord
	jmp setoserror
