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

//FIRM Reboot patch, which is necessary for the hi-ram using games to boot.
//TODO : Do not exclude other FIRMS to be opened. Not important until we patch them for emunand

//----------------- GLOBALS -----------------------
#ifdef PLATFORM_KTR
.equ readpxi,       0x0805501C
#else
.equ readpxi,       0x08055178
#endif
.equ filehandle,    0x2000E000
.equ byteswritten,  0x2000E100
.equ buffer,        0x24000000

//----------------- PROCESS9 ----------------------
.section .patch.p9.reboot.entry, "a"
.arm
.align 2
	GetFileName:
		ldr r2, =filename
		mov r1, #0x30
		add r0, sp, #0x3A8-0x70
		blx 0x080282CE

    ClearWorkspace:
        ldr r0, =filehandle
        ldr r1, =0x200
        mov r2, #0
        add r1, r1, r0
        CHL1:
            str r2, [r0]
            add r0, #4
            cmp r0, r1
            blt CHL1

    OpenFirm:
        ldr r0, =filehandle
		add r1, sp, #0x3A8-0x70
        mov r2, #1
 	blx fopen9

    ReadFirm:
        ldr r0, =filehandle
        ldr r1, =byteswritten
        ldr r2, =buffer
        ldr r3, =0x200000
        blx fread9

    CheckFirm:
        ldr r0, =buffer
        ldr r0, [r0]
        ldr r1, =0x4D524946
        cmp r0, r1
        bne InfiniteLoop

    doPxi:
        ldr r4, =0x44846
        blx readpxi
        cmp r0, r4
        bne doPxi

    KernelSetState:
        mov r2, #0
        mov r3, r2
        mov r1, r2
        mov r0, r2
        svc 0x7C

    GoToReboot:
#ifdef PLATFORM_KTR
	ldr r0, =0x0817F4FC
#else
        ldr r0, =0x080FF4FC
#endif
        svc 0x7B

    InfiniteLoop:
        b InfiniteLoop

.pool
filename:
	//.string16 "nand:/rxtools/%08x%08x.bin"
	.string16 "sdmc:/rxtools/data/%08x%08x.bin"
