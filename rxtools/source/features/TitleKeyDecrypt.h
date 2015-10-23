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

#ifndef TITLE_KEY_DECRYPT_H
#define TITLE_KEY_DECRYPT_H

#include <stdint.h>

typedef struct {
	uint8_t issuer[64];
	uint8_t pubKey[60];
	uint8_t ver;
	uint8_t caCrlVer;
	uint8_t issuerCrlVer;
	uint8_t titleKey[16];
	uint8_t unused0;
	uint8_t ticketId[8];
	uint8_t consoleId[4];
	uint8_t titleId[8];
	uint8_t unused1[2];
	uint16_t titleVer;
	uint8_t unused2[8];
	uint8_t type;
	uint8_t keyIndex;
	uint8_t unused3[42];
	uint8_t unk[4];
	uint8_t unused4;
	uint8_t audit;
	uint8_t unused6[66];
	uint8_t limits[64];
	uint8_t contentIndex[172];
} TicketHdr;

void DecryptTitleKeys();
void DecryptTitleKeyFile(void);
int getTitleKey(uint8_t *TitleKey, uint32_t low, uint32_t high, int drive);
int getTitleKeyWithCetk(uint8_t dst[16], const char *path);

#endif