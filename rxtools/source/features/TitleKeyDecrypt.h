#ifndef TITLE_KEY_DECRYPT_H
#define TITLE_KEY_DECRYPT_H

void DecryptTitleKeys();
u32 DecryptTitleKey(u8 *titleid, u8 *key, u32 index);
int GetTitleKey(u8 *TitleKey, u32 low, u32 high);

#endif
