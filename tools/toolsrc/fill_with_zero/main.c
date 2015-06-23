#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

int main(int argc, char** argv){
	if(argc != 3) return -1;
	srand(time(NULL));
	FILE* pf = fopen(argv[1], "rb+");
	unsigned int size = atoi(argv[2]);
	printf("%d bytes\n", size);
	fseek(pf, 0, 2);
	while(ftell(pf) < size)
		fputc(0, pf);
	fclose(pf);
	return 0;
}