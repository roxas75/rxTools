#ifndef FILEPACK_H
#define FILEPACK_H

enum{
	TITLE_KEY = 0,
	REBOOT,
	PATCH,
	TOP_PIC,
};

typedef struct{
	unsigned int off;
	unsigned int size;
	unsigned int hash;
	unsigned int edited;
}PackEntry;

void LoadPack();
void SavePack();
unsigned char* GetFilePack(int filenumber);
PackEntry* GetEntryPack(int filenumber);

unsigned int HashGen(unsigned char* file, unsigned int size);
int CheckHash(unsigned char* file, unsigned int size, unsigned int hash);  //0 if wrong signature

#endif