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

compat_app_s app_42_cn = {
	.memcpy = (void *)0x0023F048,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x00190118,
	.svcSleepThread = (void *)0x00104218,
	.svcControlMemory = (void *)0x00142F64,
	.srvGetServiceHandle = (void *)0x00114E0C,

	.IFile_Open = (void *)0x0022E334,
	.IFile_Read = (void *)0x001674BC,
	.IFile_Write = (void *)0x00167544,

	.srv_handle = (Handle *)0x003D974C
};

compat_app_s app_45_cn = {
	.memcpy = (void *)0x0023EFA0,
	.GX_SetTextureCopy = (void *)0x0011DD68,
	.GSPGPU_FlushDataCache = (void *)0x0018FC0C,
	.svcSleepThread = (void *)0x0010420C,
	.svcControlMemory = (void *)0x00142F58,
	.srvGetServiceHandle = (void *)0x00114E34,

	.IFile_Open = (void *)0x0022E2B0,
	.IFile_Read = (void *)0x00166FC8,
	.IFile_Write = (void *)0x00167050,

	.srv_handle = (Handle *)0x003D974C
};

compat_app_s app_5x_cn = {
	.memcpy = (void *)0x0023F80C,
	.GX_SetTextureCopy = (void *)0x0011DD80,
	.GSPGPU_FlushDataCache = (void *)0x001902A8,
	.svcSleepThread = (void *)0x0010420C,
	.svcControlMemory = (void *)0x00143110,
	.srvGetServiceHandle = (void *)0x00114E30,

	.IFile_Open = (void *)0x0022EA5C,
	.IFile_Read = (void *)0x0016751C,
	.IFile_Write = (void *)0x001675A4,

	.srv_handle = (Handle *)0x003D966C
};

compat_app_s app_9x_cn = {
	.memcpy = (void *)0x0023F808,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x001902B8,
	.svcSleepThread = (void *)0x001041F8,
	.svcControlMemory = (void *)0x001430F0,
	.srvGetServiceHandle = (void *)0x00114E00,

	.IFile_Open = (void *)0x0022EA24,
	.IFile_Read = (void *)0x00167540,
	.IFile_Write = (void *)0x001675C8,

	.srv_handle = (Handle *)0x003D966C
};

compat_app_s app_4x_kr = {
	.memcpy = (void *)0x0023FF90,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x00190D30,
	.svcSleepThread = (void *)0x00104218,
	.svcControlMemory = (void *)0x00142FA0,
	.srvGetServiceHandle = (void *)0x00114E0C,

	.IFile_Open = (void *)0x0022F284,
	.IFile_Read = (void *)0x001680F8,
	.IFile_Write = (void *)0x00168180,

	.srv_handle = (Handle *)0x003DA74C
};

compat_app_s app_5x_kr = {
	.memcpy = (void *)0x002407DC,
	.GX_SetTextureCopy = (void *)0x0011DD80,
	.GSPGPU_FlushDataCache = (void *)0x0019154C,
	.svcSleepThread = (void *)0x0010420C,
	.svcControlMemory = (void *)0x00143108,
	.srvGetServiceHandle = (void *)0x00114E30,

	.IFile_Open = (void *)0x0022FAC8,
	.IFile_Read = (void *)0x001686FC,
	.IFile_Write = (void *)0x00168784,

	.srv_handle = (Handle *)0x003D966C
};

compat_app_s app_9x_kr = {
	.memcpy = (void *)0x002407D4,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x00191554,
	.svcSleepThread = (void *)0x001041F8,
	.svcControlMemory = (void *)0x001430E8,
	.srvGetServiceHandle = (void *)0x00114E00,

	.IFile_Open = (void *)0x0022FA8C,
	.IFile_Read = (void *)0x00168718,
	.IFile_Write = (void *)0x001687A0,

	.srv_handle = (Handle *)0x003D966C
};

compat_app_s app_4x_tw = {
	.memcpy = (void *)0x0023FFE4,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x00190D34,
	.svcSleepThread = (void *)0x00104218,
	.svcControlMemory = (void *)0x00142F64,
	.srvGetServiceHandle = (void *)0x00114E0C,

	.IFile_Open = (void *)0x0022F2D8,
	.IFile_Read = (void *)0x001680FC,
	.IFile_Write = (void *)0x00168184,

	.srv_handle = (Handle *)0x003DA74C
};

