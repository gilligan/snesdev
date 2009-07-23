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
        char .word  ; BG1 char address
        map  .word  ; BG1 map address
        size .byte  ; BG1 size
        scx  .word  ; BG1 scroll-x
        scy  .word  ; BG1 scroll-y
.endstruct

.struct display_t

        vmode   .byte  ; current video mode
        bg1 .tag bg_t
        bg2 .tag bg_t
        bg3 .tag bg_t
        bg4 .tag bg_t

.endstruct
