#include "common.h"
#include "console.h"
#include "draw.h"

char console[MAX_LINES][MAX_LINE_LENGTH];
int BackgroundColor = WHITE;
int TextColor = BLACK;
int BorderColor = BLUE;
int nLines = 0;
int ConsoleX = CONSOLE_X, ConsoleY = CONSOLE_Y, ConsoleW = CONSOLE_WIDTH, ConsoleH = CONSOLE_HEIGHT;
int BorderWidth = 1;

void ConsoleInit(){
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

void ConsoleShow(){
	for(int y = ConsoleY; y < ConsoleH + ConsoleY + BorderWidth; y++){
		for(int x = ConsoleX; x < ConsoleW + ConsoleX + BorderWidth; x++){
			//if(x == ConsoleX || x == ConsoleW + ConsoleX - 1 || y == ConsoleY || y == ConsoleH + ConsoleY - 1){
			if((x >= ConsoleX && x <= ConsoleX + BorderWidth) || (x >= ConsoleW + ConsoleX - 1 && x <= ConsoleW + ConsoleX - 1 + BorderWidth) || (y >= ConsoleY && y <= ConsoleY + BorderWidth) || (y >= ConsoleH + ConsoleY - 1 && y <= ConsoleH + ConsoleY - 1 + BorderWidth)){          
				DrawPixel(x, y, BorderColor, TOP_SCREEN);
				//DrawPixel(x, y, BorderColor, TOP_SCREEN2);
			}else{
				DrawPixel(x, y, BackgroundColor, TOP_SCREEN);
				//DrawPixel(x, y, BackgroundColor, TOP_SCREEN2);
			}
		}
	}
	for(int i = 0; i < nLines; i++){
		DrawString(TOP_SCREEN, console[i], ConsoleX + CHAR_WIDTH*2, i * CHAR_WIDTH + ConsoleY + CHAR_WIDTH*2, i == 0 ? RED : TextColor, BackgroundColor);
		//DrawString(TOP_SCREEN2, console[i], ConsoleX + CHAR_WIDTH*2, i * CHAR_WIDTH + ConsoleY + CHAR_WIDTH*2, i == 0 ? RED : TextColor, BackgroundColor);
	}
}

void ConsoleFlush(){
	memset(console, 0, MAX_LINES*MAX_LINE_LENGTH);
	nLines = 0;
}

void ConsoleAddText(char* str){
	if(nLines + 1 > MAX_LINES) ConsoleFlush();
	//strncpy(console[nLines++], str, 30);
	int j = 0;
	for(int i = 0; i < strlen(str); i++){
		if(str[i] == '\n' || (str[i] == '\\' && str[i+1] == 'n') || j == MAX_LINE_LENGTH){
			while((str[i] == '\\' && str[i+1] == 'n') || str[i] == '\n') i++;
			console[nLines][j++] = 0;
			j = 0; nLines++;
		}
		console[nLines][j++] = str[i];
	}
	console[nLines++][j] = 0;
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










