/*
  types.h _ Various system types.
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
