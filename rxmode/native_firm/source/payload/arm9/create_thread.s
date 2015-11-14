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
	.text
	.global	createThread
	.type	createThread, %function
createThread:
	mov	r0, #0x3F @ Priority
	ldr	r1, =thread @ Address
	mov	r2, #0 @ Argument Address
	mov	r3, #0x08000000	@ The top of the stack
	orr	r3, #0xC00
	mov	r4, #0xFFFFFFFE	@ Affinity Mask
	svc	8	@ Create thread

	ldr	r0, [lr, #48]
	bx	lr
