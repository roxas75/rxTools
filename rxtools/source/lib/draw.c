#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "font.h"
#include "draw.h"
#include "filepack.h"

int current_y = 1;

u8* tmpscreen = (u8*)0x26000000;

void ClearScreen(unsigned char *screen, int color)
{
    u32 i = SCREEN_SIZE/4;  //Surely this is an interger.
    u32* tmpscr = (u32*)screen; //To avoid using array index which would decrease speed.
    //Prepared 3 u32, that includes 4 24-bits color, cached. 4x(BGR)
    u32 color0 = (color) | (color << 24),
        color1 = (color << 16) | (color >> 8),
        color2 = (color >> 16) | (color << 8);
    while (i--) {
        *(tmpscr++) = color0;
        *(tmpscr++) = color1;
        *(tmpscr++) = color2;
    }
}

void DrawCharacter(unsigned char *screen, int character, int x, int y, int color, int bgcolor)
{
    int yy, xx;
    int xDisplacement = x * SCREEN_HEIGHT;
    int yDisplacement = SCREEN_HEIGHT - y - 1;
    unsigned char *screenPos = screen + (xDisplacement + yDisplacement) * BYTES_PER_PIXEL;
    //Use cached value, yep.
    u8 foreR = color >> 16, foreG = color >> 8, foreB = color;
    u8 backR = bgcolor >> 16, backG = bgcolor >> 8, backB = bgcolor;
    for (yy = 0; yy < 8; yy++)
    {
        unsigned char charPos = font[character * 8 + yy];
        for (xx = 7; xx >= 0; xx--)
        {
            if ((charPos >> xx) & 1)
            {
				if(color != TRANSPARENT){
                    *(screenPos++) = foreB;
                    *(screenPos++) = foreG;
                    *(screenPos++) = foreR;
				}
            }
            else
            {
				if(bgcolor != TRANSPARENT){
					*(screenPos++) = backB;
                    *(screenPos++) = backG;
                    *(screenPos++) = backR;
				}
            }
            screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT - 3;
        }
        screenPos -= BYTES_PER_PIXEL * (SCREEN_HEIGHT * 8 + 1);
    }
    //Still i don't know if we should draw the text twice.
    if(screen == TOP_SCREEN && TOP_SCREEN2){
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
                        *(screenPos++) = foreB;
                        *(screenPos++) = foreG;
                        *(screenPos++) = foreR;
    				}
                }
                else
                {
    				if(bgcolor != TRANSPARENT){
    					*(screenPos++) = backB;
                        *(screenPos++) = backG;
                        *(screenPos++) = backR;
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
//[Unused]
void DrawHex(unsigned char *screen, unsigned int hex, int x, int y, int color, int bgcolor)
{
    char HexStr[4+1] = {0,}, i = 4; //4=sizeof(u32)
    while (i) {
        HexStr[(i--)-1] = 0x30 + (hex & 0xF); hex = hex >> 4;
    }
    DrawString(screen, HexStr, x+(i*8), y, color, bgcolor);
}
//[Unused]
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
//No need to enter and exit again and again, isn't it
inline void writeByte(int address, u8 value) {
	*((u8*)address) = value;
}

inline u8 readByte(int address) {
	return *((u8*)address);
}

void DrawPixel(int x, int y, int color, int screen){
	if(x >= 400 || x < 0) return;
	if(y >= 240 || y < 0) return;
	if(color != TRANSPARENT){
        int cord = 720 * x + 720 -(y * 3);
        int address  = cord + screen;
		writeByte(address, color);
		writeByte(address+1, color >> 8);
		writeByte(address+2, color >>16);
	}
    //Mind i ask why paint the pixel twice?
    //GCC: comparison between pointer and integer
    //GCC: assignment makes integer from pointer without a cast
    if(screen == TOP_SCREEN && TOP_SCREEN2){
        screen = TOP_SCREEN2;
        int cord = 720 * x + 720 -(y * 3);
        int address  = cord + screen;
        if(color != TRANSPARENT){
    		writeByte(address, color);
    		writeByte(address+1, color >> 8);
    		writeByte(address+2, color >>16);
    	}
    }
}

int GetPixel(int x, int y, int screen){
	int cord = 720 * x + 720 -(y * 3);
	int address  = cord + screen;
    return RGB(readByte(address+0),readByte(address+1),readByte(address+2));
}


//----------------Some of my shit..........
void SplashScreen(void){
	u8 *top = GetFilePack(TOP_PIC), *tmp = top;
    //Use pointer not array. dangarous but quite quicker.
    u8 *scr = TOP_SCREEN, *mir = TOP_SCREEN2;
    for(int i = 0; i < 0x46500; i+=3){
        *(scr++) = *(top++);
        *(scr++) = *(top++);
        *(scr++) = *(top++);
        if(TOP_SCREEN2){
            *(mir++) = *(tmp++);
            *(mir++) = *(tmp++);
            *(mir++) = *(tmp++);
        }
	}
}
