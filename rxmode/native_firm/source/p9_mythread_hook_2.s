// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
@ p9 mythread hook
.arm
.align 2

	.word 0x0801A6E0	//This is our CreateThread code address, we'll compile it with gcc

.pool
