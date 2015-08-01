@ Copyright (C) 2015 The PASTA Team
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

.macro GEN_HANDLER name, type
	.global	\name
	.type	\name, %function

\name:
	str	lr, [sp, #-8]

	bl	storeRegs

	ldr	r1, =\type
	b	mainHandler

	.size	\name, . - \name
.endm

	.text
	.align	2

	GEN_HANDLER handleFiq, fiqStr
	GEN_HANDLER handleInstr instrStr
	GEN_HANDLER handleData, dataStr
	GEN_HANDLER handlePrefetch, prefetchStr

storeRegs:	@ Except PC
	sub	sp, sp, #16
	push	{ r0-r12 }

	mrs	r0, cpsr
	mov	r1, sp

	mrs	r2, spsr
	str	r2, [sp, #64]

	tst	r2, #0xF
	orreq	r2, r2, #0xF
	bic	r2, r2, #0x10
	msr	cpsr_c, r2
	str	lr, [r1, #52]
	str	sp, [r1, #56]
	msr	cpsr_c, r0

	mov	r0, sp

	bx	lr

	.section .rodata
fiqStr:
	.asciz	"FIQ"

instrStr:
	.asciz	"Undefined Instruction"

dataStr:
	.asciz	"Data Abort"

prefetchStr:
	.asciz	"Prefetch Abort"
