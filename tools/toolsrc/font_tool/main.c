#include <stdio.h>
#define fontsize 256 * 8
int main(int argc, char** argv){
	if(argc < 3){
		printf("Usage : font_tool.exe <font(128x128x1bpp).png> <font.bin>\n");
		return -1;
	}
	unsigned char font[fontsize];
	FILE * pFile;	
	int i,j,k;
	pFile=fopen(argv[1],"rb");
	fseek(pFile,0x43,SEEK_SET);
	for(k=0; k<16; k++){
		for(j=0; j<8; j++){
			for(i=0; i<16; i++){
				fread(&font[i*8+j+k*8*16],1,1,pFile);
			}
			fseek(pFile,1,SEEK_CUR);
		}
	}
	fclose(pFile);
	pFile=fopen(argv[2],"wb");
	for(i=0; i<fontsize; i+=8){
		for(j=7; j>=0; j--){
			k=(((font[i+0] >> j) & 1) << 7) |
				(((font[i+1] >> j) & 1) << 6) |
				(((font[i+2] >> j) & 1) << 5) |
				(((font[i+3] >> j) & 1) << 4) |
				(((font[i+4] >> j) & 1) << 3) |
				(((font[i+5] >> j) & 1) << 2) |
				(((font[i+6] >> j) & 1) << 1) |
				(((font[i+7] >> j) & 1) << 0);
			fwrite(&k,1,1,pFile);
		}
	}
	fclose(pFile);
	return 0;
}
