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

	.section ".text.vect"
	.align	2
	.arm

	@ Vector Table
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.
	b	.

perm:
	.word	0x33333333

	ldr	r0, perm
	mcr	p15, 0, r0, c5, c0, 2

	msr	CPSR_c, #0xD3 @ Disable IRQ and FIQ and enter supervisor mode
