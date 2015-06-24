#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"
#include "filepack.h"

int current_y = 1;

unsigned char* tmpscreen = 0x26000000;

void ClearScreen(unsigned char *screen, int color)
{
    int i;
//    unsigned char *screenPos = screen;
    color = color >> 16 & 0xFF | color & 0xFF00 | color << 16 & 0xFF0000;
    for (i = 0; i < SCREEN_SIZE; i += 4)
    {
//        *(screenPos++) = color >> 16; //B
//        *(screenPos++) = color >> 8; //G
//        *(screenPos++) = color & 0xFF; //R
        color |= color << 24;
        *(unsigned *)(screen + i) = color;
        color >>= 8;
    }

    //memset(screen,color,SCREEN_SIZE);
    //memset(screen + SCREEN_SIZE + 16,color,SCREEN_SIZE);
}

void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor)
{
    int yy, xx;
    int xDisplacement = x * SCREEN_HEIGHT;
    int yDisplacement = SCREEN_HEIGHT - y - 1;
    unsigned char *screenPos = screen + (xDisplacement + yDisplacement) * BYTES_PER_PIXEL;
    for (yy = 0; yy < 8; yy++)
    {
        unsigned char charPos = font[character * 8 + yy];
        for (xx = 7; xx >= 0; xx--)
        {
            if ((charPos >> xx) & 1)
            {
				if(color != TRANSPARENT){
					*(screenPos++) = color >> 16; //B
					*(screenPos++) = color >> 8; //G
					*(screenPos++) = color & 0xFF; //R
				}
            }
            else
            {
				if(bgcolor != TRANSPARENT){
					*(screenPos++) = bgcolor >> 16; //B
					*(screenPos++) = bgcolor >> 8; //G
					*(screenPos++) = bgcolor & 0xFF; //R
				}
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT - 3;
        }
        screenPos -= BYTES_PER_PIXEL * (SCREEN_HEIGHT * 8 + 1);
    }
    if(screen = TOP_SCREEN && TOP_SCREEN2){
        screen = TOP_SCREEN2;
        int xDisplacement = x * SCREEN_HEIGHT;
        int yDisplacement = SCREEN_HEIGHT - y - 1;
        unsigned char *screenPos = screen + (xDisplacement + yDisplacement) * BYTES_PER_PIXEL;
        for (yy = 0; yy < 8; yy++)
        {
            unsigned char charPos = font[character * 8 + yy];
            for (xx = 7; xx >= 0; xx--)
            {
                if ((charPos >> xx) & 1)
                {
    				if(color != TRANSPARENT){
    					*(screenPos++) = color >> 16; //B
    					*(screenPos++) = color >> 8; //G
    					*(screenPos++) = color & 0xFF; //R
    				}
                }
                else
                {
    				if(bgcolor != TRANSPARENT){
    					*(screenPos++) = bgcolor >> 16; //B
    					*(screenPos++) = bgcolor >> 8; //G
    					*(screenPos++) = bgcolor & 0xFF; //R
    				}
                }
                screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT - 3;
            }
            screenPos -= BYTES_PER_PIXEL * (SCREEN_HEIGHT * 8 + 1);
        }
    }
}

void DrawString(unsigned char *screen, const char *str, int x, int y, int color, int bgcolor)
{
    int i;
    for (i = 0; i < strlen(str); i++)
    {
        DrawCharacter(screen, str[i], x + i * 8, y, color, bgcolor);
    }
}

void DrawHex(unsigned char *screen, unsigned int hex, int x, int y, int color, int bgcolor)
{
    int i;
    for(i=0; i<8; i++)
    {
        int character = '-';
        int nibble = (hex >> ((7-i)*4))&0xF;
        if(nibble > 9) character = 'A' + nibble-10;
        else character = '0' + nibble;

        DrawCharacter(screen, character, x+(i*8), y, color, bgcolor);
    }
}

void DrawHexWithName(unsigned char *screen, const char *str, unsigned int hex, int x, int y, int color, int bgcolor)
{
    DrawString(screen, str, x, y, color, bgcolor);
    DrawHex(screen, hex,x + strlen(str) * 8, y, color, bgcolor);
}

void Debug(const char *format, ...)
{
    char* str;
    va_list va;

    va_start(va, format);
    vasprintf(&str, format, va);
    va_end(va);

    DrawString(TOP_SCREEN, str, 10, current_y, RGB(255, 255, 255), RGB(0, 0, 0));
    free(str);

    current_y += 10;
}

void writeByte(int address, u8 value) {
	char *addr = (u8*)address;

	*addr = value;
}

u8 readByte(int address) {
	char *addr = (u8*)address;

	return *addr;
}

void DrawPixel(int x, int y, int color, int screen){
	if(x >= 400 || x < 0) return;
	if(y >= 240 || y < 0) return;
	if(color != TRANSPARENT){
		int cord = 720 * x + 720 -(y * 3);
		int address  = cord + screen;
		writeByte(address, color >> 16);
		writeByte(address+1, color >> 8);
		writeByte(address+2, color & 0xFF);
	}
    if(screen == TOP_SCREEN && TOP_SCREEN2){
        screen = TOP_SCREEN2;
        if(color != TRANSPARENT){
    		int cord = 720 * x + 720 -(y * 3);
    		int address  = cord + screen;
    		writeByte(address, color >> 16);
    		writeByte(address+1, color >> 8);
    		writeByte(address+2, color & 0xFF);
    	}
    }
}

int GetPixel(int x, int y, int screen){
	int cord = 720 * x + 720 -(y * 3);
	int address  = cord + screen;
	int color = 0;
	color |= readByte(address+0)<<0;
	color |= readByte(address+1)<<8;
	color |= readByte(address+2)<<16;
	return color;
}


//----------------Some of my shit..........
void SplashScreen(){
	u8* Top = GetFilePack(TOP_PIC);
    for(int i = 0; i < 0x46500; i+=3){
		*(TOP_SCREEN + i + 0) = Top[i];
		*(TOP_SCREEN + i + 1) = Top[i + 1];
		*(TOP_SCREEN + i + 2) = Top[i + 2];
        if(TOP_SCREEN2){
            *(TOP_SCREEN2 + i + 0) = Top[i];
    		*(TOP_SCREEN2 + i + 1) = Top[i + 1];
    		*(TOP_SCREEN2 + i + 2) = Top[i + 2];
        }
	}
}
