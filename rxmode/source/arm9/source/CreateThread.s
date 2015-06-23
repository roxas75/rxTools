// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //

.arm
.global myThread
KEYXSPACE:    						//needed to reserve the space where we'll write the keyX. Optimizing every single byte!
	.word 0xDEADBEEF
	.string8 "InsertKeyXHere!"
//_______________________________________________________________________________________

StackTop = 0x0801AE00 				//That's arbitrary
StackSize = 0x200					//This too

.global CreateMyThread
.type CreateMyThread STT_FUNC

CreateMyThread:
	push {r0-r12 , lr}

	ldr r0, =0x08000c00
	mov r1, #0xA00
	mov r2, #0x0
	thread_stack_loop:
		str r2, [r0], #0x4
		subs r1, r1, #4
		bgt thread_stack_loop
	mov r0, #0x3F @ thread priority
	ldr r1, =myThread @ thread_addr
	mov r2, #0x0 @ arg
	ldr r3, =0x08000c00 @ StackTop
	ldr r4, =0xFFFFFFFE
	svc 0x8

	pop {r0-r12 , lr}
	ldr r0, =0x80CAFA8
	ldr pc, =0x8086144