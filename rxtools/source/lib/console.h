#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_WIDTH 300
#define CONSOLE_HEIGHT 200
#define CHAR_WIDTH 10
#define MAX_LINES (int) (CONSOLE_HEIGHT / CHAR_WIDTH - 4)
#define MAX_LINE_LENGTH 36//(int) (CONSOLE_WIDTH / CHAR_WIDTH - 4)
#define CONSOLE_X (400 - CONSOLE_WIDTH) / 2
#define CONSOLE_Y (240 - CONSOLE_HEIGHT) / 2

void ConsoleInit();
void ConsoleShow();
void ConsoleFlush();
void ConsoleAddText(char* str);
void ConsoleSetBackgroundColor(int color);
int ConsoleGetBackgroundColor();
void ConsoleSetBorderColor(int color);
int ConsoleGetBorderColor();
void ConsoleSetTextColor(int color);
int ConsoleGetTextColor();
void ConsoleSetXY(int x, int y);
void ConsoleGetXY(int *x, int *y);
void ConsoleSetWH(int width, int height);
void ConsoleSetTitle(const char *format, ...);
void ConsoleNextLine();
void ConsolePrevLine();
void ConsoleNext();
void ConsolePrev();
void ConsoleSetBorderWidth(int width);
int ConsoleGetBorderWidth(int width);
void ConsoleSetSpecialColor(int color);
int ConsoleGetSpecialColor();
void ConsoleSetSpacing(int space);
int ConsoleGetSpacing();

void print(const char *format, ...);
#endif
