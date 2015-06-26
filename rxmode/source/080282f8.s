// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
@ p9 keyx spoof
.equ load_offset, 0x080282F8
.thumb
.align 2

	ldr 	r2, =0x801A6D0 		//Here we'll put the keyX, this is a redirection
	mov	    r1, #5
	mov 	r0, #0x25
	.hword 0xF02F, 0xF959	@bl		0x080575B4-load_offset
	.hword 0xF037, 0xFC21	@bl		0x0805FB48-load_offset
	pop 	{r4-r6,pc}

.pool
