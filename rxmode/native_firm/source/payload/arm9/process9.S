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

.global p9Open
.type p9Open, %function

.global p9Write
.type p9Write, %function

.global p9Read
.type p9Read, %function

.global p9Close
.type p9Close, %function

.global p9RecvPxi
.type p9RecvPxi, %function

.global swprintf
.type swprintf, %function

#ifdef PLATFORM_KTR
.thumb_set p9Open, 0x0805B180
.thumb_set p9Write, 0x0805C4D0
.thumb_set p9Read, 0x0804D9B0
.thumb_set p9Close, 0x0805B26C
.thumb_set p9RecvPxi, 0x08055178
#else
.thumb_set p9Open, 0x0805B180
.thumb_set p9Write, 0x0805C4D0
.thumb_set p9Read, 0x0804D9B0
.thumb_set p9Close, 0x0805B26C
.thumb_set p9RecvPxi, 0x08055178
#endif
.thumb_set swprintf, 0x080282CE
