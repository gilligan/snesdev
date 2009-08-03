.global dma_tag_t
.global fifo_t


.struct fifo_t
        buffer .word
        size   .byte
        in     .byte
        out    .byte
.endstruct

.struct dma_tag_t
        control  .byte
        dst_reg  .byte
        src_ofs  .word
        src_bank .byte
        len      .word
.endstruct

.struct bg_t
        id   .byte  ; BG id number : 0=bg1/bg2 , 1=bg2/b3

        char_1 .byte  ; BG char address
        map_1  .byte  ; BG map address
        size_1 .byte  ; BG size

        char_2 .byte  ; BG char address
        map_2  .byte  ; BG map address
        size_2 .byte  ; BG size
.endstruct

.struct display_t

        vmode   .byte  ; current video mode
        bg1 .tag bg_t
        bg2 .tag bg_t
        bg3 .tag bg_t
        bg4 .tag bg_t

.endstruct
