// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
/*
		3DS Custom Firmware Tool - Roxas75
		This is meant to be an utility to make a simple patch that
		allows cfw generation.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#define MAX_PATCHES 100

typedef struct {
	char name[13];
	unsigned int off;
} file_info;

typedef struct{
	char* name;
        unsigned arm9_addr;
        unsigned arm9_size;
        unsigned arm9_entry;
        unsigned arm11_addr;
        unsigned arm11_size;
        unsigned arm11_entry;
        unsigned p9_addr;
        unsigned p9_start;
        unsigned p9_entry;
} firm_info;

firm_info native_info = { "NATIVE_FIRM", 0x66000, 0x84A00, 0x08006800, 0x35000, 0x31000, 0x1FF80000, 0x15B00, 0x16700, 0x08028000};
firm_info agb_info = { "AGB_FIRM", 0x8B800, 0x4CE00, 0x08006800, 0, 0, 0, 0xD600, 0xE200, 0x08020000};
firm_info twl_info = { "TWL_FIRM", 0x153600, 0x4D200, 0x08006800, 0, 0, 0, 0xD600, 0xE200, 0x08020000};

file_info files[MAX_PATCHES];
int npatch = 0;

unsigned int FirmAddr(unsigned int vaddr, firm_info info){
	if(vaddr >= info.arm9_entry && vaddr <= info.arm9_entry + info.p9_addr){  					//Kernel9 Area
		return info.arm9_addr + (vaddr - info.arm9_entry);
	}
	if(vaddr >= info.p9_entry && vaddr <= info.p9_entry + info.arm9_size - info.p9_start){  		//Process9 area
		return info.arm9_addr + (vaddr - info.p9_entry) + info.p9_start;
	}
	if(vaddr >= info.arm11_entry && vaddr <= info.arm11_entry + info.arm11_size){  				//Kernel11 Area
		return info.arm11_addr + (vaddr - info.arm11_entry);
	}
	return 0;
}

int listfiles(char* curDir){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (curDir)) != NULL) {
		while ((ent = readdir (dir)) != NULL) {
			char ext[4];

			if (sscanf(ent->d_name, "%08x%4c", &files[npatch].off, ext) != 2)
				continue;

			if (memcmp(ext, ".bin", sizeof(ext)))
				continue;

			strcpy(files[npatch++].name, ent->d_name);
			//printf("%s\n", ent->d_name);
		}
		closedir (dir);
		printf("Patches folder : %s\nNumber of patches : %d\n", curDir, npatch);
		return 0;
	} else {
		return -1;
	}
}

int main(int argc, char** argv){
	int i;
	char str[256];
	int print = 0;
	if(argc < 4){
		printf("Usage : patchfirm.exe <patches_folder_path> <patch_path> <mode> <-v = verbose>\n");
		exit(-1);
	}
	if(argc > 4)
		if(!strcmp(argv[4], "-v")) print = 1;
	listfiles(argv[1]);
	FILE* patch = fopen(argv[2], "wb");
	if(!patch){
		printf("No patch path specified!\n");
		exit(-1);
	}
	
	firm_info info;
	switch(argv[3][0]){
		case 'n': info = native_info; break;
		case 'a': info = agb_info; break;
		case 't': info = twl_info; break;
		default : info = native_info; break;
	}
	printf("cfwtool - generating : %s\n", info.name);
	fwrite(&npatch, 1, 4, patch);
	for(i = 0; i < npatch; i++){
		unsigned int size = 0;

		sprintf(str, "%s/%s", argv[1], files[i].name);
		FILE* fp = fopen(str, "rb");
		fseek(fp, 0, 2); size = ftell(fp); rewind(fp);
		if(size > 0){
			unsigned char* buf = (unsigned char*) malloc (size);
			fread(buf, 1, size, fp); fclose(fp);
			unsigned int pos = FirmAddr(files[i].off, info);
			fwrite(&pos, 1, 4, patch);
			fwrite(&size, 1, 4, patch);
			fwrite(buf, 1, size, patch);
			while((ftell(patch) % 4) != 0) fputc(0, patch);		//Aligned 4
			if(print)
				printf("Addr : %08X		Size : %d\n",
					files[i].off, size);
			free(buf);
		} else {
		    fclose(fp);
        }
	}
    fclose(patch);

	return 0;
}
