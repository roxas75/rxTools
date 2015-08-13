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

#include <memory.h>

	.section .rodata

reg = 0x1EF00000

	.global hookSwi
	.type	hookSwi, %object

	.align	2
hookSwi:
	push	{ r0, r1 }

	mov	r1, #ARM11_VRAM_PADDR
	add	r0, r1, #reg - ARM11_VRAM_PADDR

	@ Set VRAM
	str	r1, [r0, #0x468]
	str	r1, [r0, #0x46C]
	str	r1, [r0, #0x568]
	str	r1, [r0, #0x56C]

	@ Set LCD width and height
	ldr	r1, lcdWH
	str	r1, [r0, #0x45C]
	str	r1, [r0, #0x55C]

	@ Set LCD color format and turn off 3D
	ldr	r1, [r0, #0x470]
	bic	r1, r1, #0x27
	orr	r1, r1, #1
	str	r1, [r0, #0x470]

	ldr	r1, [r0, #0x570]
	bic	r1, r1, #0x7
	orr	r1, r1, #1
	str	r1, [r0, #0x570]

	@ Commit changes
	mov	r1, #1
	str	r1, [r0, #0x478]
	str	r1, [r0, #0x578]

	pop	{ r0, r1 }
	ldr	pc, hookSwi - 4

lcdWH:
	.word	0x14000F0	@ 320x240

	.size	hookSwi, . - hookSwi

	.global hookSwiBtm
hookSwiBtm:
