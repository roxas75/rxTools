#include "common.h"
#include "filepack.h"
#include "fs.h"
#include "draw.h"
#include "CTRDecryptor.h"
#include "console.h"

#define FILEPACK_ADDR	(void*)0x20400000
#define FILEPACK_OFF	0x100000			//Offset in Launcher.dat
#define FILEPACK_SIZE	0x100000

int nEntry = 0;
PackEntry *Entry;    //max 100 files
unsigned int curSize = 0;

PartitionInfo packInfo;

void LoadPack(){
	packInfo.size = FILEPACK_SIZE; packInfo.buffer = FILEPACK_ADDR; // FilePack Encryption removed. We don't really need that anymore. Make sure makefile doesn't have it encrypted.
	File FilePack; FileOpen(&FilePack, "rxTools.dat", 0);
	curSize = FileRead(&FilePack, FILEPACK_ADDR, FILEPACK_SIZE, FILEPACK_OFF);
	FileClose(&FilePack);
	
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
	FileWrite(&FilePack, FILEPACK_ADDR, FILEPACK_SIZE, FILEPACK_OFF);
	FileClose(&FilePack);
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
	if(HashGen(file,size) == hash) return 1;
	else return 0;
}

unsigned int HashGen(unsigned char* file, unsigned int size){
	unsigned tbl[256];
	unsigned crc;
	for (unsigned i = 0; i < 256; i++)
	{
	        crc = i << 24;
	        for (unsigned j = 8; j > 0; j--)
	        {
			if (crc & 0x80000000)
				crc = (crc << 1) ^ 0x04c11db7;
			else
				crc = (crc << 1);
			tbl[i] = crc;
		}
	}
	crc = 0;
	for(unsigned i = 0; i < size; i++)
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ *file++) & 0xFF];
	for (; size; size >>= 8)
		crc = (crc << 8) ^ tbl[((crc >> 24) ^ size) & 0xFF];
	return ~crc;
}
