int run_exploit();

typedef struct arm11_shared_data {
	u32 va_pdn_regs;
	u32 va_pxi_regs;
	u32 va_hook1_ret;
};

#define ARM_JUMPOUT 0xE51FF004 // LDR PC, [PC, -#04]
#define ARM_RET     0xE12FFF1E // BX LR
#define ARM_NOP     0xE1A00000 // NOP
