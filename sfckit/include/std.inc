.include "types.inc"

;
; initialize stack and direct page
; set 8bit accu / 16bit index
;
; input : --
; output: --
; 
.macro native_init

        sei
        clc
        xce           ; native mode

        rep #$38
        .a16
        .i16          ; 16bit accu / 16bit index

        ldx #$1fff    
        txs           ; stack at $1fff

        phk
        plb
        lda #$0000
        tcd           ; direct page at $0000

        sep #$20      ; 8bit accu / 16bit index
        .a8
        .i16

.endmacro

;
; clears ppu registers and vram
; 
; input : --
; output: --
;
.macro init_snes

        .local clear_ppu
        .local clear_vram
        
        lda #$80
        sta INIDISP

        ldx #$33
clear_ppu:
        stz $2100,x
        dey
        bne clear_ppu

        ; scroll BG's down 1 pixel so
        ; that first scanline is not hidden
        lda #$ff
        sta BG1VOFS
        stz BG1VOFS
        sta BG2VOFS
        stz BG2VOFS
        sta BG3VOFS
        stz BG3VOFS
        sta BG4VOFS
        stz BG4VOFS

        ldx #$0000
        stx VMADDL
        ldx #$8000
clear_vram:
        sty $2118
        dex
        bne clear_vram
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
;        [color_count] number of colors to copy (bytes=colors*2)
; output: --
; 
.macro load_pal pal_data, cgram_addr, color_count
	
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
	cpy #color_count*2
	bne @color_loop
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
               ;
        pha    ; dummy byte - jsr puts 2 bytes on stack/ jsl 3 bytes
               ;
        jsr fptr
.endmacro

.macro fcall fptr, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8
        process_arguments arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8
        jsl fptr
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
        rtl  ; for now stick to always assuming jsl/rtl
.endmacro

.macro build_dma_tag control,dest,src_ofs,src_bank,len
        .byte control
        .byte dest
        .word src_ofs
        .byte src_bank
        .word len
.endmacro

.macro bg_init __bg,__id,__char,__map,__size,__scx,__scy
        lda __id
        sta __bg+bg_t::id
        ldx __char
        stx __bg+bg_t::char
        ldx __map
        stx __bg+bg_t::map
        lda __size
        sta __bg+bg_t::size
        ldx __scx
        stx __bg+bg_t::scx
        ldx __scy
        stx __bg+bg_t::scy
.endmacro

.define ARG_1 12
.define ARG_2 14
.define ARG_3 16
.define ARG_4 18
.define ARG_5 20
.define ARG_6 22
.define ARG_7 23
.define ARG_8 25

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

.define MAP_SIZE $800


.ifdef CC_PRE_PROCESS

#define DMA_TRANSFER_TAG(name,ctr,dst,src,bnk,len) name: build_dma_tag ctr,dst,src,bnk,len
#define CALL(fptr, ...) call fptr, ## __VA_ARGS__ 
#define FCALL(fptr, ...) fcall fptr, ## __VA_ARGS__

; INIDISP
#define FORCE_BLANK #$80
#define FULL_BRIGHTNESS #$0F

; OBSEL
#define OBJ_SIZE_8X8_16X16   #$00
#define OBJ_SIZE_8X8_32X32   #$01
#define OBJ_SIZE_8x8_64X64   #$02
#define OBJ_SIZE_16X16_32X32 #$03
#define OBJ_SIZE_16X16_64X64 #$04
#define OBJ_SIZE_32X32_64X64 #$05

#define INIT_BG(bg,id,char,map,size,scx,scy) bg_init bg,id,char,map,size,scx,scy


; BG1SC,BG2SC,BG3SC,BG4SC
#define BG_MAP_ADDR(addr) #(addr>>8)
#define BG_SIZE_32X32 0
#define BG_SIZE_64X32 1
#define BG_SIZE_32X64 2
#define BG_SIZE_64X64 3

; BG12NBA
#define BG1_CHR_ADDR(addr) #(addr>>12)
#define BG2_CHR_ADDR(addr) #(addr>>8)

; BG34NBA
#define BG3_CHR_ADDR(addr) #(addr>>12)
#define BG4_CHR_ADDR(addr) #(addr>>8)



.endif
