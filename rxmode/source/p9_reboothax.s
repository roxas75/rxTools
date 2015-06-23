// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
//FIRM Reboot patch, which is necessary for the hi-ram using games to boot.
//TODO : Do not exclude other FIRMS to be opened. Not important until we patch them for emunand
//----------------- GLOBALS -----------------------
//Taken From NATIVE_FIRM 9.6
.definelabel fopen9,        0x0805B180
.definelabel fread9,        0x0804D9B0
.definelabel fwrite9,       0x0805C4D0
.definelabel readpxi,       0x08055178
.definelabel filehandle,    0x2000E000
.definelabel byteswritten,  0x2000E100
.definelabel buffer,        0x24000000

//----------------- PROCESS9 ----------------------
.nds
.create build/080859C8.bin, 0x080859C8
.arm
.align 4

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
        ldr r1, =filename
        mov r2, #1
        blx fopen9

    Fseek:
    //Crappy method... if only i knew the addr of the real fseek, lol
    //So we read until the cfw address and put the crap in a unsuseful place
    ldr r0, =filehandle
    ldr r1, =byteswritten
    ldr r2, =0x21000000
    ldr r3, =0x200000
    blx fread9

    ReadFirm:
        ldr r0, =filehandle
        ldr r1, =byteswritten
        ldr r2, =buffer
        ldr r3, =0xF0000
        blx fread9

    DecryptFirm:
        ldr r0, =buffer
        ldr r1, =0xF0000
        ldr r2, =0xDEADBEEF
        ldr r4, =0x12345678
        add r1, r0
        dfl1:
            ldr r3, [r0]
            eor r3, r2
            str r3, [r0]
            add r0, #4
            add r2, r4
            cmp r0, r1
            blt dfl1

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
        .word 0xEF00007C    //SVC 0x7C

    GoToReboot:
        ldr r0, =0x80FF4FC
        .word 0xEF00007B    //SVC 0x7B

    InfiniteLoop:
        b InfiniteLoop

.pool
filename:
dcw "sdmc:/rxTools.dat"
.halfword 0
.close

//----------------- REBOOT FUNCTION ----------------
.nds
.create build/08094454.bin, 0x080FF600
.arm
.align 4

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
        MOV     R1, 0x1FFFFFFC
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

.close
