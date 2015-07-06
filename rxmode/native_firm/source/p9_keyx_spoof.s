// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
@ p9 keyx spoof
.thumb
.align 2

	ldr 	r2, =0x801A6D0 		//Here we'll put the keyX, this is a redirection
	mov	    r1, #5
	mov 	r0, #0x25
	bl		0x080575B4
	bl		0x0805FB48
	pop 	{r4-r6,pc}

.pool
