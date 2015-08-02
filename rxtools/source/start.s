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
