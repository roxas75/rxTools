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

//----------------- REBOOT FUNCTION ----------------
.equ buffer,        0x24000000
.section .patch.p9.reboot.body, "a"
.arm
.align 2

    memoryPermission:
        STMFD   SP!, {R4-R11,LR}
        SUB     SP, SP, #0x3C
        MRC     p15, 0, R0,c2,c0, 0
        MRC     p15, 0, R12,c2,c0, 1
        MRC     p15, 0, R1,c3,c0, 0
        MRC     p15, 0, R2,c5,c0, 2
        MRC     p15, 0, R3,c5,c0, 3
        LDR     R4, =0x18000035
        BIC     R2, R2, #0xF0000
        BIC     R3, R3, #0xF0000
        ORR     R0, R0, #0x10
        ORR     R2, R2, #0x30000
        ORR     R3, R3, #0x30000
        ORR     R12, R12, #0x10
        ORR     R1, R1, #0x10
        MCR     p15, 0, R0,c2,c0, 0
        MCR     p15, 0, R12,c2,c0, 1
        MCR     p15, 0, R1,c3,c0, 0
        MCR     p15, 0, R2,c5,c0, 2
        MCR     p15, 0, R3,c5,c0, 3
        MCR     p15, 0, R4,c6,c4, 0
        MRC     p15, 0, R0,c2,c0, 0
        MRC     p15, 0, R1,c2,c0, 1
        MRC     p15, 0, R2,c3,c0, 0
        ORR     R0, R0, #0x20
        ORR     R1, R1, #0x20
        ORR     R2, R2, #0x20
        MCR     p15, 0, R0,c2,c0, 0
        MCR     p15, 0, R1,c2,c0, 1
        MCR     p15, 0, R2,c3,c0, 0

    copyFirmPartitions:
        LDR     R4, =buffer
        ADD     R3, R4, #0x40
        LDR     R0, [R3]
        ADD     R0, R0, R4
        LDR     R1, [R3,#4]
        LDR     R2, [R3,#8]
        BL      MemCopy
        ADD     R3, R4, #0x70
        LDR     R0, [R3]
        ADD     R0, R0, R4
        LDR     R1, [R3,#4]
        LDR     R2, [R3,#8]
        BL      MemCopy
        ADD     R3, R4, #0xA0
        LDR     R0, [R3]
        ADD     R0, R0, R4
        LDR     R1, [R3,#4]
        LDR     R2, [R3,#8]
        BL      MemCopy
		ADD     R3, R4, #0xD0
        LDR     R0, [R3]
		CMP		R0, #0
		BEQ		invalidateDataCache
        ADD     R0, R0, R4
        LDR     R1, [R3,#4]
        LDR     R2, [R3,#8]
        BL      MemCopy

    invalidateDataCache:
        MOV     R2, #0
        MOV     R1, R2
        loc_809460C:
        MOV     R0, #0
        MOV     R3, R2,LSL#30
        loc_8094614:
        ORR     R12, R3, R0,LSL#5
        MCR     p15, 0, R1,c7,c10, 4
        MCR     p15, 0, R12,c7,c14, 2
        ADD     R0, R0, #1
        CMP     R0, #0x20
        BCC     loc_8094614
        ADD     R2, R2, #1
        CMP     R2, #4
        BCC     loc_809460C

    jumpToEntrypoint:
        MCR     p15, 0, R1,c7,c10, 4
        LDR     R0, =0x42078
        MCR     p15, 0, R0,c1,c0, 0
        MCR     p15, 0, R1,c7,c5, 0
        MCR     p15, 0, R1,c7,c6, 0
        MCR     p15, 0, R1,c7,c10, 4
        LDR     R0, =buffer
        MOV     R1, #0x1FFFFFFC
        LDR     R2, [R0,#8]
        STR     R2, [R1]
        LDR     R0, [R0,#0xC]
        ADD     SP, SP, #0x3C
        LDMFD   SP!, {R4-R11,LR}
        BX      R0
.pool

//--------------------------------------------
MemCopy:
                 MOV     R12, LR
                 STMFD   SP!, {R0-R4}
                 ADD     R2, R2, R0

 loc_80C4220:
                 LDR     R3, [R0],#4
                 STR     R3, [R1],#4
                 CMP     R0, R2
                 BLT     loc_80C4220
                 LDMFD   SP!, {R0-R4}
                 MOV     LR, R12
                 BX      LR
.pool
