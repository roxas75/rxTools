/*
* types.h _ Various system types.
*
* Copyright (c) 2014,2015 fincs, plutoo, smealum, idunoe, WinterMute
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgement in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*
* Modified from ctrulib https://github.com/smealum/ctrulib/
*/
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef u32 Handle;
typedef s32 Result;
typedef void (*ThreadFunc)(void *);

struct KProcess4 {
	/* 00 */ void *vtable;
	/* 04 */ u8 padding1[0xA0 - 0x04];
	/* a0 */ u32 exheader_flags;
	/* a4 */ u8 padding2[0xA8 - 0xA4];
	/* a8 */ struct KCodeSet *code_set;
	/* ac */ u32 pid;
};

struct KProcess8 {
	/* 00 */ void *vtable;
	/* 04 */ u8 padding1[0xA8 - 0x04];
	/* a8 */ u32 exheader_flags;
	/* ac */ u8 padding2[0xB0 - 0xAC];
	/* b0 */ struct KCodeSet *code_set;
	/* b4 */ u32 pid;
};

#endif