compat_app_s app_5x_tw = {
	.memcpy = (void *)0x00240870,
	.GX_SetTextureCopy = (void *)0x0011DD80,
	.GSPGPU_FlushDataCache = (void *)0x00191594,
	.svcSleepThread = (void *)0x0010420C,
	.svcControlMemory = (void *)0x00143110,
	.srvGetServiceHandle = (void *)0x00114E30,

	.IFile_Open = (void *)0x0022FB5C,
	.IFile_Read = (void *)0x00168744,
	.IFile_Write = (void *)0x001687CC,

	.srv_handle = (Handle *)0x003D966C
};

compat_app_s app_9x_tw = {
	.memcpy = (void *)0x00240868,
	.GX_SetTextureCopy = (void *)0x0011DD48,
	.GSPGPU_FlushDataCache = (void *)0x0019159C,
	.svcSleepThread = (void *)0x001041F8,
	.svcControlMemory = (void *)0x001430F0,
	.srvGetServiceHandle = (void *)0x00114E00,

	.IFile_Open = (void *)0x0022FB20,
	.IFile_Read = (void *)0x00168760,
	.IFile_Write = (void *)0x001687E8,

	.srv_handle = (Handle *)0x003D966C
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

enum rop_type
{
	MSET_4X = 0,
	MSET_4X_DG,
	MSET_6X
};

void init_compat()
{
	const compat_app_s *app = NULL;
	uint32_t with_mset6 = 0;
	uint32_t app_spec = *(uint32_t *)0x0010000C;
	switch(app_spec)
	{
	case 0xEB0676B5: // spider 4.x
		app = &app_4x;
		break;
	case 0xEB050B2A: // spider 5.x
		app = &app_5x;
		with_mset6 = 1;
		break;
	case 0xEB050B28: // spider 9.0
		app = &app_9x;
		with_mset6 = 1;
		break;
	case 0xEB050466:
		app = &app_42_cn;
		break;
	case 0xEB05043C:
		app = &app_45_cn;
		break;
	case 0xEB050657:
		app = &app_5x_cn;
		break;
	case 0xEB050656:
		app = &app_9x_cn;
		break;
	case 0xEB050838:
		app = &app_4x_kr;
		break;
	case 0xEB050A4B:
		app = &app_5x_kr;
		with_mset6 = 1;
		break;
	case 0xEB050A49:
		app = &app_9x_kr;
		with_mset6 = 1;
		break;
	case 0xEB05084D:
		app = &app_4x_tw;
		break;
	case 0xEB050A70:
		app = &app_5x_tw;
		break;
	case 0xEB050A6E:
		app = &app_9x_tw;
		break;
	}

	_memcpy(&compat.app, app, sizeof(compat_app_s));

	uint32_t kernel_version = *(uint32_t *)0x1FF80000;
	switch(kernel_version)
	{
	case 0x02220000: // 2.34-0 4.1.0
		compat.create_thread_patch = 0xEFF83C97;
		compat.svc_patch = 0xEFF827CC;

		compat.patch_sel = MSET_4X;
		break;
	case 0x02230600: // 2.35-6 5.0.0
		compat.create_thread_patch = 0xEFF8372F;
		compat.svc_patch = 0xEFF822A8;

		compat.patch_sel = MSET_4X_DG;
		break;
	case 0x02240000: // 2.36-0 5.1.0
		compat.create_thread_patch = 0xEFF8372B;
		compat.svc_patch = 0xEFF822A4;

		compat.patch_sel = MSET_4X_DG;
		break;
	case 0x02270400: // 2.39-4 7.0.0	
		compat.create_thread_patch = 0xEFF8372F;
		compat.svc_patch = 0xEFF822A8;

		compat.patch_sel = MSET_6X;
		break;
	case 0x02250000: // 2.37-0 6.0.0
	case 0x02260000: // 2.38-0 6.1.0
	case 0x02280000: // 2.40-0 7.2.0			
		compat.create_thread_patch = 0xEFF8372B;
		compat.svc_patch = 0xEFF822A4;

		compat.patch_sel = MSET_6X;
		break;
	case 0x022C0600: // 2.44-6 8.0.0
		compat.create_thread_patch = 0xDFF83767;
		compat.svc_patch = 0xDFF82294;

		compat.patch_sel = MSET_6X;
		break;
	case 0x022E0000: // 2.26-0 9.0.0
		compat.create_thread_patch = 0xDFF83837;
		compat.svc_patch = 0xDFF82290;

		compat.patch_sel = MSET_6X;
		break;
	}
	
	// Install ROP for 4.x DG, this is for CN/TW which doesn't have mset 6.x
	if(compat.patch_sel == MSET_6X && !with_mset6)
		compat.patch_sel = MSET_4X_DG;

	compat.firmver = kernel_version;
}
