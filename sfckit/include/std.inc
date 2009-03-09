.macro native_init


        rep #$38
        .a8
        .i16          ; 8bit accu / 16bit index

        ldx #$1fff    
        txs           ; stack at $1fff
        phk
        plb
        lda #$0000
        tcd
        sep #$20      ; direct page at $0000
.endmacro

.macro init_snes

        .local clear_ppu
        .local clear_vram
        
        lda #$80
        sta $2100

        ldx #$33
clear_ppu:
        stz $2100,x
        dey
        bne clear_ppu

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
