#include "compat.h"

__attribute__((section(".compat")))
compat_s compat;

compat_app_s app_4x = {
	.memcpy = (void *)0x0029BF60,
	.GX_SetTextureCopy = (void *)0x002C62E4,
	.GSPGPU_FlushDataCache = (void *)0x00344B84,
	.svcSleepThread = (void *)0x002A513C,
	.svcControlMemory = (void *)0x002D6ADC,
	.srvGetServiceHandle = (void *)0x002B7ECC,

	.IFile_Open = (void *)0x0025B0A4,
	.IFile_Read = (void *)0x002FC8E4,
	.IFile_Write = (void *)0x00311D90,

	.srv_handle = (Handle *)0x003B644C
};

compat_app_s app_5x = {
	.memcpy = (void *)0x00240B58,
	.GX_SetTextureCopy = (void *)0x0011DD80,
	.GSPGPU_FlushDataCache = (void *)0x001914FC,
	.svcSleepThread = (void *)0x0010420C,
	.svcControlMemory = (void *)0x001431C0,
	.srvGetServiceHandle = (void *)0x00114E30,

	.IFile_Open = (void *)0x0022FE44,
	.IFile_Read = (void *)0x001686C0,
	.IFile_Write = (void *)0x00168748,

	.srv_handle = (Handle *)0x003D968C
};

compat_app_s app_9x = {
	.memcpy = (void *)0x00240B50,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x00191504,
	.svcSleepThread = (void *)0x0023FFE8,
	.svcControlMemory = (void *)0x001431A0,
	.srvGetServiceHandle = (void *)0x00114E00,

	.IFile_Open = (void *)0x0022FE08,
	.IFile_Read = (void *)0x001686DC,
	.IFile_Write = (void *)0x00168764,

	.srv_handle = (Handle *)0x003D968C
};

// Slow ass but safe memcpy
void _memcpy(void* dst, const void* src, uint32_t size)
{
	char *destc = (char *) dst;
	const char *srcc = (const char *) src;
	for(uint32_t i = 0; i < size; i++)
	{
		destc[i] = srcc[i];
	}
}

void _memset(void* dst, int val, uint32_t size)
{
	char *destc = (char *) dst;
	for(uint32_t i = 0; i < size; i++)
	{
		destc[i] = (char)val;
	}
}

void init_compat()
{
	const compat_app_s *app = NULL;
	uint32_t app_spec = *(uint32_t *)0x0010000C;
	switch(app_spec)
	{
	case 0xEB0676B5: // spider 4.x
		app = &app_4x;
		break;
	case 0xEB050B2A: // spider 5.x
		app = &app_5x;
		break;
	case 0xEB050B28: // spider 9.0
		app = &app_9x;
		break;
	}

	_memcpy(&compat.app, app, sizeof(compat_app_s));

	uint32_t kernel_version = *(uint32_t *)0x1FF80000;
	switch(kernel_version)
	{
	case 0x02220000: // 2.34-0 4.1.0
		compat.create_thread_patch = 0xEFF83C97;
		compat.svc_patch = 0xEFF827CC;

		// mset 4.x
		compat.patch_sel = 0;
		break;
	case 0x02230600: // 2.35-6 5.0.0
	case 0x02270400: // 2.39-4 7.0.0	
		compat.create_thread_patch = 0xEFF8372F;
		compat.svc_patch = 0xEFF822A8;

		// mset 6.x
		compat.patch_sel = 2;
		break;
	case 0x02240000: // 2.36-0 5.1.0
	case 0x02250000: // 2.37-0 6.0.0
	case 0x02260000: // 2.38-0 6.1.0
	case 0x02280000: // 2.40-0 7.2.0			
		compat.create_thread_patch = 0xEFF8372B;
		compat.svc_patch = 0xEFF822A4;

		// mset 6.x
		compat.patch_sel = 2;
		break;
	case 0x022C0600: // 2.44-6 8.0.0
		compat.create_thread_patch = 0xDFF83767;
		compat.svc_patch = 0xDFF82294;

		// mset 6.x
		compat.patch_sel = 2;
		break;
	case 0x022E0000: // 2.26-0 9.0.0
		compat.create_thread_patch = 0xDFF83837;
		compat.svc_patch = 0xDFF82290;

		// mset 6.x
		compat.patch_sel = 2;
		break;
	}
	
	compat.firmver = kernel_version;
}
