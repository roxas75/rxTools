// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
@ p9 mythread hook
@.equ load_offset, 0x08086140
.arm
.align 2

	.word 0xE59FF02C	//ldr	pc, =0x0801A6E0

.pool
