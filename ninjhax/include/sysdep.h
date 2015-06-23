#define SYS_MODEL_NONE    0
#define SYS_MODEL_OLD_3DS 1
#define SYS_MODEL_NEW_3DS 2

#define PA_EXC_HANDLER_BASE        0x1FFF4000
#define OFFS_FCRAM_ARM9_PAYLOAD    0x03F00000
#define OFFS_EXC_HANDLER_UNUSED    0xC80

typedef struct exploit_data {

	u32 firm_version;
	u32 sys_model; // mask
		
	u32 va_patch_createthread;
	u32 va_patch_svc_handler;
	u32 va_patch_hook1;
	u32 va_patch_hook2;
	u32 va_hook1_ret;
				
	u32 va_fcram_base;
	u32 va_exc_handler_base_W;
	u32 va_exc_handler_base_X;
	u32 va_kernelsetstate;
	
	u32 va_pdn_regs;
	u32 va_pxi_regs;
};

// add all vulnerable systems below
struct exploit_data supported_systems[] = {
	{
		0x022E0000,        // FIRM version
		SYS_MODEL_NEW_3DS, // model
		0xDFF83837,        // VA of CreateThread code to corrupt
		0xDFF82260,        // VA of ARM11 Kernel SVC handler priv check
		0xDFFE7A50,        // VA of 1st hook for firmlaunch
		0xDFFF4994,        // VA of 2nd hook for firmlaunch
		0xFFF28A58,        // VA of return address from 1st hook 
		0xE0000000,        // VA of FCRAM
		0xDFFF4000,        // VA of lower mapped exception handler base
		0xFFFF0000,        // VA of upper mapped exception handler base
		0xFFF158F8,        // VA of the KernelSetState syscall (upper mirror)
		0xFFFBE000,        // VA PDN registers
		0xFFFC0000         // VA PXI registers		
	},
	{
		0x022C0600,        // FIRM version
		SYS_MODEL_NEW_3DS, // model
		0xDFF83837,        // VA of CreateThread code to corrupt
		0xDFF82260,        // VA of ARM11 Kernel SVC handler priv check
		0xDFFE7A50,        // VA of 1st hook for firmlaunch
		0xDFFF4994,        // VA of 2nd hook for firmlaunch
		0xFFF28A58,        // VA of return address from 1st hook 
		0xE0000000,        // VA of FCRAM
		0xDFFF4000,        // VA of lower mapped exception handler base
		0xFFFF0000,        // VA of upper mapped exception handler base
		0xFFF158F8,        // VA of the KernelSetState syscall (upper mirror)
		0xFFFBE000,        // VA PDN registers
		0xFFFC0000         // VA PXI registers		
	},
	{
		0x02220000,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83C9F,
		0xEFF827CC,
		0xEFFE4DD4,
		0xEFFF497C,
		0xFFF84DDC,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF748C4,
		0xFFFD0000,
		0xFFFD2000
	},
	{
		0x02230600,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83737,
		0xEFF822A8,
		0xEFFE55BC,
		0xEFFF4978,
		0xFFF765C4,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF64B94,
		0xFFFD0000,
		0xFFFD2000
	},
	{
		0x022E0000,
		SYS_MODEL_OLD_3DS,
		0xDFF8383F,
		0xDFF82290,
		0xDFFE59D0,
		0xDFFF4974,
		0xFFF279D8,
		0xE0000000,
		0xDFFF4000,
		0xFFFF0000,
		0xFFF151C0,
		0xFFFC2000,
		0xFFFC4000
	},
	{
		0x022C0600,
		SYS_MODEL_OLD_3DS,
		0xDFF8376F,
		0xDFF82294,
		0xDFFE4F28,
		0xDFFF4974,
		0xFFF66F30,
		0xE0000000,
		0xDFFF4000,
		0xFFFF0000,
		0xFFF54BAC,
		0xFFFBE000,
		0xFFFC0000
	},
	{
		0x02280000,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83733,
		0xEFF822A4,
		0xEFFE5B30,
		0xEFFF4974,
		0xFFF76B38,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF54BAC,
		0xFFFD0000,
		0xFFFD2000
	},
	{
		0x02270400,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83737,
		0xEFF822A8,
		0xEFFE5B34,
		0xEFFF4978,
		0xFFF76B3C,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF64AB0,
		0xFFFD0000,
		0xFFFD2000
	},
	{
		0x02250000,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83733,
		0xEFF822A4,
		0xEFFE5AE8,
		0xEFFF4978,
		0xFFF76AF0,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF64A78,
		0xFFFD0000,
		0xFFFD2000
	},
	{
		0x02260000,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83733,
		0xEFF822A4,
		0xEFFE5AE8,
		0xEFFF4978,
		0xFFF76AF0,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF64A78,
		0xFFFD0000,
		0xFFFD2000
	},
	{
		0x02240000,
		SYS_MODEL_OLD_3DS | SYS_MODEL_NEW_3DS,
		0xEFF83733,
		0xEFF822A4,
		0xEFFE55B8,
		0xEFFF4978,
		0xFFF765C0,
		0xF0000000,
		0xEFFF4000,
		0xFFFF0000,
		0xFFF64B90,
		0xFFFD0000,
		0xFFFD2000
	}
};