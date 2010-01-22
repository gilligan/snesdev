.include "regs.inc"
.include "std.inc"
.include "sincos.inc"

.include "header.inc"

.global main
.global mem_pool
.global DEBUG_FONT_DMA,brk_info,debug_colors


.SEGMENT "ZEROPAGE"

        ; careful with zeropage!
        ; it is used to access arguments
        ; on stack in subroutines !

.SEGMENT "BSS"

        __M7_ANGLE: .res 2
        __M7_SX:    .res 2
        __M7_SY:    .res 2
        __M7_A:     .res 2
        __M7_B:     .res 2
        __M7_C:     .res 2
        __M7_D:     .res 2

.SEGMENT "HRAM" : FAR
.SEGMENT "HRAM2" : FAR
.SEGMENT "HDATA" : FAR
.SEGMENT "XCODE"
.SEGMENT "SOUNDBANK"

.SEGMENT "CODE"

main:
        .a8
        .i16
        .smart

        lda #7
        sta BGMODE

        ;
        ; copy mode7 data
        ;


        lda #$00
        sta $2121
        CALL(g_dma_tag_2,cgdata_dma)

        lda #$00
        sta $2115
        ldx #$0000
        stx VMADDL
        CALL(g_dma_tag_2,map_dma)

        lda #$80
        sta $2115
        ldx #$0000
        stx VMADDL
        CALL(g_dma_tag_2,char_dma)




        ;
        ; print some text
        ;


        ;
        ; config & enable display
        ;

        ldx #$400
        stx __M7_SX
        stx __M7_SY

        lda #256/2  ; set center-x
        sta M7X
        stz M7X
        lda #224/2  ; set center-y
        sta M7Y
        sty M7Y

        stz M7SEL
        lda #$07
        sta BGMODE
        lda #$1
        sta TM
        lda #$0f
        sta INIDISP

        ;
        ; enable NMI/joypad reading
        ;

        lda #$81
        sta $4200
        cli

main_loop: 


        jmp main_loop

quit:


brk_handler:


nmi:

        phb
        pha
        phx
        phy
        phd

        lda #$00
        xba

        ;
        ; calc M7B == -sin(a) * (1/sx)
        ;

        lda __M7_ANGLE
        inc a
        sta __M7_ANGLE
        tax             ; x=angle

        lda __M7_SX     ;
        sta M7A         ;
        lda __M7_SX+1   ;
        sta M7A         ; M7A=SX

        lda sincos,x    ;
        eor #$ff        ; 
        inc a           ; 
        sta M7B         ; M7B=-sin(angle)
        ldy MPYM
        sty __M7_B      ; __M7_B = -sin(angle)*SX

        ;
        ; calc M7C == sin(a) * (1/sy)
        ;

        lda __M7_SY
        sta M7A
        lda __M7_SY+1
        sta M7A         ; M7A=SY

        lda sincos,x
        sta M7B
        ldy MPYM
        sty __M7_C     ; __M7_B = sin(angle)*SY

        ;
        ; get cos index
        ; cos(a)=sin(a+64)

        lda #$00
        xba
        txa
        clc
        adc #64
        tax           ; x = cos(angle)

        ;
        ; calc M7A == cos(a) * (1/sx)
        ;

        lda __M7_SX
        sta M7A
        lda __M7_SX+1
        sta M7A
        lda sincos,x
        sta M7B
        ldy MPYM
        sty __M7_A  ; __M7_A = SX*cos(angle)


        ;
        ; calc M7D == cos(a) * (1/sy)
        ;

        lda __M7_SY
        sta M7A
        lda __M7_SY+1
        sta M7A
        lda sincos,x
        sta M7B
        ldy MPYM
        sty __M7_D

	
        ;
        ; store parameters
        ;

	lda __M7_A
	sta M7A
	lda __M7_A+1
	sta M7A
	
	lda __M7_B
	sta M7B
	lda __M7_B+1
	sta M7B
	
	lda __M7_C
	sta M7C
	lda __M7_C+1
	sta M7C
	
	lda __M7_D
	sta M7D
	lda __M7_D+1
	sta M7D

        .ifdef SWC

        jsr process_joypads
        lda joy1_held
        beq :+
                jsr __swc_return_to_bios
        :

        .endif

        lda $4210

        pld
        ply
        plx
        pla
        plb

        rti


