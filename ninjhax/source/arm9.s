.arm
.align 4
.code 32
.text

.global arm9_start
@ insert your funky stuff here
@ this code will not be executed if "/arm9payload.bin" is present
arm9_start:
	B               skipvars

	@ offs 4, to be filled during runtime
	pa_arm9_entrypoint_backup: .long 0
	
skipvars:
	STMFD           SP!, {R0-R12,LR}
	ldr r0, =0x20000000
	ldr r1, =0x21000000
	ldr r2, =0xFFFFFFFF
	loop1:
		str r2, [r0], #4
		cmp r0, r1
		blt loop1
		
	@ delay execution. just for "fun"
	MOV             R1, #0x10
outer:
	
	MOV             R0, #0
	ADD             R0, R0, #0xFFFFFFFF
inner:
	SUBS            R0, R0, #1
	NOP
	BGT             inner
	SUBS            R1, R1, #1
	BGT             outer
	
		
	LDMFD           SP!, {R0-R12,LR}
	
	LDR             PC, pa_arm9_entrypoint_backup
	
.global arm9_end
arm9_end:
