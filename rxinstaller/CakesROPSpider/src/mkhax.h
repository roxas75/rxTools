#ifndef MKHAX_H
#define MKHAX_H
/*
 * Myria's libkhax implementation in C
 */

#include "types.h"

void arm11_kernel_exploit_setup(void);
void patch_srv_access();

Result srvGetServiceHandle(Handle* out, const char* name);

#endif
