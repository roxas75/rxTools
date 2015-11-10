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

#define ASM
#include <reboot.h>

line = 32

	.arm
	.global	execReboot
	.type	execReboot, %function
execReboot:
	mov	r4, #0x08000000	@ Address
	orr	r4, #0x29	@ Size: 2^0x28 = 2M, Enable
	mcr	p15, 0, r4, c6, c2, 0	@ Set MPU area 2

	ldr	r4, [r3, #20]
	add	r5, r2, r4

	ldr	r3, [r3, #16]
	add	r3, #PATCH_ADDR & 0xFFF00000
	add	r3, #PATCH_ADDR & 0x000FFFFF
loop:
	ldr	r6, [r3], #4
	str	r6, [r2], #4
	tst	r2, #line - 1
	bleq	flushInLoop
	cmp	r2, r5
	blo	loop

	tst	r2, #line - 1
	bicne	r6, r2, #line - 1
	blne	flush

	mov	r2, #0x1FFFFFF8
	mov	r3, #0
	mcr	p15, 0, r3, c7, c10, 4	@ Drain write buffer
	sub	pc, r5, r4

flushInLoop:
	sub	r6, r2, #line
flush:
	mcr	p15, 0, r6, c7, c10, 1	@ Clean Dcache
	mcr	p15, 0, r6, c7, c5, 1	@ Flush Icache
	bx	lr

	.size	execReboot, . - execReboot
