.include "std.inc"

        .a8
        .i16
        .smart

.SEGMENT "CODE"



.global g_dma_0
.global g_dma_tag_0
.global g_dma_tag_1
.global g_dma_tag_2
.global g_dma_tag_3
.global g_dma_tag_4
.global g_dma_tag_5
.global g_dma_tag_6
.global g_dma_tag_7


.define tag ARG_1

; h_dma_tag_0( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 0
;
.proc h_dma_tag_0

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4300   ; set dma mode
        iny

        lda (tag),y
        sta $4301   ; set dest register
        iny

        lda (tag),y
        sta $4302   ; set source addr (low)
        iny
        lda (tag),y
        sta $4303   ; set source addr (high)
        iny
        lda (tag),y
        sta $4304   ; set source bank
        iny

        lda (tag),y
        sta $4305   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4306   ; set transfer length (high)

        lda #1
        sta $420c   ; start transfer

        PROC_EPILOGUE

.endproc

; g_dma_tag_0( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 0
;
.proc g_dma_tag_0

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4300   ; set dma mode
        iny

        lda (tag),y
        sta $4301   ; set dest register
        iny

        lda (tag),y
        sta $4302   ; set source addr (low)
        iny
        lda (tag),y
        sta $4303   ; set source addr (high)
        iny
        lda (tag),y
        sta $4304   ; set source bank
        iny

        lda (tag),y
        sta $4305   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4306   ; set transfer length (high)

        lda #1
        sta $420b   ; start transfer

        PROC_EPILOGUE

.endproc

; g_dma_tag_1( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 1
;
.proc g_dma_tag_1

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4310   ; set dma mode
        iny

        lda (tag),y
        sta $4311   ; set dest register
        iny

        lda (tag),y
        sta $4312   ; set source addr (low)
        iny
        lda (tag),y
        sta $4313   ; set source addr (high)
        iny
        lda (tag),y
        sta $4314   ; set source bank
        iny

        lda (tag),y
        sta $4315   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4316   ; set transfer length (high)

        lda #1<<1
        sta $420b   ; start transfer

        PROC_EPILOGUE

.endproc


; g_dma_tag_2( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 2
;
.proc g_dma_tag_2

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4320   ; set dma mode
        iny

        lda (tag),y
        sta $4321   ; set dest register
        iny

        lda (tag),y
        sta $4322   ; set source addr (low)
        iny
        lda (tag),y
        sta $4323   ; set source addr (high)
        iny
        lda (tag),y
        sta $4324   ; set source bank
        iny

        lda (tag),y
        sta $4325   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4326   ; set transfer length (high)

        lda #$1<<2
        sta $420b   ; start transfer

        PROC_EPILOGUE

.endproc

; g_dma_tag_3( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 3
;
.proc g_dma_tag_3

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4330   ; set dma mode
        iny

        lda (tag),y
        sta $4331   ; set dest register
        iny

        lda (tag),y
        sta $4332   ; set source addr (low)
        iny
        lda (tag),y
        sta $4333   ; set source addr (high)
        iny
        lda (tag),y
        sta $4334   ; set source bank
        iny

        lda (tag),y
        sta $4335   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4336   ; set transfer length (high)

        lda #1<<3
        sta $420b   ; start transfer

        PROC_EPILOGUE

.endproc

; g_dma_tag_4( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 4
;
.proc g_dma_tag_4

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4340   ; set dma mode
        iny

        lda (tag),y
        sta $4341   ; set dest register
        iny

        lda (tag),y
        sta $4342   ; set source addr (low)
        iny
        lda (tag),y
        sta $4343   ; set source addr (high)
        iny
        lda (tag),y
        sta $4344   ; set source bank
        iny

        lda (tag),y
        sta $4345   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4346   ; set transfer length (high)

        lda #1<<4
        sta $420b   ; start transfer

        PROC_EPILOGUE
.endproc

; g_dma_tag_5( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 5
;
.proc g_dma_tag_5

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4350   ; set dma mode
        iny

        lda (tag),y
        sta $4351   ; set dest register
        iny

        lda (tag),y
        sta $4352   ; set source addr (low)
        iny
        lda (tag),y
        sta $4353   ; set source addr (high)
        iny
        lda (tag),y
        sta $4354   ; set source bank
        iny

        lda (tag),y
        sta $4355   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4356   ; set transfer length (high)

        lda #1<<5
        sta $420b   ; start transfer

        PROC_EPILOGUE
.endproc

; g_dma_tag_6( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 6
;
.proc g_dma_tag_6

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4360   ; set dma mode
        iny

        lda (tag),y
        sta $4361   ; set dest register
        iny

        lda (tag),y
        sta $4362   ; set source addr (low)
        iny
        lda (tag),y
        sta $4363   ; set source addr (high)
        iny
        lda (tag),y
        sta $4364   ; set source bank
        iny

        lda (tag),y
        sta $4365   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4366   ; set transfer length (high)

        lda #1<<6
        sta $420b   ; start transfer

        PROC_EPILOGUE
.endproc

; g_dma_tag_7( dma_tag* tag)
;
; transfers data according to info pointed to by [tag] using
; DMA channel 7
;
.proc g_dma_tag_7

        PROC_PROLOGUE

        ldy #$00
        lda (tag),y  
        sta $4370   ; set dma mode
        iny

        lda (tag),y
        sta $4371   ; set dest register
        iny

        lda (tag),y
        sta $4372   ; set source addr (low)
        iny
        lda (tag),y
        sta $4373   ; set source addr (high)
        iny
        lda (tag),y
        sta $4374   ; set source bank
        iny

        lda (tag),y
        sta $4375   ; set transfer length (low)
        iny
        lda (tag),y
        sta $4376   ; set transfer length (high)

        lda #1<<7
        sta $420b   ; start transfer

        PROC_EPILOGUE
.endproc


; g_dma_0( u16 bank, u16 src, u16 dest, u16 len)
;
; transfers [len] bytes of data from [src]
; to [dst] using channel [ch]
;

.proc g_dma_0

.define src_bank ARG_1
.define src_ofs  ARG_2
.define dst      ARG_3
.define len      ARG_4

        PROC_PROLOGUE

        ldx src_ofs
        stx $4302          ; set source address
        ldx src_bank
        stx $4304          ; set source bank
        ldx len
        stx $4305          ; set transfer length
        lda dst
        sta $4301          ; set destination register

        lda #$01
        sta $4300
        lda #$01
        sta $420b

        PROC_EPILOGUE
.endproc
