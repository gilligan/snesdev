
.macro init_snes
			stz     $210D   ; BG1HOFS
                        stz     $210D   ;
                        stz     $210E   ; BG1VOFS
                        stz     $210E   ;
                        stz     $210F   ; BG2HOFS
                        stz     $210F   ;
                        stz     $2110   ; BG2VOFS
                        stz     $2110   ;
                        stz     $2111   ; BG3HOFS
                        stz     $2111   ;
                        stz     $2112   ; BG3VOFS
                        stz     $2112   ;
                        stz     $2113   ; BG4HOFS
                        stz     $2113   ;
                        stz     $2114   ; BG4VOFS
                        stz     $2114   ;
                        stz     $211A   ; M7SEL
                        stz     $211B   ; M7A
                        stz     $211B   
                        stz     $211C   ; M7B
                        stz     $211C   
                        stz     $211D   ; M7C
                        stz     $211D   
                        stz     $211E   ; M7D
                        stz     $211E   
                        stz     $211F   ; M7X
                        stz     $211F   
                        stz     $2120   ; M7Y
                        stz     $2120
                        stz     $2123   ; W12SEL
                        stz     $2124   ; W34SEL
                        stz     $2125   ; WOBJSEL
                        stz     $2126   ; WH0
                        stz     $2127   ; WH1
                        stz     $2128   ; WH2
                        stz     $2129   ; WH3
                        stz     $212A   ; WBGLOG
                        stz     $212B   ; WOBJLOG
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
