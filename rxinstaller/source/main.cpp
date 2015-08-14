#include <nds.h>
#include <stdio.h>
#include <fat.h>
#include <vector>
#include <string>
#include "patches.h"
#include "drunkenlogo.h"

unsigned int rawDataOffset=0;

u8 workbuffer[1024] ALIGN(32);

#define SCREEN_COLS 32
#define ITEMS_PER_SCREEN 10
#define ITEMS_START_ROW 12

using namespace std;

struct patchEntry {
	string description;
	u32 fileoffset;
};

//---------------------------------------------------------------------------------
void halt() {
//---------------------------------------------------------------------------------
	int pressed;

	iprintf("      Press A to exit\n");

	while(1) {
		swiWaitForVBlank();
		scanKeys();
		pressed = keysDown();
		if (pressed & KEY_A) break;
	}
	exit(0);
}

//---------------------------------------------------------------------------------
void userSettingsCRC(void *buffer) {
//---------------------------------------------------------------------------------
	u16 *slot = (u16*)buffer;
	u16 CRC1 = swiCRC16(0xFFFF, slot, 0x70);
	u16 CRC2 = swiCRC16(0xFFFF, &slot[0x3a], 0x8A);
	slot[0x39] = CRC1; slot[0x7f] = CRC2;
}

/*
//---------------------------------------------------------------------------------
void saveFile(char *name, void *buffer, int size) {
//---------------------------------------------------------------------------------
	FILE *out = fopen(name,"wb");
	if (out) {
		fwrite(buffer, 1, 1024, out);
		fclose(out);
	} else {
		printf("couldn't open %s for writing\n",name);
	}
}
*/

//---------------------------------------------------------------------------------
void showPatchList (const vector<patchEntry>& patchList, int startRow) {
//---------------------------------------------------------------------------------

	for (int i = 0; i < ((int)patchList.size() - startRow) && i < ITEMS_PER_SCREEN; i++) {
		const patchEntry* patch = &patchList.at(i + startRow);

		// Set row
		iprintf ("\x1b[%d;6H", i + ITEMS_START_ROW);
		iprintf ("%s", patch->description.c_str());
	}
}


void aread( void * ptr, size_t size, size_t count, int stream ){  // 'array read' drop in substitute for fread

	size=0;
	stream=0;
	memcpy(ptr,&rawData[rawDataOffset],count);
	rawDataOffset+=count;
}

int aseek( int stream, int offset, int origin ){                  // 'array seek' drop in substitute for fseek

	origin=0;
	stream=0;
	rawDataOffset=offset;
	return 0;
}

int aeof(int stream){

	stream=0;
	int result=0;
	if(rawDataOffset >= fSIZE)result=1;
	return result;
}

int atell(int stream){
	return rawDataOffset;
}

char * agets( char * str, int num, int stream ){

	stream=0;
	memcpy(str,&rawData[rawDataOffset],num);
	return str;
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------
	static const char path[] = "YS:/" CODE_PATH;
	int i;

	videoSetMode(MODE_5_2D);                         //shamlessly ripped from the libnds examples :p
	videoSetModeSub(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_BG);
	bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0,0);
	decompress(drunkenlogoBitmap, BG_GFX,  LZ77Vram);

	consoleDemoInit();

	iprintf("\n\n\n\n\n\n");
	iprintf("   >> Cakes ROP Installer <<  \n");
	iprintf("   >> rxTools Edition <<  \n");
	iprintf("\n\n");

	int patchfile = 0;
	int header;
	rawDataOffset=0;
    char custom[6][35];

	aread(&header,1,4,patchfile);

	if ( header != 'ROPP') {
		iprintf("      Invalid patch file!\n");
		halt();
	}

	int index_offset;
	aread(&index_offset,1,4,patchfile);

	aseek(patchfile,index_offset,SEEK_SET);

	vector<patchEntry> patches;
	patchEntry patch;

	while(1) {
		patch.description.clear();
		int string_offset;

		aread(&string_offset,1,4,patchfile);
		if (aeof(patchfile)) break;

		aread(&patch.fileoffset,1,4,patchfile);

		// save file pointer
		int file_ptr = atell(patchfile);

		aseek(patchfile,string_offset,SEEK_SET);

		char description[21];

		char *desc = agets(description, 20, patchfile);

		if (desc == NULL ) {
			iprintf("      Failed reading description\n");
			halt();
		}

		// terminate string
		description[20] = 0;
		patch.description = description;
		patches.push_back(patch);

		// restore file pointer for next offset
		aseek(patchfile,file_ptr,SEEK_SET);
	}

	int pressed,fwSelected=0,screenOffset=0;
	// Default + custom
    int patch_count = patches.size() * 2;

	showPatchList(patches, fwSelected);

	// Custom filenames
	for(unsigned int i = 0; i < patches.size(); i++) {
		const patchEntry* patch = &patches.at(i);
		iprintf ("\x1b[%d;6H", patches.size() + i + ITEMS_START_ROW);
		iprintf ("%.7sropCustom.txt", patch->description.c_str());
	}

	while(1) {

		// Show cursor
		iprintf ("\x1b[%d;3H[>\x1b[21C<]", fwSelected - screenOffset + ITEMS_START_ROW);

		// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
		do {
			scanKeys();
			pressed = keysDownRepeat();
			swiWaitForVBlank();
		} while (!pressed);

		// Hide cursor
		iprintf ("\x1b[%d;3H  \x1b[21C  ", fwSelected - screenOffset + ITEMS_START_ROW);
		if (pressed & KEY_UP) 		fwSelected -= 1;
		if (pressed & KEY_DOWN) 	fwSelected += 1;

		if (pressed & KEY_A) break;

		if (fwSelected < 0) 	fwSelected = patch_count - 1;		// Wrap around to bottom of list
		if (fwSelected > (patch_count - 1))		fwSelected = 0;		// Wrap around to top of list


	}

	iprintf ("\x1b[5;0H\x1b[J");


    const patchEntry *selectedPatch;
    if (fwSelected < (int)patches.size()) {
	    selectedPatch = &patches.at(fwSelected);
    } else {
        selectedPatch = &patches.at(fwSelected - (patch_count - patches.size()));
    }

	iprintf("Patching for %s\n\n",selectedPatch->description.c_str());

	// read header
	readFirmware(0,workbuffer,42);

	u32 userSettingsOffset = (workbuffer[32] + (workbuffer[33] << 8))<<3;

	// read User Settings
	readFirmware(userSettingsOffset,workbuffer,512);


	aseek(patchfile,selectedPatch->fileoffset,SEEK_SET);
	aread(&header,1,4,patchfile);

	if (header != 'PTCH' ) {

			printf("      Patch set invalid\n");
			halt();

	} else {

		int32_t numPatches;
		uint32_t patchSize,patchOffset;
		aread(&numPatches,1,4,patchfile);

		uint32_t patchOffsetList[numPatches];
		aread(patchOffsetList,1,sizeof(patchOffsetList),patchfile);

		for (int i=0; i<numPatches; i++) {

			aseek(patchfile,patchOffsetList[i],SEEK_SET);
			aread(&patchSize,1,4,patchfile);
			aread(&patchOffset,1,4,patchfile);
			aread(&workbuffer[patchOffset],1,patchSize,patchfile);

		}
	}

    if (fwSelected < (int)patches.size()) {
        i = 0;
        do {
            *(workbuffer+0x11C+i*2)=path[i];
            *(workbuffer+0x11C+i*2+1)=0;
        } while (path[i++] != 0);
    } else {
        // Load filename from ropCustom.txt
        int csSelected=0;  //custom selected
		int customLinesTotal=6;
		int i=0;
		char pathBegin[]="YS:/";


		if(!fatInitDefault()){
			iprintf("      fat init error\n");
			halt();
		}

		FILE *text=fopen("ropCustom.txt","r");

		if(!text){
			iprintf("      ropCustom.txt load error\n      Does it exist?\n\n");
			halt();
		}

		for(i=0;i<customLinesTotal;i++){
			fgets(custom[i],30,text);
			if(!custom[i] || (custom[i][0] < 0x21) )break;

			for(int j=0; j < 30 ;j++){  //strip newline junk
				if(custom[i][j]==0x0D || custom[i][j]==0x0A)
					custom[i][j]=0;
			}
			custom[i][25]=0;    //make damn sure it terminates
		}
		if(i==0){
			iprintf("      ropCustom.txt read error\n");
			iprintf("      invalid first line path\n");
			halt();
		}

		customLinesTotal=i;

		fclose(text);

		iprintf("\x1b[2J"); //clear console
		iprintf("Custom filename list:");

		for(i=0;i<customLinesTotal;i++){
			iprintf ("\x1b[%d;3H", i + ITEMS_START_ROW -8);
			iprintf ("%s", custom[i]);
		}


		while(1) {

			// Show cursor
			iprintf ("\x1b[%d;0H[>", csSelected - screenOffset + ITEMS_START_ROW -8);

			// Power saving loop. Only poll the keys once per frame and sleep the CPU if there is nothing else to do
			do {
				scanKeys();
				pressed = keysDownRepeat();
				swiWaitForVBlank();
			} while (!pressed);

			// Hide cursor
			iprintf ("\x1b[%d;0H  ", csSelected - screenOffset + ITEMS_START_ROW -8);
			if (pressed & KEY_UP) 		csSelected -= 1;
			if (pressed & KEY_DOWN) 	csSelected += 1;

			if (pressed & KEY_A) break;

			if (csSelected < 0) 	csSelected =  customLinesTotal - 1;		// Wrap around to bottom of list
			if (csSelected > (customLinesTotal - 1))		csSelected = 0;		// Wrap around to top of list


		}

		for(int i=0; i < 4*2 ;i+=2){
			*(workbuffer+0x11C+i)=pathBegin[i/2];
			*(workbuffer+0x11C+i+1)=0;
		}

		for(int i=0; i < 32 ;i+=2){
			*(workbuffer+0x124+i)=custom[csSelected][i/2];
			*(workbuffer+0x124+i+1)=0;
		}

		*(workbuffer+0x142)=0; //ensure terminated string (again)
		*(workbuffer+0x143)=0;
    }

	userSettingsCRC(workbuffer);
	userSettingsCRC(workbuffer+256);


	iprintf("\n\n\n\n\n      Writing ... ");
	int ret = writeFirmware(userSettingsOffset,workbuffer,512);

	if (ret) {

		iprintf("failed\n");

	} else {

		iprintf("success\n");

	}

	iprintf("      Verifying ... ");
	readFirmware(userSettingsOffset,workbuffer+512,512);

	if (memcmp(workbuffer,workbuffer+512,512)){

		iprintf("failed\n");

	} else {

		iprintf("success\n");

	}

	halt();
	return 0;
}
