.include "header.inc"
.include "regs.inc"
.include "std.inc"

.global main
.global mem_pool


.SEGMENT "ZEROPAGE"

        ; careful with zeropage!
        ; it is used to access arguments
        ; on stack in subroutines !

.SEGMENT "BSS"

        mem_pool:   .res 64  

.SEGMENT "HRAM" : FAR
.SEGMENT "HRAM2" : FAR
.SEGMENT "HDATA" : FAR
.SEGMENT "XCODE"

.SEGMENT "CODE"

start:

        init_snes

        .a8
        .i16
        .smart

        ;;
        ;; all the following code is by
        ;; blargg - file was posted
        ;; on the nesdev forums snesdev sub-forum
        ;;

        jsr spc_wait_boot
        
        ; Upload code to SPC at $200
        ldy #$0200
        jsr spc_begin_upload ; sets Y to 0
loop:   lda spc_code,y
        jsr spc_upload_byte ; increments Y
        cpy #spc_code_end - spc_code
        bne loop
        
        ; Start executing at $204
        ldy #$0204
        jsr spc_execute
        
forever:
        jmp forever

; Code to upload to SPC
spc_code:
;.org $200
;
;directory:
.byte  $07,$02
.byte  $07,$02

.byte  $5F,$19,$02 ; jmp   !$0219

;sample:
;sample_loop:
.byte $b0,$78,$78,$78,$78,$78,$78,$78,$78
.byte $b3,$78,$78,$78,$78,$78,$78,$78,$78

.byte  $00         ; nop
.byte  $00         ; nop
.byte  $8F,$6C,$F2 ; mov   $f2, #$6c
.byte  $8F,$20,$F3 ; mov   $f3, #$20
.byte  $8F,$4C,$F2 ; mov   $f2, #$4c
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$5C,$F2 ; mov   $f2, #$5c
.byte  $8F,$FF,$F3 ; mov   $f3, #$ff
.byte  $8F,$5D,$F2 ; mov   $f2, #$5d
.byte  $8F,$02,$F3 ; mov   $f3, #$02
.byte  $8F,$00,$F2 ; mov   $f2, #$00
.byte  $8F,$7F,$F3 ; mov   $f3, #$7f
.byte  $8F,$01,$F2 ; mov   $f2, #$01
.byte  $8F,$7F,$F3 ; mov   $f3, #$7f
.byte  $8F,$02,$F2 ; mov   $f2, #$02
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$03,$F2 ; mov   $f2, #$03
.byte  $8F,$02,$F3 ; mov   $f3, #$02
.byte  $8F,$04,$F2 ; mov   $f2, #$04
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$05,$F2 ; mov   $f2, #$05
.byte  $8F,$C3,$F3 ; mov   $f3, #$c3
.byte  $8F,$06,$F2 ; mov   $f2, #$06
.byte  $8F,$2F,$F3 ; mov   $f3, #$2f
.byte  $8F,$07,$F2 ; mov   $f2, #$07
.byte  $8F,$CF,$F3 ; mov   $f3, #$cf
.byte  $8F,$5C,$F2 ; mov   $f2, #$5c
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$3D,$F2 ; mov   $f2, #$3d
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$4D,$F2 ; mov   $f2, #$4d
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$0C,$F2 ; mov   $f2, #$0c
.byte  $8F,$7F,$F3 ; mov   $f3, #$7f
.byte  $8F,$1C,$F2 ; mov   $f2, #$1c
.byte  $8F,$7F,$F3 ; mov   $f3, #$7f
.byte  $8F,$2C,$F2 ; mov   $f2, #$2c
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$3C,$F2 ; mov   $f2, #$3c
.byte  $8F,$00,$F3 ; mov   $f3, #$00
.byte  $8F,$4C,$F2 ; mov   $f2, #$4c
.byte  $8F,$01,$F3 ; mov   $f3, #$01
.byte  $5F,$C0,$FF ; jmp   !$ffc0
spc_code_end:

; SPC code that generated the above
; (wdsp writes value to dsp register)
;
; wdsp flg,$20
; wdsp kon,0
; wdsp koff,$FF
; wdsp dir,>directory
;
; wdsp vvoll,$7F
; wdsp vvolr,$7F
; wdsp vpitchl,$00
; wdsp vpitchh,$02
; wdsp vsrcn,0
; wdsp vadsr0,$C3
; wdsp vadsr1,$2F
; wdsp vgain,$CF
;
; wdsp koff,0
; wdsp non,0
; wdsp eon,0
; wdsp mvoll,$7F
; wdsp mvolr,$7F
; wdsp evoll,0
; wdsp evolr,0
;
; wdsp kon,$01
;
; jmp !$FFC0



; High-level interface to SPC-700 bootloader
;
; 1. Call spc_wait_boot
; 2. To upload data:
;       A. Call spc_begin_upload
;       B. Call spc_upload_byte any number of times
;       C. Go back to A to upload to different addr
; 3. To begin execution, call spc_execute
;
; Have your SPC code jump to $FFC0 to re-run bootloader.
; Be sure to call spc_wait_boot after that.


; Waits for SPC to finish booting. Call before first
; using SPC or after bootrom has been re-run.
; Preserved: X, Y
spc_wait_boot:
        lda #$AA
:       cmp APUIO0
        bne :-
        
        ; Clear in case it already has $CC in it
        ; (this actually occurred in testing)
        sta APUIO0
        
        lda #$BB
:       cmp APUIO1
        bne :-
        
        rts


; Starts upload to SPC addr Y and sets Y to
; 0 for use as index with spc_upload_byte.
; Preserved: X
spc_begin_upload:
        sty APUIO2
        
        ; Send command
        lda APUIO0
        clc
        adc #$22
        bne :+  ; special case fully verified
        inc
:       sta APUIO1
        sta APUIO0
        
        ; Wait for acknowledgement
:       cmp APUIO0
        bne :-
        
        ; Initialize index
        ldy #0
        
        rts


; Uploads byte A to SPC and increments Y. The low byte
; of Y must not be disturbed between bytes.
; Preserved: X
spc_upload_byte:
        sta APUIO1
        
        ; Signal that it's ready
        tya
        sta APUIO0
        iny
        
        ; Wait for acknowledgement
:       cmp APUIO0
        bne :-
        
        rts



; Starts executing at SPC addr Y
; Preserved: X, Y
spc_execute:
        sty APUIO2
        
        stz APUIO1
        
        lda APUIO0
        clc
        adc #$22
        sta APUIO0
        
        ; Wait for acknowledgement
:       cmp APUIO0
        bne :-
        
        rts

brk_handler:
irq_handler:
nmi:
        rti

