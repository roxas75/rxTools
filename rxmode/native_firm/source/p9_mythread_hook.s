// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
.nds
.create build/08086140.bin, 0x08086140
.arm
.align 4

	.word 0xE59FF02C	//ldr	pc, =0x0801A6E0

.pool
.close

//-------------------------------------------------------
.nds
.create build/08086174.bin, 0x08086174
.arm
.align 4

	.word 0x0801A6E0	//This is our CreateThread code address, we'll compile it with gcc

.pool
.close
