// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
//FIRM Reboot patch, which is necessary for the hi-ram using games to boot.
//TODO : Do not exclude other FIRMS to be opened. Not important until we patch them for emunand

//----------------- GLOBALS -----------------------
//Taken From NATIVE_FIRM 9.6
.equ fopen9,        0x0805B180
.equ fread9,        0x0804D9B0
.equ fwrite9,       0x0805C4D0
.equ readpxi,       0x08055178
.equ filehandle,    0x2000E000
.equ byteswritten,  0x2000E100
.equ buffer,        0x24000000

//----------------- PROCESS9 ----------------------
.arm
.align 2
	GetFileName:
		ldr r2, =filename
		mov r1, #0x30
		add r0, sp, #0x3A8-0x70
		ldr r4, =0x080282CF @blx 0x080282CE
		blx r4
		
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
        ldr r0, =0x80FF4FC
        svc 0x7B

    InfiniteLoop:
        b InfiniteLoop

.pool
filename:
	//.string16 "nand:/rxtools/%08x%08x.bin"
	.string16 "sdmc:/rxtools/data/%08x%08x.bin"
