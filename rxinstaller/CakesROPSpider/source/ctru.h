#ifndef CTRU_H
#define CTRU_H

#include "types.h"

// Copied from ctrulib cause static linking fail
u32* getThreadCommandBuffer(void);

Result CFGNOR_Initialize(Handle CFGNOR_handle, u8 value);
Result CFGNOR_Shutdown(Handle CFGNOR_handle);
Result CFGNOR_ReadData(Handle CFGNOR_handle, u32 offset, u32 *buf, u32 size);
Result CFGNOR_WriteData(Handle CFGNOR_handle, u32 offset, u32 *buf, u32 size);

#endif
