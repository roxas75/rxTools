/*
 * Copyright (C) 2015 The PASTA Team
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

#include <stdio.h>
#include <malloc.h>
#ifndef NON_UNICODE
	#define FONT_WIDTH	16
	#define FONT_HEIGHT	16
	#define CHAR_COLUMNS	256
	#define CHAR_ROWS	256
#else
	#define FONT_WIDTH	8
	#define FONT_HEIGHT	8
	#define CHAR_COLUMNS	16
	#define CHAR_ROWS	16
#endif
#define fontsize	CHAR_COLUMNS*CHAR_ROWS*FONT_WIDTH*FONT_HEIGHT/FONT_WIDTH
int main(int argc, char** argv){
	if(argc < 3){
		printf("Usage : font_tool.exe <font(%dx%dx1bpp).bin> <font(%dx%dx1bpp).bin>\n\n"
		       "This program is free software; you can redistribute it and/or\n"
	         "modify it under the terms of the GNU General Public License\n"
	         "version 2 as published by the Free Software Foundation\n",
					 FONT_WIDTH*CHAR_COLUMNS,FONT_HEIGHT*CHAR_ROWS,FONT_WIDTH,CHAR_COLUMNS*FONT_HEIGHT*CHAR_ROWS);
		return -1;
	}
#if FONT_WIDTH <= 8
	unsigned char *font = malloc(fontsize);
#elif FONT_WIDTH <= 16
	unsigned short *font = malloc(fontsize*2);
#endif
	FILE * pFile;
	int i,j,k,l;
	pFile=fopen(argv[1],"rb");
	for(k=0; k<CHAR_ROWS; k++){
		for(j=0; j<FONT_HEIGHT; j++){
			for(i=0; i<CHAR_COLUMNS; i++){
				fread(&font[i*FONT_HEIGHT+j+k*FONT_HEIGHT*CHAR_COLUMNS],1,sizeof(*font),pFile);
			}
		}
	}
	fclose(pFile);
	pFile=fopen(argv[2],"wb");
	for(i=0; i<fontsize; i+=FONT_WIDTH){
		for(j=FONT_WIDTH-1; j>=0; j--){
			for(l=0,k=0; k<FONT_WIDTH; k++){
//				l|=(((font[i+k] >> ((j+8)&(FONT_WIDTH-1))) & 1) << ((FONT_WIDTH-1-k+8)&(FONT_WIDTH-1)));
				l|=(((font[i+k] >> ((j+8)&(FONT_WIDTH-1))) & 1) << (FONT_WIDTH-1-k));
			}
			fwrite(&l,1,sizeof(*font),pFile);
		}
	}
	fclose(pFile);
	free(font);
	return 0;
}
