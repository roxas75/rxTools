// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
@ p9 signature spoof
.thumb
	mov		r0, #0
	add		sp, #0x138
	pop 	{r4-r6,pc}
	.hword	0		@padding output was in armips build
.pool
