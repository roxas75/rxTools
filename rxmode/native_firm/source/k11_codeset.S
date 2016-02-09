/*
 * Copyright (C) 2015 The PASTA Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

	.section .patch.k11.codeset.addr, "a"
	.word	hook

	.section .patch.k11.codeset.hook, "a"
hook:
	push	{r0-r5}
	ldr	r0, [r1, #4]
	cmp	r0, #0
	bne	end
	ldr	r0, [r1]
	ldr	r3, word_menu
	cmp	r0, r3
	bne	end
	ldr	r0, [r1, #40]
	add	r0, r2, r0, lsl #12
	add	r2, r2, #originalBtm - original
	adr	r1, original
failLoop:
	cmp	r0, r2
	bcc	end
	adr	r3, originalBtm
successLoop:
	ldr	r4, [r0, #-4]!
	ldr	r5, [r3, #-4]!
	cmp	r4, r5
	bne	failLoop
	cmp	r3, r1
	bhi	successLoop
	ldr	r1, patch
	ldr	r2, patch + 4
	stm	r0, {r1-r2}
end:
	pop	{r0-r5}
	@ Call the original function
#ifdef PLATFORM_KTR
	b	0xFFF036F4
#else
	b	0xFFF036E8
#endif

word_menu:
	.ascii	"menu"

original:
	cmp	r5, #0
	mov	r1, #1
	lsl	r0, r1, r0
	beq	. + 20
originalBtm:

patch:
	mov	r0, #1
	pop	{r4, r5, r6, pc}
