.include "regs.inc"
.include "types.inc"

.macro init_snes

        .smart

	sei				; disable interrupts
	clc				; switch to native mode
	xce				;
	rep	#38h			; mem/A/X/Y = 16bit
					; decimal mode off

	ldx	#1FFFh			; setup stack pointer
	txs				;
	lda	#0000h			; direct page = 0000h
	tcd				;
	sep	#20h			; 8bit A/mem
	lda	#80h			; data bank = 80h
	pha				;
	plb				;
	lda	$FFD5			; get map mode
	lsr				; 21/31 jump to bank C0
	bcs	:+			; 20/30 jump to bank 80
	jml	$800000+histart	; (for switchable speed)
:	jml	$C00000+histart	;
	
;----------------------------------------------------------------------
histart:
;----------------------------------------------------------------------
	
	lda	$FFD5			; if map_mode & 10h
	bit	#10h			; switch to hi-speed mode
	beq	:+			;
	lda	#1			;
	sta	MEMSEL		        ;
:					;

	lda	#8Fh			; enter forced blank
	sta	2100h			;

	ldx	#2101h			; regs $2101-$210C
:	stz	00h,x			; set Sprite,Character,Tile sizes to lowest, 
	inx				; and set addresses to $0000
	cpx	#210Dh			;
	bne	:-			;

:	stz	00h,x			; regs $210D-$2114
	stz	00h,x			; Set all BG scroll values to $0000
	inx				;
	cpx	#2115h			;
	bne	:-			;

;        lda #255                        ; set Y-SCROLL to 255
        stz BG1VOFS                     ; so that first char line
        stz BG1VOFS                     ; is visible.
        stz BG2VOFS                     ;
        stz BG2VOFS                     ;
        stz BG3VOFS                     ;
        stz BG3VOFS                     ;
        stz BG4VOFS                     ;
        stz BG4VOFS                     ;


	lda	#80h			; Initialize VRAM transfer mode to word-access, increment by 1
	sta	2115h			;

	stz	2116h			; VRAM address = $0000
	stz	2117h			;

	stz	211Ah			; clear Mode7 setting

	ldx	#211Bh			; regs $211B-$2120

:	stz	00h,x			; clear out the Mode7 matrix values
	stz	00h,x			;
	inx				;
	cpx	#2121h			;
	bne	:-			;

	ldx	#2123h			; regs $2123-$2133
:	stz	00h,x			; turn off windows, main screens, sub screens, color addition,
	inx				; fixed color = $00, no super-impose (external synchronization),
	cpx	#2134h			; no interlaced mode, normal resolution
	bne	:-			;

	stz	213Eh			; might not be necesary, but selects PPU master/slave mode
	stz	4200h			; disable timers, NMI,and auto-joyread
	lda	#0FFh
	sta	4201h			; programmable I/O write port, 
					; initalize to allow reading at in-port

	stz	420Bh			; turn off all general DMA channels
	stz	420Ch			; turn off all H-MA channels

	lda	4210h			; NMI status, reading resets

;----------------------------------------------------------------------
; clear VRAM
;----------------------------------------------------------------------
	lda	#80h		; set vram port to word access
	sta	2115h		;
	ldx	#1809h		; clear vram with dma
	stx	4300h		; dma mode: fixed source, WORD to $2118/9
	ldx	#0000h		;
	stx	2116h		; VRAM port address to $0000
	stx	0000h		; Set $00:0000 to $0000 (assumes scratchpad ram)
	stx	4302h		; Set source address to $xx:0000
	lda	#00h
	sta	4304h		; Set source bank to $00
	ldx	#0
	stx	4305h		; Set transfer size to 64k bytes
	lda	#01h
	sta	420Bh		; Initiate transfer

	stz	2119h		; clear the last byte of the VRAM
;----------------------------------------------------------------------
; clear palette
;----------------------------------------------------------------------
	stz	2121h
	ldx	#0100h
	
:	stz	$2122
	stz	$2122
	dex
	bne	:-
;----------------------------------------------------------------------

