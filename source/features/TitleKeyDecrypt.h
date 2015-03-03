#ifndef TITLE_KEY_DECRYPT_H
#define TITLE_KEY_DECRYPT_H

void DecryptTitleKeys();
unsigned int DecryptTitleKey(unsigned char* titleid, unsigned char* key, unsigned int index);
void DumpTicketDb();
#endif