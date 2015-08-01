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
#include <stdlib.h>
#ifdef __APPLE__
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
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
		unsigned int word;
		fread(&word, 1, 4, pf);
		word ^= base;
		base += seed;
		fwrite(&word, 1, 4, out);
	}

	fclose(pf);
	return 0;
}
