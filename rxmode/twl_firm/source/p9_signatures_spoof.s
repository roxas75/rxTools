// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
.nds
.create build/080238C0.bin, 0x080238C0
.thumb

	mov		r0, #0
	add		sp, 0x138
	pop 	{r4-r6,pc}
.pool
.close