;------------------------------------------------------------------------------------------
; init OAM
;------------------------------------------------------------------------------------------

	stz	2102h			; sprites initialized to be off the screen, 
	stz	2103h			; palette 0, character 0
	ldx	#0080h
	lda	#0F0h

:	sta	2104h			; X = 240
	sta	2104h			; Y = 240
	stz	2104h			; character = $00
	stz	2104h			; set priority=0, no flips
	dex
	bne	:-

	ldx	#0020h
:	stz	2104h			; size bit=0, x MSB = 0
	dex
	bne	:-

;------------------------------------------------------------------------------------------
; erase WRAM
;------------------------------------------------------------------------------------------

	stz	2181h			; WRAM address = 0
	stz	2182h			;
	stz	2183h			;

	ldx	#8008h			; Set DMA mode to fixed source, BYTE to $2180
	stx	4300h 		        ; source = wram_fill_byte
	ldx	#wram_fill_byte		; transfer size = full 64k
	stx	4302h         		; 
	lda	#^wram_fill_byte	;
	sta	4304h  		        ;
	ldx	#0000h			;
	stx	4305h			;
	lda	#01h			;
	sta	420Bh			; start transfer (lower 64k)
	nop
	nop
	sta	420Bh			; transfer again (higher 64k)

	lda	#^__HDATA_RUN__&1	; copy to __HDATA_RUN__
	sta	WMADDH		;
	ldx	#.LOWORD(__HDATA_RUN__)	;
	stx	WMADDL		;

	ldx	#8000h			; dma increment source, copy to 2180
	stx	DMAP0		;
	ldx	#.LOWORD(__HDATA_LOAD__); copy from __HDATA_LOAD__
	stx	A1T0L		;
	lda	#^__HDATA_LOAD__	;
	sta	A1B0		;
	ldx	#__HDATA_SIZE__		; skip if data segment is empty
	beq	_empty_data_segment	;
	stx	DAS0L		;
	lda	#01h			;
	sta	MDMAEN		;
_empty_data_segment:			;

        rep #$38                        ;
        sep #$20                        ; 8bit accu / 16bit index
	
	jmp :+

wram_fill_byte:
.byte	$00

        :
.endmacro


.macro neg
        clc
        eor #$ff
        adc #$01
.endmacro

;
; waits for the hblank period of scanline [line]
;
; input : scanline to wait for
; output: --
;
.macro wait_scanline line
.local wait_line
wait_line:
	lda $2137
	lda $213d
	tay
	lda $213d
	and #$01
	pha
	tya
	pha
	plx
	cpx #line
	bne wait_line

	waitHBLANK
.endmacro

;
; loads [color_count]*2 bytes of data from [pal_data] to cgram at [cgram_addr]
; 
; input: [pal_data] palette data source address
;        [cgram_addr] destination address in cgram
;        [bytes] number of bytes to copy (bytes=colors*2)
; output: --
; 
.macro upload_to_cgram pal_data, cgram_addr, count
.local @color_loop
	
	ldy #$0000

	lda #cgram_addr
	sta $2121    ; set CGRAM write address
@color_loop:
	lda pal_data,y
	sta $2122
	iny
	lda pal_data,y
	sta $2122
	iny
	cpy #count
	bne @color_loop
.endmacro

.macro upload_to_vram src, dest, len
        ; set vram point
        ldx #dest
        stx VMADDL
        ldx #$0000
        :
        lda src,x
        sta VMDATAL
        inx
        lda src,x
        inx
        sta VMDATAH
        cpx #len
        bne :-
.endmacro 

.macro g_dma_transfer src_addr, dest_addr, count

	ldx   #src_addr
	stx   $4302
	lda   #^src_addr
	sta   $4304
	ldx   #count
	stx   $4305
	lda   #dest_addr
	sta   $4301
	lda   #$01
	sta   $4300
	lda   #$01
	sta   $420b	
.endmacro

.macro print_string string,dest
.local str_loop
.local done
.ifnblank dest
        ldx #dest
        stx VMADDL
.endif
        ldx #$00
str_loop:
        lda string,x
        beq done
        sta VMDATAL
        stz VMDATAH
        inx
        bra str_loop
