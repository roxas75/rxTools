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

#include "common.h"

void DecryptTitleKeys();
void DecryptTitleKeyFile(void);
u32 DecryptTitleKey(u8 *titleid, u8 *key, u32 index);
int GetTitleKey(u8 *TitleKey, u32 low, u32 high, int drive);

#endif
