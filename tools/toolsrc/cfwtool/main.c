// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
/*
		3DS Custom Firmware Tool - Roxas75
		This is meant to be an utility to make a simple patch that
		allows cfw generation.
		Working on NATIVE_FIRM v18182 (9.3)
*/

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#define MAX_PATCHES 100

char patch_name[MAX_PATCHES][256];
int npatch = 0;

unsigned int FirmAddr(unsigned int vaddr){
	//Taken from NATIVE_FIRM v18182
	int arm9_addr = 0x66000, arm9_size = 0x84A00;
	int arm11_addr = 0x35000, arm11_size = 0x31000;

	if(vaddr >= 0x08006800 && vaddr <= 0x08006800 + 0x15B00){  					//Kernel9 Area
		return arm9_addr + (vaddr - 0x08006800);
	}
	if(vaddr >= 0x08028000 && vaddr <= 0x08028000 + arm9_size - 0x16700){  		//Process9 area
		return arm9_addr + (vaddr - 0x08028000) + 0x16700;
	}
	if(vaddr >= 0x1FF80000 && vaddr <= 0x1FF80000 + arm11_size){  				//Kernel11 Area
		return arm11_addr + (vaddr - 0x1FF80000);
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
	if(argc < 3){
		printf("Usage : patchfirm.exe <patches_folder_path> <patch_path> <-v = verbose>\n");
		exit(-1);
	}
	if(argc > 3)
		if(!strcmp(argv[3], "-v")) print = 1;
	listfiles(argv[1]);
	FILE* patch = fopen(argv[2], "wb");
	if(!patch){
		printf("No patch path specified!\n");
		exit(-1);
	}

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
			unsigned int pos = FirmAddr(off);
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
