.include "header.inc"

.SEGMENT "BANK0"
reset:

        .i16
        .a8

	sei                      ; disable inerrupts
	clc                      ; clear carry bit
	xce                      ; native mode

	rep     #$38             ; clear decimal mode/ accu = index = 16bit
	ldx     #$1fff             
	txs                      ; set up stack
	phk
	plb                      ; set up bank register
	lda     #$0000
	tcd                      ; set up direct page
	sep     #$20             ; accu = 8bit
        
  
main_loop: 
jmp main_loop

nmi:
        nop
        nop
        nop
        rti

.segment "BANK1"
        nop
.segment "BANK2"
        nop
.segment "BANK3"
        nop
.segment "BANK4"
        nop
.segment "BANK5"
        nop
.segment "BANK6"
        nop
.segment "BANK7"
        nop

