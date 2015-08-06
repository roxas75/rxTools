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

@ k9 fix flags
.section .patch.k9.flags, "a"

    .word 0x360003
    .word 0x10100000
    .word 0x1000001
    .word 0x360003
    .word 0x20000035
    .word 0x1010101
    .word 0x200603
    .word 0x8000000
    .word 0x1010101
    .word 0x1C0603
    .word 0x8020000
