#include "ctru.h"
#include "compat.h"
#include "svc.h"

inline void* getThreadLocalStorage(void)
{
	void* ret;
	asm volatile("mrc p15, 0, %[data], c13, c0, 3" : [data] "=r" (ret));
	return ret;
}

u32* getThreadCommandBuffer(void)
{
	return (u32*)((u8*)getThreadLocalStorage() + 0x80);
}

/* cue copypasta from ctrulib we can't link in because static linkage fail */
Result CFGNOR_Initialize(Handle CFGNOR_handle, u8 value)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x00010040;
	cmdbuf[1] = (u32)value;

	if((ret = svcSendSyncRequest(CFGNOR_handle))!=0)return ret;

	ret = (Result)cmdbuf[1];
	return ret;
}

Result CFGNOR_Shutdown(Handle CFGNOR_handle)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x00020000;

	if((ret = svcSendSyncRequest(CFGNOR_handle))!=0)return ret;
	ret = (Result)cmdbuf[1];

	svcCloseHandle(CFGNOR_handle);
	CFGNOR_handle = 0;

	return ret;
}

Result CFGNOR_ReadData(Handle CFGNOR_handle, u32 offset, u32 *buf, u32 size)
{
	Result ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x00050082;
	cmdbuf[1] = offset;
	cmdbuf[2] = size;
	cmdbuf[3] = (size<<4) | 12;
	cmdbuf[4] = (u32)buf;

	if((ret = svcSendSyncRequest(CFGNOR_handle))!=0)return ret;

	ret = (Result)cmdbuf[1];
	return ret;
}

Result CFGNOR_WriteData(Handle CFGNOR_handle, u32 offset, u32 *buf, u32 size)
{
	u32 ret = 0;
	u32 *cmdbuf = getThreadCommandBuffer();

	cmdbuf[0] = 0x00060082;
	cmdbuf[1] = offset;
	cmdbuf[2] = size;
	cmdbuf[3] = (size<<4) | 10;
	cmdbuf[4] = (u32)buf;

	if((ret = svcSendSyncRequest(CFGNOR_handle))!=0)return ret;

	ret = (Result)cmdbuf[1];
	return ret;
}
