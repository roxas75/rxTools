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
.global waitcycles
.type   waitcycles STT_FUNC

@ioDelay ( uint32_t us )
ioDelay:
waitcycles:
	PUSH    {R0-R2,LR}
	STR     R0, [SP,#4]
	waitcycles_loop:
		LDR     R3, [SP,#4]
		SUBS    R2, R3, #1
		STR     R2, [SP,#4]
		CMP     R3, #0
		BNE     waitcycles_loop
	POP     {R0-R2,PC}
