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

@ p9 keyx spoof
.section .patch.p9.keyx, "a"
.thumb
.align 2

	ldr 	r2, =keyx
	mov	    r1, #5
	mov 	r0, #0x25
	bl		0x080575B4
	bl		0x0805FB48
	pop 	{r4-r6,pc}

.pool
