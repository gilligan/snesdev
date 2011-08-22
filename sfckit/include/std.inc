.include "regs.inc"
.include "types.inc"


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
