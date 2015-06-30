#ifndef TITLE_KEY_DECRYPT_H
#define TITLE_KEY_DECRYPT_H

#include "common.h"

void DecryptTitleKeys();
void DecryptTitleKeyFile(void);
u32 DecryptTitleKey(u8 *titleid, u8 *key, u32 index);
int GetTitleKey(u8 *TitleKey, u32 low, u32 high);

#endif
