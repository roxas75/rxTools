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

#include <errno.h>
#include <stdio.h>
#include <malloc.h>

#ifdef NON_UNICODE

#define FONT_WIDTH	8
#define FONT_HEIGHT	8
#define CHAR_COLUMNS	16
#define CHAR_ROWS	16

#else

#define FONT_WIDTH	16
#define FONT_HEIGHT	16
#define CHAR_COLUMNS	256
#define CHAR_ROWS	256

#endif

#if FONT_WIDTH <= 8
#define FONT_TYPE unsigned char
#elif FONT_WIDTH <= 16
#define FONT_TYPE unsigned short
#endif

int main(int argc, char** argv)
{
	FONT_TYPE (* font)[CHAR_COLUMNS][FONT_HEIGHT];
	FILE *fp;
	unsigned int i, j, k, l;
	FONT_TYPE m;

	if (argc != 3) {
		fprintf(stderr, "Usage : %s <font(%dx%dx1bpp).bin> <font(%dx%dx1bpp).bin>\n\n"
			"This program is free software; you can redistribute it and/or\n"
			"modify it under the terms of the GNU General Public License\n"
			"version 2 as published by the Free Software Foundation\n",
			argv[0],
			FONT_WIDTH * CHAR_COLUMNS, FONT_HEIGHT * CHAR_ROWS,
			FONT_WIDTH, CHAR_COLUMNS * FONT_HEIGHT * CHAR_ROWS);

		return -1;
	}

	font = malloc(CHAR_ROWS * CHAR_COLUMNS * FONT_HEIGHT * sizeof(FONT_TYPE));
	if (font == NULL) {
		perror(NULL);
		return errno;
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		perror(argv[1]);
		goto fail;
	}

	for (i = 0; i < CHAR_ROWS; i++)
		for (j = 0; j < FONT_HEIGHT; j++)
			for (k = 0; k < CHAR_COLUMNS; k++)
				if (fread(&font[i][k][j], sizeof(FONT_TYPE), 1, fp) != 1)
					perror(argv[1]);

	if (fclose(fp))
		perror(argv[1]);

	fp = fopen(argv[2], "wb");
	if (fp == NULL) {
		perror(argv[2]);
		goto fail;
	}

	for (i = 0; i < CHAR_ROWS; i++)
		for (j = 0; j < CHAR_COLUMNS; j++) {
			k = FONT_WIDTH;
			while (k > 0) {
				k--;

				m = 0;
				for (l = 0; l < FONT_WIDTH; l++)
					m |= (((font[i][j][l] >> ((k + 8) & (FONT_WIDTH - 1))) & 1) << ((FONT_WIDTH - 1) - l));
//					m|=(((font[i+l] >> ((k+8)&(FONT_WIDTH-1))) & 1) << ((FONT_WIDTH-1-l+8)&(FONT_WIDTH-1)));

				if (fwrite(&m, sizeof(m), 1, fp) != 1) {
					perror(argv[2]);

					if (fclose(fp))
						perror(argv[2]);

					goto fail;
				}
			}
		}

	if (fclose(fp)) {
		perror(argv[2]);
		goto fail;
	}

	free(font);
	return 0;

fail:
	free(font);
	return errno;
}
