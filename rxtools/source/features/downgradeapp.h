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

#ifndef DOWNGRADEAPP_H
#define DOWNGRADEAPP_H

#include <stdint.h>

typedef struct {
        unsigned int drive;
        uint32_t tidLo;
        uint32_t tidHi;
        char tmd[64];
        char content[64];
} AppInfo;

//Utilities
// Fill drive, tidLo and tidHi before calling this.
int FindApp(AppInfo *info);
int CheckRegion(int drive);

//Features
void downgradeMSET();
void installFBI();
void restoreHS();

#endif
