@---------------------------------------------------------------------------------
	.section ".init"
@---------------------------------------------------------------------------------
	.global _start
	.align	4
	.arm

_start:
				 MSR     CPSR_c, #0xDF
                 LDR     R0, =0x10000035
                 MCR     p15, 0, R0,c6,c3, 0
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
                 MOV     R0, #0x8000000
                 ADR     R1, InfiniteLoop
                 STR     R1, [R0,#4]
                 STR     R1, [R0,#0x14]
                 STR     R1, [R0,#0x1C]
                 STR     R1, [R0,#0x24]
                 STR     R1, [R0,#0x2C]
                 ADR     LR, InfiniteLoop
				 BL 	 FlushDataCache
 
 CopyPartitions:
				 LDR     R4, =0x24000000
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
				 BL 	 FlushDataCache
				
 RelaunchFIRM:
				 LDR     R3, =0x1FFFFFF8			//That's GW trick
                 LDR     R2, =0x24000008
				 LDR 	 R2, [R2]
                 STR     R2, [R3]
                 BL      FlushDataCache
                 MOV     R0, #0
                 MCR     p15, 0, R0,c7,c5, 0
                 MOV     R0, #0
                 MCR     p15, 0, R0,c7,c6, 0
                 LDR     R3, =0x10001000
                 MOVS    R2, #0
                 STR     R2, [R3]
                 LDR     R3, =0x10001004
                 MOVS    R2, #1
                 NEGS    R2, R2
                 STR     R2, [R3]
                 LDR     R3, =0x10000010
                 MOVS    R2, #0xC
                 STRB    R2, [R3]
                 LDR     R3, =0x10164000
                 MOVS    R2, #0
                 STRH    R2, [R3]
                 LDR     R3, =0x10164004
                 MOVS    R2, #0
                 STR     R2, [R3]
                 LDR     R3, =0x10004000
                 MOVS    R2, #0
                 STR     R2, [R3]
                 LDR     R3, =0x10005000
                 MOVS    R2, #0
                 STR     R2, [R3]
                 LDR     R3, =0x10001004
                 MOVS    R2, #1
                 NEGS    R2, R2
                 STR     R2, [R3]
                 LDR     R3, =0x10141200
                 LDR     R2, =0x1007E
                 STR     R2, [R3]
                 MOVS    R0, #0xA
                 BL      sub_80F0AB8
                 LDR     R3, =0x10141200
                 LDR     R2, =0x1007F
                 STR     R2, [R3]
                 MOVS    R0, #0xA
                 BL      sub_80F0AB8
                 MOV     R1, #0
                 LDR     R0, =0x42078
                 MCR     p15, 0, R0,c1,c0, 0
                 MCR     p15, 0, R1,c7,c5, 0
                 MCR     p15, 0, R1,c7,c6, 0
                 MCR     p15, 0, R1,c7,c10, 4
                 LDR     R0, =0x801B01C
				 BX 	 R0
 

InfiniteLoop:
	b InfiniteLoop
	
.pool
	
//---------------------------------------------------------
.global MemCopy
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
//---------------------------------------------------------
.global FlushDataCache
FlushDataCache:
				 MOV     R12, #0

 loc_80F6BA4:
                 MOV     R0, #0
                 MOV     R2, R12,LSL#30

 loc_80F6BAC:
                 ORR     R1, R2, R0,LSL#5
                 MCR     p15, 0, R1,c7,c14, 2
                 ADD     R0, R0, #1
                 CMP     R0, #0x20
                 BCC     loc_80F6BAC
                 ADD     R12, R12, #1
                 CMP     R12, #4
                 BCC     loc_80F6BA4
                 MOV     R0, #0
                 MCR     p15, 0, R0,c7,c10, 4
				 bx lr
.pool
//---------------------------------------------------------		
.global sub_80F0AB8		 
sub_80F0AB8:

 var_14          = -0x14
 var_C           = -0xC

                 PUSH    {LR}
                 SUB     SP, SP, #0x14
                 STR     R0, [SP,#0x18+var_14]
                 LDR     R3, [SP,#0x18+var_14]
                 STR     R3, [SP,#0x18+var_C]
                 B       loc_80F0ACA

 loc_80F0AC4:
                 LDR     R3, [SP,#0x18+var_C]
                 SUBS    R3, #1
                 STR     R3, [SP,#0x18+var_C]

 loc_80F0ACA:
                 LDR     R3, [SP,#0x18+var_C]
                 CMP     R3, #0
                 BNE     loc_80F0AC4
                 ADD     SP, SP, #0x14
                 POP     {PC}
.pool
//---------------------------------------------------------	


