#include "common.h"
#include "filepack.h"
#include "fs.h"
#include "draw.h"
#include "CTRDecryptor.h"
#include "console.h"

#define FILEPACK_ADDR	(void*)0x20400000
#define FILEPACK_OFF	0x100000			//Offset in Launcher.dat
#define FILEPACK_SIZE	0x100000

static unsigned char KeyY[16] = {0x98, 0x23, 0x48, 0xF9, 0x8E, 0xF9, 0x0E, 0x89, 0xF8, 0x29, 0x3F, 0x89, 0x23, 0x8E, 0x98, 0xF8 };
static unsigned char Ctr[16] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int nEntry = 0;
PackEntry *Entry;    //max 100 files
unsigned int curSize = 0;

PartitionInfo packInfo;

void LoadPack(){
	packInfo.keyslot = 0x2C; packInfo.keyY = KeyY; packInfo.ctr = Ctr; packInfo.size = FILEPACK_SIZE; packInfo.buffer = FILEPACK_ADDR;
	File FilePack; FileOpen(&FilePack, "rxTools.dat", 0);
	curSize = FileRead(&FilePack, FILEPACK_ADDR, FILEPACK_SIZE, FILEPACK_OFF);
	FileClose(&FilePack);
	DecryptPartition(&packInfo);

	nEntry = *((unsigned int*)(FILEPACK_ADDR));
	Entry = ((PackEntry*)(FILEPACK_ADDR + 0x10));
	for(int i = 0; i < nEntry; i++){
		Entry[i].off += (int)FILEPACK_ADDR;
		if(!CheckHash((void*)Entry[i].off, Entry[i].size, Entry[i].hash)){
			DrawString(TOP_SCREEN, " rxTools.dat is corrupted!", 0, 240-8, BLACK, WHITE); 		//Who knows, if there is any corruption in our files, we need to stop
			while(1);
		}
	}
}

void SavePack(){
	File FilePack; FileOpen(&FilePack, "rxTools.dat", 0);
	DecryptPartition(&packInfo);
	FileWrite(&FilePack, FILEPACK_ADDR, FILEPACK_SIZE, FILEPACK_OFF);
	FileClose(&FilePack);
	DecryptPartition(&packInfo);
}

void* GetFilePack(char* name){
	for(int i = 0; i < nEntry; i++){
		if(strncmp(name, Entry[i].name, 16) == 0) return (void*)Entry[i].off;
	}
	return NULL;
}

PackEntry* GetEntryPack(int filenumber){
	if(filenumber < nEntry)
		return &Entry[filenumber];
	else return NULL;
}

int CheckHash(unsigned char* file, unsigned int size, unsigned int hash){ //BSD checksum
	unsigned int HASH = 0;
	for(int i = 0; i < size; HASH = (((HASH >> 1) | (HASH << 15)) + file[i++]) & 0xffff);
	if(HASH == hash) return 1;
	else return 0;
}

unsigned int HashGen(unsigned char* file, unsigned int size){
	unsigned int HASH = 0;
	for(int i = 0; i < size; HASH = (((HASH >> 1) | (HASH << 15)) + file[i++]) & 0xffff);
	return HASH;
}
