// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
.nds
.create build/0805E010.bin, 0x0805E010
.thumb
	.word 0xEF260000 // SVC 0x260000
.pool
.close
