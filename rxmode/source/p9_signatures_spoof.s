// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
.nds
.create build/0805D63C.bin, 0x0805D63C
.thumb

	mov		r0, #0
	add		sp, 0x138
	pop 	{r4-r6,pc}
.pool
.close

//------------------------------------------------

.nds
.create build/080632B8.bin, 0x080632B8
.thumb
.align 2
	mov		r0, #0
	b 		0x80631AA
.pool
.close
