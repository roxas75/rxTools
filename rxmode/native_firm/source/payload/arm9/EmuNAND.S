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

.section .rodata.nand.sector, "a"
nandTopSector:
	.word 0xAAAABBBB

.arm
.text
.global EMUNAND_WRITE

#ifdef PLATFORM_KTR
EntryPoint = 0x080D86F0
Break_Func = 0x08062891
#else
EntryPoint = 0x080D8670
Break_Func = 0x08062A29
#endif

EMUNAND_WRITE:
				 STMFD   SP!, {R0-R3}
                 MOV     R3, R0
                 LDR     R1, =bytes1
                 LDR     R2, [R3,#4]
                 STR     R2, [R1,#4]
                 LDR     R0, =EntryPoint
                 CMP     R2, R0
                 LDR     R2, [R3,#8]
                 STR     R2, [R1]
                 LDRNE   R1, =bytes2
                 LDRNE   R1, [R1]
                 CMPNE   R1, #0
                 BEQ     loc_801A310
                 LDR     R1, =EntryPoint
                 STR     R1, [R3,#4]
				 cmp     r2, #0
				 ldreq   r2, =nandTopSector
				 ldreq   r2, [r2]
                 STR     R2, [R3,#8]

 loc_801A310:
                 LDMFD   SP!, {R0-R3}
                 MOVS    R4, R0
                 MOVS    R5, R1
                 MOVS    R7, R2
                 MOVS    R6, R3
                 MOVS    R0, R1,LSL#23
                 BEQ     loc_801A33C
                 STMFD   SP!, {R4}
                 LDR     R4, =Break_Func
                 BLX     R4
                 LDMFD   SP!, {R4}

 loc_801A33C:
                 LDR     R0, [R4,#4]
                 LDR     R1, [R0]
                 LDR     R1, [R1,#0x18]
                 BLX     R1
                 LDR     R1, [R4,#4]
                 MOVS    R3, R0
                 LDR     R0, [R1,#0x20]
                 MOVS    R2, R5,LSR#9
                 MOV     R12, R0
                 LDR     R0, [R4,#8]
                 STR     R7, [SP,#4]
                 ADDS    R0, R0, R2
                 CMP     R1, #0
                 STR     R6, [SP,#8]
                 STR     R0, [SP]
                 BEQ     loc_801A380
                 ADDS    R1, R1, #8

 loc_801A380:
                 MOVS    R2, R4
                 ADDS    R2, R2, #0xC
                 MOV     R0, R12
#ifdef PLATFORM_KTR
                 LDR     R5, =0x080788C1
#else
                 LDR     R5, =0x08078971
#endif
                 BLX     R5
                 STMFD   SP!, {R0-R3}
                 ldr r2, =bytes1
                 LDR     R1, [R2]
                 STR     R1, [R4,#8]
                 LDR     R1, [R2,#4]
                 STR     R1, [R4,#4]
                 LDMFD   SP!, {R0-R3}
                 LDMFD   SP!, {R1-R7,PC}

.global EMUNAND_READ
EMUNAND_READ:
				 STMFD   SP!, {R0-R3}
                 MOV     R3, R0
				 LDR r1, =bytes1
                 LDR     R2, [R3,#4]
                 STR     R2, [R1,#4]
                 LDR     R0, =EntryPoint
                 CMP     R2, R0
                 LDR     R2, [R3,#8]
                 STR     R2, [R1]
                 LDRNE   R1, =bytes2
                 LDRNE   R1, [R1]
                 CMPNE   R1, #0
                 BEQ     loc_801A40C
                 LDR     R1, =EntryPoint
                 STR     R1, [R3,#4]
                 cmp     r2, #0
				 ldreq   r2, =nandTopSector
				 ldreq   r2, [r2]
                 STR     R2, [R3,#8]

 loc_801A40C:
                 LDMFD   SP!, {R0-R3}
                 MOVS    R4, R0
                 MOVS    R5, R1
                 MOVS    R7, R2
                 MOVS    R6, R3
                 MOVS    R0, R1,LSL#23
                 BEQ     loc_801A438
                 STMFD   SP!, {R4}
                 LDR     R4, =Break_Func
                 BLX     R4
                 LDMFD   SP!, {R4}

 loc_801A438:
                 LDR     R0, [R4,#4]
                 LDR     R1, [R0]
                 LDR     R1, [R1,#0x18]
                 BLX     R1
                 LDR     R1, [R4,#4]
                 MOVS    R3, R0
                 LDR     R0, [R1,#0x20]
                 MOVS    R2, R5,LSR#9
                 MOV     R12, R0
                 LDR     R0, [R4,#8]
                 STR     R7, [SP,#4]
                 ADDS    R0, R0, R2
                 CMP     R1, #0
                 STR     R6, [SP,#8]
                 STR     R0, [SP]
                 BEQ     loc_801A47C
                 ADDS    R1, R1, #8

 loc_801A47C:
                 MOVS    R2, R4
                 ADDS    R2, R2, #0xC
                 MOV     R0, R12
#ifdef PLATFORM_KTR
                 LDR     R5, =0x080786A1
#else
                 LDR     R5, =0x08078751
#endif
                 BLX     R5
                 STMFD   SP!, {R0-R3}
                 ldr r2, =bytes1
                 LDR     R1, [R2]
                 STR     R1, [R4,#8]
                 LDR     R1, [R2,#4]
                 STR     R1, [R4,#4]
                 LDMFD   SP!, {R0-R3}
                 LDMFD   SP!, {R1-R7,PC}

bytes1:
	.word 0
	.word 0
bytes2:
	.word 1
	.word 0
