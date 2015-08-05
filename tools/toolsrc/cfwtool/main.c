/*
 * Copyright (C) 2015 The PASTA Team
 * Originally written by Roxas75
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/*
		3DS Custom Firmware Tool - Roxas75
		This is meant to be an utility to make a simple patch that
		allows cfw generation.
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#define MAX_PATCHES 100

typedef struct {
	char name[13];
	uint32_t off;
} file_info;

file_info files[MAX_PATCHES];
int npatch = 0;

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
	if(argc < 3){
		printf("Usage : patchfirm.exe <patches_folder_path> <patch_path> <-v = verbose>\n\n"
		       "This program is free software; you can redistribute it and/or\n"
	         "modify it under the terms of the GNU General Public License\n"
	         "version 2 as published by the Free Software Foundation\n");
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

	printf("cfwtool - generating : %s\n", argv[2]);
	fwrite(&npatch, 1, 4, patch);
	for(i = 0; i < npatch; i++){
		uint32_t size = 0;

		sprintf(str, "%s/%s", argv[1], files[i].name);
		FILE* fp = fopen(str, "rb");
		fseek(fp, 0, 2); size = ftell(fp); rewind(fp);
		if(size > 0){
			unsigned char* buf = (unsigned char*) malloc (size);
			fread(buf, 1, size, fp); fclose(fp);
			fwrite(&files[i].off, 1, sizeof(files[i].off), patch);
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
