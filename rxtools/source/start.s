@---------------------------------------------------------------------------------
	.section ".init"
@---------------------------------------------------------------------------------
	.global _start
	.extern main
	.align	4
	.arm

_vectors:
	ldr pc, =InfiniteLoop
	.pool
	ldr pc, =InfiniteLoop
	.pool
	ldr pc, =InfiniteLoop
	.pool
	ldr pc, =InfiniteLoop
	.pool
	ldr pc, =InfiniteLoop
	.pool
	ldr pc, =InfiniteLoop
	.pool
@---------------------------------------------------------------------------------
_start:
@---------------------------------------------------------------------------------
	mrc p15, 0, r0, c5, c0, 2
	ldr r0, =0x33333333
	mcr p15, 0, r0, c5, c0, 2
	synchronize_kernels:
		ldr r1, =0x10000
		waitLoop9:
			sub r1, #1
			cmp r1, #0
			bgt waitLoop9
		ldr r1, =0x10000
		waitLoop92:
			sub r1, #1
			cmp r1, #0
			bgt waitLoop92

	jump_to_code:
		ldr	r3, =main
		blx r3

InfiniteLoop:
	b InfiniteLoop

.pool
	