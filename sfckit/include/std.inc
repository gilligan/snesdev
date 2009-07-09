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

.macro move_block_positive src_bank,src_ofs,dest_bank,dest_ofs,len

        phb
        rep #$30
        .a16
        .i16
        lda len-1
        ldx src_ofs
        ldy dest_ofs
        mvn src_bank, dest_bank
        sep #$20
        .a8
        .i16
        plb

.endmacro

.macro move_block_negative src_bank,src_ofs,dest_bank,dest_ofs,len

        phb
        rep #$30
        .a16
        .i16
        lda len-1
        ldx src_ofs
        ldy dest_ofs
        mvn src_bank, dest_bank
        sep #$20
        .a8
        .i16
        plb

.endmacro

.macro gDMA src,dest,len
        nop
        nop
.endmacro

.macro gDMA_RAM
        nop
        nop
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

.ifdef CC_PRE_PROCESS

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
