#ifndef CFW_H
#define CFW_H

#include "common.h"

u8* decryptFirmTitle(u8* title, unsigned int size, unsigned int tid);
void LaunchCfw();
void DevMode();
void rxMode_boot();

#endif