done:
.endmacro

.macro push_argument arg
        .if (.match (.left (1,{arg}),x))
                phx
        .elseif (.match (.left (1,{arg}),y))
                phy
        .elseif (.match (.left (1,{arg}),'('))
                pei arg
        .else
                pea arg
        .endif
.endmacro

.macro process_arguments arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8
        .ifnblank arg8
                push_argument arg8
        .endif
        .ifnblank arg7
                push_argument arg7
        .endif
        .ifnblank arg6
                push_argument arg6
        .endif
        .ifnblank arg5
                push_argument arg5
        .endif
        .ifnblank arg4
                push_argument arg4
        .endif
        .ifnblank arg3
                push_argument arg3
        .endif
        .ifnblank arg2
                push_argument arg2
        .endif
        .ifnblank arg1
                push_argument arg1
        .endif
.endmacro


.macro call fptr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8

        process_arguments arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8
        jsr fptr


        rep #$38
        .a16
        tsc                               ;
        clc                               ; 
        adc # ( ( .paramcount -1 ) * 2 )  ;
        tcs                               ; remove args from stack
        sep #$20
        .a8

.endmacro


.macro PROC_PROLOGUE
        pha
        phx
        phy
        php
        phd
        tsc
        tcd
.endmacro

.macro PROC_EPILOGUE
        tdc
        tcs
        pld
        plp
        ply
        plx
        pla
        rts
.endmacro

; push all regs
;
.macro phr
        pha
        phx
        phy
.endmacro

; pull all regs
;
.macro plr
        ply
        plx
        pla
.endmacro

.define DMA_CTRL 0
.define DMA_DST_REG 1
.define DMA_SRC_OFS 2
.define DMA_SRC_BNK 3
.define DMA_LEN 5

.macro build_dma_tag control,dest,src_ofs,src_bank,len
        .byte control
        .byte dest
        .word src_ofs
        .byte src_bank
        .word len
.endmacro

.macro bg_init __bg,__id,__char1,__map1,__size1,__char2,__map2,__size2
        lda __id
        sta __bg+bg_t::id

        ldx __char1
        stx __bg+bg_t::char_1
        ldx __map1
        stx __bg+bg_t::map_1
        lda __size1
        sta __bg+bg_t::size_1

        ldx __char2
        stx __bg+bg_t::char_2
        ldx __map2
        stx __bg+bg_t::map_2
        lda __size2
        sta __bg+bg_t::size_2

.endmacro

.define ARG_1 12-1
.define ARG_2 14-1
.define ARG_3 16-1
.define ARG_4 18-1
.define ARG_5 20-1
.define ARG_6 22-1
.define ARG_7 23-1
.define ARG_8 25-1

.define BUTTON_A     #(1<<7) ;
.define BUTTON_X     #(1<<6) ;
.define BUTTON_L     #(1<<5) ;
.define BUTTON_R     #(1<<4) ; low ($4218)

.define BUTTON_B     #(1<<7) ;
.define BUTTON_Y     #(1<<6) ;
.define BUTTON_SEL   #(1<<5) ; high ($4219)
.define BUTTON_STA   #(1<<4) ;
.define BUTTON_UP    #(1<<3) ;
.define BUTTON_DOWN  #(1<<2) ;
.define BUTTON_LEFT  #(1<<1) ; 
.define BUTTON_RIGHT #(1<<0) ;


.macro _printf str, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8
        .local __string
        jmp :+
__string:
        .asciiz str
        : 
        process_arguments __string,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8
        jsr printf
.endmacro

.macro puts str
        .local __string
        jmp :+
__string:
        .asciiz str
        :
        pea __string
        jsr puts
        plx
.endmacro

.macro l_puts str
        .local __string
        jmp :+
__string:
        .asciiz str
        :
        pea __string
        jsr l_puts
        plx
.endmacro

.macro s_puts str
        .local __string
        jmp :+
__string:
        .asciiz str
        :
        pea __string
        jsr s_puts
        plx
.endmacro

.macro s_putsl str
        .local __string
        jmp :+
