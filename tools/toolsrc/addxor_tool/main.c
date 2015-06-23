#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv){
	if(argc != 5) return -1;
	srand(time(NULL));
	FILE* pf = fopen(argv[1], "rb+");
	if(!pf) return -1;
	FILE* out = fopen(argv[2], "wb");

	unsigned int base;
	if(argv[3][0] == '0' && (argv[3][1] == 'x' || argv[3][1] == 'X'))
		base = strtol(argv[3] + 2, NULL, 16);
	else
		base = atoi(argv[3]);

	unsigned int seed;
	if(argv[4][0] == '0' && (argv[4][1] == 'x' || argv[4][1] == 'X'))
		seed = strtol(argv[4] + 2, NULL, 16);
	else
		seed = atoi(argv[4]);
	printf("Base : %08X\nSeed : %08X\n", base, seed);

	while(!feof(pf)){
		unsigned int cur = ftell(pf);
		unsigned int word;
		fread(&word, 1, 4, pf);
		word ^= base;
		base += seed;
		fwrite(&word, 1, 4, out);
	}

	fclose(pf);
	return 0;
}
