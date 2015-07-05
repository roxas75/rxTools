//This is actually some basic code, i do not expect anyone to like it, i'm the first to say that's messed up.
//The fact there is that rxtools doesn't really need any specific console code for now, so i've written something simple.
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "common.h"
#include "console.h"
#include "draw.h"
#include "menu.h"

#define CONSOLE_SIZE 0x4000
#define MAXLINES 10

char console[CONSOLE_SIZE];
char consoletitle[100] = "Dummy Title";
char str[100];

int BackgroundColor = WHITE;
int TextColor = BLACK;
int BorderColor = BLUE;
int SpecialColor = RED;
int Spacing = 2;
unsigned int ConsoleX = CONSOLE_X, ConsoleY = CONSOLE_Y, ConsoleW = CONSOLE_WIDTH, ConsoleH = CONSOLE_HEIGHT;
unsigned int BorderWidth = 1;
unsigned int cursor = 0;
unsigned int linecursor = 0;
unsigned int consoleInited = 0;

void ConsoleInit(){
	consoleInited = 1;
	ConsoleFlush();
}

void ConsoleSetXY(int x, int y){
	ConsoleX = x;
	ConsoleY = y;
}

void ConsoleGetXY(int *x, int *y){
	*x = ConsoleX;
	*y = ConsoleY;
}

void ConsoleSetWH(int width, int height){
	ConsoleW = width;
	ConsoleH = height;
}

void ConsoleSetBorderWidth(int width){
	BorderWidth = width;
}

int ConsoleGetBorderWidth(int width){
	return BorderWidth;
}

void ConsoleSetTitle(const char* format, ...){
	char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);
	strncpy(consoletitle, str, 100);
}

int countLines(){
	int cont = 0;
	for(int i = 0; i < CONSOLE_SIZE; i++){
		if(console[i] == '\n'); cont++;
	}
	return cont;
}

int findCursorLine(){
	int cont = 0;
	for(int i = 0; i < cursor; i++){
		if(console[i] == '\n'); cont++;
	}
	return cont;
}
void ConsoleShow(){
	
	sprintf(str, "/rxTools/Theme/%c/app.bin", Theme);
	DrawBottomSplash(str);

    void *tmpscreen = (void*)0x27000000;
	memcpy(tmpscreen, BOT_SCREEN, 0x38400);
	if(!consoleInited) return;
	int titley = 2*CHAR_WIDTH;

	//for(int y = ConsoleY; y < ConsoleH + ConsoleY + BorderWidth; y++){
	//	for(int x = ConsoleX; x < ConsoleW + ConsoleX + BorderWidth; x++){
	//		if(//(x >= ConsoleX && x <= ConsoleX + BorderWidth) || 
	//		   //(x >= ConsoleW + ConsoleX - 1 && x <= ConsoleW + ConsoleX - 1 + BorderWidth) || 
	//		   (y >= ConsoleY && y <= ConsoleY + BorderWidth) || 
	//		   (y >= ConsoleH + ConsoleY - 1 && y <= ConsoleH + ConsoleY - 1 + BorderWidth) ||
	//		   (y >= ConsoleY + titley - BorderWidth && y <= ConsoleY + titley)){
	//			DrawPixel(x, y, BorderColor, (int)tmpscreen);
	//		}else{
	//			DrawPixel(x, y, BackgroundColor, (int)tmpscreen);
	//		}
	//	}
	//}
	int titlespace = 2*CHAR_WIDTH-2*BorderWidth;
	DrawString(tmpscreen, consoletitle, ConsoleX + BorderWidth + 2 * CHAR_WIDTH, ConsoleY + (titlespace - CHAR_WIDTH) / 2 + BorderWidth, TextColor, 0x00000000);
	
	char tmp[256], *point;
        if(findCursorLine() < MAXLINES) point = &console[0];
	else{
		int cont = 0;
		int tmp1;
		for(tmp1 = cursor; tmp1 >= 0 && cont <= MAXLINES + 1; tmp1--){
			if(console[tmp1] == '\n') cont++;
		}
		while(console[tmp1] != 0x00 && console[tmp1] != '\n') tmp1--;
		point = &console[tmp1+1];
	}
	int lines = 0;
	for(int i = 0; i < CONSOLE_SIZE; i++){
		int linelen = 0;
		memset(tmp, 0, 256);
		while(1){
			if(*point == 0x00)  break;
			if(*point == '\n'){ point++; break; }
			tmp[linelen++] = *point++;
		}
		DrawString(tmpscreen, tmp, ConsoleX + CHAR_WIDTH*Spacing, lines++ * CHAR_WIDTH + ConsoleY + 15 + CHAR_WIDTH*(Spacing - 1) + titley, TextColor, 0x00000000);
		if(!*point) break;
		if(lines == MAXLINES) break;
	}
	memcpy(BOT_SCREEN, tmpscreen, 0x38400);
	if (BOT_SCREEN2) memcpy(BOT_SCREEN2, tmpscreen, 0x38400);
}

void ConsoleFlush(){
	memset(console, 0, CONSOLE_SIZE);
	cursor = 0;
	linecursor = 0;
}

void ConsoleAddText(char* str){
//    int linel = ((int)((float)(ConsoleW)/(float)CHAR_WIDTH))-5;
    for(int i = 0; *str != 0x00; i++){
		if(!(*str == '\\' && *(str+1) == 'n')){	//we just handle the '\n' case, who cares of the rest
			console[cursor++] = *str++;
			linecursor++;
		}else{
			linecursor = 0;
			console[cursor++] = '\n';
			str += 2;
		}
    }
}

void print(const char *format, ...){
    char str[256];
    va_list va;

    va_start(va, format);
    vsnprintf(str, 256, format, va);
    va_end(va);
	ConsoleAddText(str);
}

void ConsoleNextLine(){
	while(console[cursor] != '\n'){
		if(console[cursor] == 0x00){
			cursor-=2;
			break;
		}
		cursor++;
	}
	cursor++;
}

void ConsolePrevLine(){ 
	if(console[cursor-1] == '\n') cursor-=2;
	while(console[cursor] != '\n'){
		if(cursor == 0){
			break;
		}
		cursor--;
	}
	if(cursor) cursor++;
	if(cursor < 0) cursor = 0;
}

void ConsoleNext(){
	if(console[cursor + 1] != 0x00) cursor++;
}

void ConsolePrev(){
	if(cursor - 1 >= 0 ) cursor--;
}

void ConsoleSetBackgroundColor(int color){
	BackgroundColor = color;
}

int ConsoleGetBackgroundColor(){
	return BackgroundColor;
}

void ConsoleSetBorderColor(int color){
	BorderColor = color;
}

int ConsoleGetBorderColor(){
	return BorderColor;
}

void ConsoleSetTextColor(int color){
	TextColor = color;
}

int ConsoleGetTextColor(){
	return TextColor;
}

void ConsoleSetSpecialColor(int color){
	SpecialColor = color;
}

int ConsoleGetSpecialColor(){
	return SpecialColor;
}

void ConsoleSetSpacing(int space){
	Spacing = space;
}

int ConsoleGetSpacing(){
	return Spacing;
}
