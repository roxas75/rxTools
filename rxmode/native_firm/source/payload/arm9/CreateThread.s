@ Copyright (C) 2015 The PASTA Team
@ Originally written by Roxas75
@
@ This program is free software; you can redistribute it and/or
@ modify it under the terms of the GNU General Public License
@ version 2 as published by the Free Software Foundation
@
@ This program is distributed in the hope that it will be useful,
@ but WITHOUT ANY WARRANTY; without even the implied warranty of
@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@ GNU General Public License for more details.
@
@ You should have received a copy of the GNU General Public License
@ along with this program; if not, write to the Free Software
@ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

.arm
.global KEYXSPACE
KEYXSPACE:    						//needed to reserve the space where we'll write the keyX. Optimizing every single byte!
	.word 0xDEADBEEF
	.string8 "InsertKeyXHere!"
//_______________________________________________________________________________________

StackTop = 0x0801AE00 				//That's arbitrary
StackSize = 0x200					//This too

.global CreateMyThread
.type CreateMyThread STT_FUNC

CreateMyThread:
	push {r1-r12, lr}

	ldr r3, =0x08000c00	@ top of the stack

	mov r0, r3
	mov r1, #0xA00
	mov r2, #0x0

	thread_stack_loop:
		str r2, [r0], #0x4
		subs r1, r1, #4
		bgt thread_stack_loop

	mov r0, #0x3F @ thread priority
	ldr r1, =myThread @ thread_addr
	mov r2, #0x0 @ arg
	mov r4, #0xFFFFFFFE
	svc 0x8

	pop {r1-r12, lr}
	ldr r0, =0x80CAFA8
	ldr pc, =0x8086144
