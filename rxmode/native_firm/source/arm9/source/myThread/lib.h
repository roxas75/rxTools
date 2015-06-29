// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //

typedef signed char s8;
typedef signed short int s16;
typedef signed int s32;
typedef signed long s64;

typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

#define FCRAM (unsigned char*)0x20000000
#define VRAM (unsigned char*)0x18000000

//The basic functions we will need
void* rx_memset(void * ptr, u8 value, u32 num);
int rx_strcmp(char *s1, char *s2, u32 size, u32 w1, u32 w2);    // w1 = size of string1 letters
void rx_strcpy(char * dest, char * source, u32 size, u32 w1, u32 w2);
void rx_hextostr(u32 num, void * str);
int rx_memcmp(void* buf1, void* buf2, int size);
static inline void svc_Backdoor(void *addr)
{
    register void *_r0 __asm ("r0") = addr;
    __asm volatile ( "SVC 0x7B" : : "r"(_r0) );
}

//hid
#define BUTTON_A      (1 << 0)
#define BUTTON_B      (1 << 1)
#define BUTTON_SELECT (1 << 2)
#define BUTTON_START  (1 << 3)
#define BUTTON_RIGHT  (1 << 4)
#define BUTTON_LEFT   (1 << 5)
#define BUTTON_UP     (1 << 6)
#define BUTTON_DOWN   (1 << 7)
#define BUTTON_R1     (1 << 8)
#define BUTTON_L1     (1 << 9)
#define BUTTON_X      (1 << 10)
#define BUTTON_Y      (1 << 11)

u32 getHID();
