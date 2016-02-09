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

	.arm
	.text
	.global	nandHook
nandHook:
	stmib	sp, {r2-r3}
	ldr	r7, [lr, #-1]
	ldmib	r0, {r5-r6}
#ifdef PLATFORM_KTR
	ldr	r4, =0x080D86F0
#else
	ldr	r4, =0x080D8670
#endif
	cmp	r5, r4
	beq	loc_801A310
	mov	r5, r4
	cmp	r6, #0
	ldreq	r6, nandSector

loc_801A310:
	mov	r4, r0
	movs	r0, r1, lsl #23
	beq	loc_801A33C
#ifdef PLATFORM_KTR
	blx	0x08062A28
#else
	blx	0x08062A28
#endif

loc_801A33C:
	mov	r0, r1, lsr #9
	add	r0, r0, r6
	str	r0, [sp]
	ldr	r1, [r5]
	ldr	r1, [r1, #24]
	blx	r1
	mov	r3, r0
	ldr	r0, [r5, #32]
	cmp	r5, #0
	moveq	r1, r5
	addne	r1, r5, #8
	add	r2, r4, #12
	blx	r7
	stmib	r4, {r5-r6}
	ldmfd	sp!, {r1-r7, pc}

	.size nandHook, . - nandHook

	.global	nandSector
	.type	nandSector, %object
nandSector:
	.word	0
	.size	nandSector, . - nandSector
