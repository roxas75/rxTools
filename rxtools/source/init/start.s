@ Copyright (C) 2016 The PASTA Team
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

	.section .text.start
	.arm

	msr	CPSR_c, #0xD3 @ Disable IRQ and FIQ and enter supervisor mode

	mov	sp, #0x28000000

	mov	r0, #0	@ disable
	mcr	p15, 0, r0, c6, c0, 0	@ MPU area 0
	mcr	p15, 0, r0, c6, c1, 0	@ MPU area 1

	mov	r0, #0x08000000	@ address 0x08000000
	orr	r0, #0x29	@ size: 2M, enable
	mcr	p15, 0, r0, c6, c2, 0	@ MPU area 2

	mov	r0, #0x10000000	@ address: 0x1000000
	orr	r0, #0x21	@ size: 128K, enable
	mcr	p15, 0, r0, c6, c3, 0	@ MPU area 3

	mov	r0, #0x18000000	@ address: 0x18000000
	orr	r0, #0x2D	@ size: 8M, enable
	mcr	p15, 0, r0, c6, c4, 0	@ MPU area 4

	ldr	r0, =0x01FF801D	@ address: 0x01FF8000, size: 32K, enable
	mcr	p15, 0, r0, c6, c5, 0	@ MPU area 5

	mov	r0, #0x20000000	@ address: 0x20000000
	orr	r0, #0x35	@ size: 128M, enable
	mcr	p15, 0, r0, c6, c6, 0	@ MPU area 6

	ldr	r0, =0x10100025	@ address: 10100000, size: 512K, enable
	mcr	p15, 0, r0, c6, c7, 0	@ MPU area 7

	ldr	r0, =0x33333300	@ permit reading and writing for all areas
	mcr	p15, 0, r0, c5, c0, 2	@ write data permission bits

	mov	r0, #0x00000600	@ permit reading for area 2
	orr	r0, #0x06000000	@ permit reading for area 6
	mcr	p15, 0, r0, c5, c0, 3	@ write instruction permission bits

	mov	r0, #0x54	@ area 2, 4 and 6
	mcr	p15, 0, r0, c3, c0, 0	@ write data bufferable bits
	mcr	p15, 0, r0, c2, c0, 0	@ write data cacheable bits

	mov	r0, #0x44	@ area 2 and 6
	mcr	p15, 0, r12, c2, c0, 1	@ write instruction cacheable bits

	mov	r0, #0x43000	@ enable ITCM, use alternative vector
	orr	r0, #0x00005	@ enable instruction and data cache, enable MPU
	mcr	p15, 0, r0, c1, c0, 0	@ write control register

	b	initStart
