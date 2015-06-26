// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
//FIRM Reboot patch, which is necessary for the hi-ram using games to boot.
//TODO : Do not exclude other FIRMS to be opened. Not important until we patch them for emunand
//----------------- GLOBALS -----------------------
//Taken From NATIVE_FIRM 9.6
.equ load_offset,   0x080859FC

.equ fopen9,        0x0805B180-load_offset
.equ fread9,        0x0804D9B0-load_offset
.equ fwrite9,       0x0805C4D0-load_offset
.equ readpxi,       0x08055178-load_offset
.equ filehandle,    0x2000E000
.equ byteswritten,  0x2000E100
.equ buffer,        0x24000000

//----------------- PROCESS9 ----------------------
.arm
.align 2

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
.string16 "sdmc:/rxTools.dat"
