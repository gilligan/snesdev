
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char AppendRecord  (void);

	    .import setoserror
	    .export _AppendRecord

	    .include "../inc/jumptab.inc"
	
_AppendRecord:

	jsr AppendRecord
	jmp setoserror
