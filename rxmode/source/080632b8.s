// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
@ p9 signature spoof
.equ load_offset, 0x080632B8
.thumb
.align 1
	mov		r0, #0
	.hword	0xE776	@b		0x80631AA-load_offset
.pool
