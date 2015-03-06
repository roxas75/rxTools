#ifndef CTR_DECRYPTOR
#define CTR_DECRYPTOR

#include "common.h"

typedef struct{
	u8* buffer;
	u8* keyY;
	u8* ctr;
	size_t size;
	u32 keyslot;
}PartitionInfo;  //This basic struct can be useful anytime, even if i'll add nand decryption/exploring

u32 DecryptPartition(PartitionInfo* info);
void CTRDecryptor();

#endif