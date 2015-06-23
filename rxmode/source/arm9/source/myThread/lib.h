// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
#define FCRAM (unsigned char*)0x20000000
#define VRAM (unsigned char*)0x18000000

//The basic functions we will need
void* memset(void * ptr, int value, unsigned int num);
int rx_strcmp(char* s1, char*s2, unsigned int size, unsigned int w1, unsigned int w2);    // w1 = size of string1 letters
void rx_strcpy(char* dest, char* source, unsigned int size, unsigned int w1, unsigned int w2);
void rx_hextostr(unsigned int num, char* str);
int memcmp(void* buf1, void* buf2, int size);
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

unsigned int getHID();
