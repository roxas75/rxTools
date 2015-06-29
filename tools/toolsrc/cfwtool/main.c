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
#include <dirent.h>
#include <unistd.h>
#define MAX_PATCHES 100

typedef struct{
	char* name;
	int arm9_addr;
	int arm9_size;
	int arm9_entry;
	int arm11_addr;
	int arm11_size;
	int arm11_entry;
	int p9_addr;
	int p9_start;
	int p9_entry;
} firm_info;

firm_info native_info = { "NATIVE_FIRM", 0x66000, 0x84A00, 0x08006800, 0x35000, 0x31000, 0x1FF80000, 0x15B00, 0x16700, 0x08028000};
firm_info agb_info = { "AGB_FIRM", 0x8B800, 0x4CE00, 0x08006800, 0, 0, 0, 0xD600, 0xE200, 0x08020000};
firm_info twl_info = { "TWL_FIRM", 0x153600, 0x4D200, 0x08006800, 0, 0, 0, 0xD600, 0xE200, 0x08020000};

char patch_name[MAX_PATCHES][256];
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
			if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
				strcpy(patch_name[npatch++], ent->d_name);
				//printf("%s\n", ent->d_name);
		}
		closedir (dir);
		printf("Patches folder : %s\nNumber of patches : %d\n", curDir, npatch);
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
		unsigned int off = 0, size = 0;
		sscanf(patch_name[i], "%08x.bin", &off);
		sprintf(str, "%s/%s", argv[1], patch_name[i]);
		FILE* fp = fopen(str, "rb");
		fseek(fp, 0, 2); size = ftell(fp); rewind(fp);
		if(size > 0){
			unsigned char* buf = (unsigned char*) malloc (size);
			fread(buf, 1, size, fp); fclose(fp);
			unsigned int pos = FirmAddr(off, info);
			fwrite(&pos, 1, 4, patch);
			fwrite(&size, 1, 4, patch);
			fwrite(buf, 1, size, patch);
			while((ftell(patch) % 4) != 0) fputc(0, patch);		//Aligned 4
			if(print) printf("Addr : %08X		Size : %d\n", off, size);
			free(buf);
		} else {
		    fclose(fp);
        }
	}
    fclose(patch);

	return 0;
}
