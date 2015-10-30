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

.global fopen9
.type fopen9, %function

.global fwrite9
.type fwrite9, %function

.global fread9
.type fread9, %function

.global fclose9
.type fclose9, %function

#ifdef PLATFORM_KTR
.thumb_set fopen9, 0x0805B014
.thumb_set fwrite9, 0x0805C378
.thumb_set fread9, 0x0804D854
.thumb_set fclose9, 0x0805B100
#else
.thumb_set fopen9, 0x0805B180
.thumb_set fwrite9, 0x0805C4D0
.thumb_set fread9, 0x0804D9B0
.thumb_set fclose9, 0x0805B26C
#endif
