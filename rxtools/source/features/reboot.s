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

line = 32

	.arm
	.global	execReboot
	.type	execReboot, %function
execReboot:
	mov	r4, #0x08000000	@ Address
	orr	r4, #0x29	@ Size: 2^0x28 = 2M, Enable
	mcr	p15, 0, r4, c6, c2, 0	@ Set MPU area 2

	add	r4, r1, r3
loop:
	ldr	r5, [r2], #4
	str	r5, [r1], #4
	tst	r1, #line - 1
	bleq	flushInLoop
	cmp	r1, r4
	blo	loop

	tst	r1, #line - 1
	bicne	r5, r1, #line - 1
	blne	flush

	mov	r1, #0x1FFFFFF8
	sub	pc, r4, r3

flushInLoop:
	sub	r5, r1, #line
flush:
	mcr	p15, 0, r5, c7, c10, 1	@ Clean Dcache
	mcr	p15, 0, r5, c7, c5, 1	@ Flush Icache
	bx	lr

	.size	execReboot, . - execReboot
