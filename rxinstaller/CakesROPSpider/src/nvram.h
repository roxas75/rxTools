#ifndef NVRAM_H
#define NVRAM_H

#include "types.h"

Result DumpNVRAM(Handle CFGNOR_handle);
Result PatchNVRAM(Handle CFGNOR_handle);

#endif
