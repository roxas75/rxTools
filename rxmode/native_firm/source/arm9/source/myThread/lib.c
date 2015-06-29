// ---------------------------------------- //
// |      Copyright(c) 2015, Roxas75      | //
// |         All rights reserved.         | //
// ---------------------------------------- //
#include "lib.h"

void *rx_memset(void *ptr, u8 value, u32 num) {
	u8 *p = ptr;
	while (num) {
		*p++ = value;
		num--;
	}
	return ptr;
}

int rx_strcmp(char *s1, char *s2, u32 size, u32 w1, u32 w2) {
	for (int i = 0; i < size; i++) {
		if (s1[i * w1] != s2[i * w2]) { return 0; }
	}
	return 1;
}

void rx_strcpy(char *dest, char *source, u32 size, u32 w1, u32 w2) {
	for (int i = 0; i < size; i++) {
		dest[i * w1] = source[i * w2];
	}
}

void rx_hextostr(u32 num, void *str) {
	int i;
	char *ptr = str + 8;
	for (i = 0; i < 8; i++) {
		int x = (num & 0xF);
		char y = '0';
		if (x > 0x9) { y += 7; }
		*--ptr = y + x;
		num >>= 4;
	}
}

int rx_memcmp(void *buf1, void *buf2, int size) {
	int equal = 0;
	for (int i = 0; i < size; i++) {
		if (*((u8 *)buf1 + i) != *((u8 *)buf2 + i)) {
			equal = i;
			break;
		}
	}
	return equal;
}

/** Search the memory for target buffer.
  * @param tb The buffer of your target.
  * @param sb The memory where to look in.
  * @param tl Length of tb.
  * @param sl Length of sb.
  * @retval   Where it is found in buffer.
  */
void* rx_memmem(const void* tb, const void* sb, u32 tl, u32 sl) {
	char *ss = (char*)sb; //Backed up points from comparison, for source.
	char *tp = (char*)tb, *sp = (char*)sb; //Current pointer in both buffers.
	u32 lm = 0, lt = sl; //the length of matched part. And the total length
	//T==S?	Y:T,S:Forward.L++.L==l?
	//			Y:Sucuess, return.
	//		N:T,S:Unwind.S:Forward.L=0.
	while (lt --) {
		if (*tp == *sp) {
			tp++; sp++; lm++;
			if (lm == tl) return ss;
		} else {
			lt += lm;
			ss++;
			tp = (char*)tb; sp = ss;
			lm = 0;
		}
	}
	return 0;
}

u32 getHID() {
	return ~*(u32 *)0x10146000;
}