__string:
        .asciiz str
        :
        pea __string
        jsr s_putsl
        plx
.endmacro

.define OAM_SIZE 544
.define OAM_XPOS 0
.define OAM_YPOS 1
.define OAM_NUM  2
.define OAM_FLAG 3
.define OAM_V_FLIP 7
.define OAM_H_FLIP 6
.define OAM_PRIO   4
.define OAM_PAL    1
.define OAM_NAME   0

; OBSEL
OBSEL_8_16		= (0<<5)
OBSEL_8_32		= (1<<5)
OBSEL_8_64		= (2<<5)
OBSEL_16_32		= (3<<5)
OBSEL_16_64		= (4<<5)
OBSEL_32_64		= (5<<5)
OBSEL_16x32_32x64	= (6<<5)
OBSEL_16x32_32x32	= (7<<5)
OBSEL_NN_16K		= (3<<3)
OBSEL_NN_12K		= (2<<3)
OBSEL_NN_8K		= (1<<3)

; TM
TM_BG1 = (1<<0)
TM_BG2 = (1<<1)
TM_BG3 = (1<<2)
TM_BG4 = (1<<3)
TM_OBJ = (1<<4)

; BGMODE

BG1_8  = (0<<4)
BG1_16 = (1<<4)
BG2_8  = (0<<5)
BG2_16 = (1<<5)
BG3_8  = (0<<6)
BG3_16 = (1<<6)
BG4_8  = (0<<7)
BG4_16 = (1<<7)

MODE_0   = 0
MODE_1   = 1
MODE_2   = 2
MODE_3   = 3
MODE_4   = 4
MODE_5   = 5
MODE_6   = 6
MODE_7   = 7
MODE_EXT = 8

; NMITIMEN

AUTOJOY_ON = (1<<0)
NMI_ON     = (1<<7)


.define OBSEL_BASE(bbb)	(((bbb*2)>>14))

.macro set_vram_addr addr
        ldx #addr
        stx VMADDL
.endmacro

.macro set_oam_addr addr
        ldx #addr
        stx OAMADDL
.endmacro


.define MAP_SIZE $800
.macro DEF_SOUND _rate, _pan, _vol, _src, _srcend
        .byte _rate
        .byte _pan
        .byte _vol
        .word (_srcend-_src)/9
        .word .loword(_src)
        .byte ^_src
.endmacro

; BG12NBA
.define BG1_CHR_ADDR(addr) (addr>>12)
.define BG2_CHR_ADDR(addr) (addr>>8)
.define BG1_BG2_CHR_ADDR(bg1_addr,bg2_addr) (#bg1_addr>>12|#bg2_addr>>8)
.define BG1_CHR_SHIFT 12
.define BG2_CHR_SHIFT 8

; BG34NBA
.define BG3_CHR_ADDR(addr) (addr>>12)
.define BG4_CHR_ADDR(addr) (addr>>8)

; BG1SC,BG2SC,BG3SC,BG4SC
.define BG_MAP_ADDR(addr) #(addr>>8)
.define BG_MAP_SHIFT 8
.define BG_SIZE_32X32 0
.define BG_SIZE_64X32 1
.define BG_SIZE_32X64 2
.define BG_SIZE_64X64 3

.define SET_VRAM_ADDR(__addr) set_vram_addr __addr
.define SET_OAM_ADDR(__addr) set_oam_addr __addr

;.define DMA_TRANSFER_TAG(name,ctr,dst,src,bnk,len) name: build_dma_tag ctr,dst,src,bnk,len

.ifdef CC_PRE_PROCESS

#define AUTO_DMA_TRANSFER_TAG(src,ctr,dst) src##_dma: build_dma_tag ctr,dst,.loword(src),^src,src##_end-src
#define CALL(fptr, ...) call fptr, ## __VA_ARGS__ 
#define FCALL(fptr, ...) fcall fptr, ## __VA_ARGS__
#define PRINTF(str, ...) _printf str, ## __VA_ARGS__


; INIDISP
#define FORCE_BLANK #$80
#define FULL_BRIGHTNESS #$0F




.endif
