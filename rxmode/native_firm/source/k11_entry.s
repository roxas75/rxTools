/*
 * Copyright (C) 2016 The PASTA Team
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

@ This patch fixes 3D adjustment on KTR

	.section .patch.k11.entry, "a"

	ldr r1, =0x10202000
	str r0, [r1, #0x00C]
	str r0, [r1, #0x014]
	str r0, [r1, #0x244]
	str r0, [r1, #0xA44]
	b	pooled
	.pool
pooled:
