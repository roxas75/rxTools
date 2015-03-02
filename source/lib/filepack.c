#include "common.h"
#include "filepack.h"
#include "fs.h"
#include "draw.h"

#define FILEPACK_ADDR	0x20400000
#define FILEPACK_OFF	0x250000			//Offset in Launcher.dat
#define FILEPACK_SIZE	0x200000

int nEntry = 0;
PackEntry *Entry;    //max 100 files 
unsigned int curSize = 0;

void LoadPack(){
	File FilePack; FileOpen(&FilePack, "rxTools.dat", 0);
	curSize = FileRead(&FilePack, FILEPACK_ADDR, FILEPACK_SIZE, FILEPACK_OFF);
	FileClose(&FilePack);
	
	nEntry = *((unsigned int*)(FILEPACK_ADDR));
	Entry = ((PackEntry*)(FILEPACK_ADDR + 0x10));
	for(int i = 0; i < nEntry; i++){
		Entry[i].off += FILEPACK_ADDR;
		if(!CheckHash(Entry[i].off, Entry[i].size, Entry[i].hash)){
			DrawString(TOP_SCREEN, " rxTools.dat is corrupted!", 0, 240-8, BLACK, WHITE); 		//Who knows, if there is any corruption in our files, we ned to stop
			while(1);
		}
	}
}

unsigned char* GetFilePack(int filenumber){
	if(filenumber < nEntry)
		return (unsigned char*)Entry[filenumber].off;
	else return NULL;
}

PackEntry* GetEntryPack(int filenumber){
	if(filenumber < nEntry)
		return &Entry[filenumber];
	else return NULL;
}

int CheckHash(unsigned char* file, unsigned int size, unsigned int hash){ //that's the simplest and crappiest hash engine i could invent, but it works, so i don't give a shit
	unsigned int HASH = 0;							 
	for(int i = 0; i < size - 4; i++){
		HASH ^= file[i];
		HASH ^= file[i+1] << 8;
		HASH ^= file[i+2] << 16;
		HASH ^= file[i+3] << 24;
	}
	if(HASH == hash) return 1;
	else return 0;
}

unsigned int HashGen(unsigned char* file, unsigned int size){  
	unsigned int HASH = 0;							  
	for(int i = 0; i < size - 4; i++){
		HASH ^= file[i];
		HASH ^= file[i+1] << 8;
		HASH ^= file[i+2] << 16;
		HASH ^= file[i+3] << 24;
	}
	return HASH;
}