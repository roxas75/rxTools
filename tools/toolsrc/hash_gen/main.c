#include <stdio.h>
#include <malloc.h>

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

int main(int argc, char** argv){
    if(argc < 2) return -1;
    FILE* in = fopen(argv[1], "rb");
    fseek(in, 0, 2); unsigned int fsz = ftell(in); rewind(in);
    unsigned char* buf = (unsigned char*)malloc(fsz);
    fread(buf, 1, fsz, in);
    printf("Hash : %08X\n", HashGen(buf, fsz));
    fclose(in);
    free(buf);
    getchar();

    return 0;
}