sin_tab:

    .byte 0
    .byte 1
    .byte 2
    .byte 3
    .byte 4
    .byte 5
    .byte 6
    .byte 6
    .byte 7
    .byte 8
    .byte 9
    .byte 10
    .byte 11
    .byte 12
    .byte 13
    .byte 13
    .byte 14
    .byte 15
    .byte 16
    .byte 17
    .byte 18
    .byte 19
    .byte 19
    .byte 20
    .byte 21
    .byte 22
    .byte 23
    .byte 24
    .byte 25
    .byte 25
    .byte 26
    .byte 27
    .byte 28
    .byte 29
    .byte 30
    .byte 30
    .byte 31
    .byte 32
    .byte 33
    .byte 34
    .byte 35
    .byte 35
    .byte 36
    .byte 37
    .byte 38
    .byte 39
    .byte 39
    .byte 40
    .byte 41
    .byte 42
    .byte 43
    .byte 43
    .byte 44
    .byte 45
    .byte 46
    .byte 46
    .byte 47
    .byte 48
    .byte 49
    .byte 50
    .byte 50
    .byte 51
    .byte 52
    .byte 52
    .byte 53
    .byte 54
    .byte 55
    .byte 55
    .byte 56
    .byte 57
    .byte 58
    .byte 58
    .byte 59
    .byte 60
    .byte 60
    .byte 61
    .byte 62
    .byte 62
    .byte 63
    .byte 64
    .byte 64
    .byte 65
    .byte 66
    .byte 66
    .byte 67
    .byte 68
    .byte 68
    .byte 69
    .byte 70
    .byte 70
    .byte 71
    .byte 71
    .byte 72
    .byte 73
    .byte 73
    .byte 74
    .byte 74
    .byte 75
    .byte 76
    .byte 76
    .byte 77
    .byte 77
    .byte 78
    .byte 78
    .byte 79
    .byte 79
    .byte 80
    .byte 80
    .byte 81
    .byte 81
    .byte 82
    .byte 82
    .byte 83
    .byte 83
    .byte 84
    .byte 84
    .byte 85
    .byte 85
    .byte 86
    .byte 86
    .byte 87
    .byte 87
    .byte 87
    .byte 88
    .byte 88
    .byte 89
    .byte 89
    .byte 89
    .byte 90
    .byte 90
    .byte 90
    .byte 91
    .byte 91
    .byte 92
    .byte 92
    .byte 92
    .byte 93
    .byte 93
    .byte 93
    .byte 93
    .byte 94
    .byte 94
    .byte 94
    .byte 95
    .byte 95
    .byte 95
    .byte 95
    .byte 96
    .byte 96
    .byte 96
    .byte 96
    .byte 97
    .byte 97
    .byte 97
    .byte 97
    .byte 97
    .byte 97
    .byte 98
    .byte 98
    .byte 98
    .byte 98
    .byte 98
    .byte 98
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 99
    .byte 98
    .byte 98
    .byte 98
    .byte 98
    .byte 98
    .byte 98
    .byte 97
    .byte 97
    .byte 97
    .byte 97
    .byte 97
    .byte 97
    .byte 96
    .byte 96
    .byte 96
    .byte 96
    .byte 95
    .byte 95
    .byte 95
    .byte 95
    .byte 94
    .byte 94
    .byte 94
    .byte 93
    .byte 93
    .byte 93
    .byte 93
    .byte 92
    .byte 92
    .byte 92
    .byte 91
    .byte 91
    .byte 90
    .byte 90
    .byte 90
    .byte 89
    .byte 89
    .byte 89
    .byte 88
    .byte 88
    .byte 87
    .byte 87
    .byte 87
    .byte 86
    .byte 86
    .byte 85
    .byte 85
    .byte 84
    .byte 84
    .byte 83
    .byte 83
    .byte 82
    .byte 82
    .byte 81
    .byte 81
    .byte 80
    .byte 80
    .byte 79
    .byte 79
    .byte 78
    .byte 78
    .byte 77
    .byte 77
    .byte 76
    .byte 76
    .byte 75
    .byte 74
    .byte 74
    .byte 73
    .byte 73
    .byte 72
    .byte 71
    .byte 71
    .byte 70
    .byte 70
    .byte 69
    .byte 68
    .byte 68
    .byte 67
    .byte 66
    .byte 66
    .byte 65
    .byte 64
    .byte 64
    .byte 63
    .byte 62
    .byte 62
    .byte 61
    .byte 60
    .byte 60
    .byte 59
    .byte 58
    .byte 58
    .byte 57
    .byte 56
    .byte 55
    .byte 55
    .byte 54
    .byte 53
    .byte 52
    .byte 52
    .byte 51
    .byte 50
    .byte 50
    .byte 49
    .byte 48
    .byte 47
    .byte 46
    .byte 46
    .byte 45
    .byte 44
    .byte 43
    .byte 43
    .byte 42
    .byte 41
    .byte 40
    .byte 39
    .byte 39
    .byte 38
    .byte 37
    .byte 36
    .byte 35
    .byte 35
    .byte 34
    .byte 33
    .byte 32
    .byte 31
    .byte 30
    .byte 30
    .byte 29
    .byte 28
    .byte 27
    .byte 26
    .byte 25
    .byte 25
    .byte 24
    .byte 23
    .byte 22
    .byte 21
    .byte 20
    .byte 19
    .byte 19
    .byte 18
    .byte 17
    .byte 16
    .byte 15
    .byte 14
    .byte 13
    .byte 13
    .byte 12
    .byte 11
    .byte 10
    .byte 9
    .byte 8
    .byte 7
    .byte 6
    .byte 6
    .byte 5
    .byte 4
    .byte 3
    .byte 2
    .byte 1
    .byte 0
    .byte 0
;
; screen setup and dma transfers
;
DMA_TRANSFER_TAG(cgdata_dma,0,$22,.loword(cgdata),^cgdata,cgdata_end-cgdata)
DMA_TRANSFER_TAG(map_dma,0,$18,.loword(screen_map),^screen_map,screen_map_end-screen_map)
DMA_TRANSFER_TAG(char_dma,0,$19,.loword(screen_tiles),^screen_tiles,screen_tiles_end-screen_tiles)


;
; graphics data 
;
.SEGMENT "GRAPHICS"

screen_map:
.incbin "up.mp7"
screen_map_end:

screen_tiles:
.incbin "up.pc7"
screen_tiles_end:

cgdata:
.incbin "up.clr"
cgdata_end:

