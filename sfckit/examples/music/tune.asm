;************************************************
; snesmod soundbank data                        *
; total size:      34630 bytes                  *
;************************************************

	.global __SOUNDBANK__
	.segment "SOUNDBANK" ; need dedicated bank(s)

__SOUNDBANK__:
	.incbin "tune.bank"
