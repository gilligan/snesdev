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
        id   .byte  ; BG id number (1-4)
        char .word  ; BG char address
        map  .word  ; BG map address
        size .byte  ; BG size
        scx  .word  ; BG scroll-x
        scy  .word  ; BG scroll-y
.endstruct

.struct display_t

        vmode   .byte  ; current video mode
        bg1 .tag bg_t
        bg2 .tag bg_t
        bg3 .tag bg_t
        bg4 .tag bg_t

.endstruct
