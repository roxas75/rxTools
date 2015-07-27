#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_WIDTH	280
#define CONSOLE_HEIGHT	200
#define MAX_LINES (int)	(CONSOLE_HEIGHT / FONT_HEIGHT - 4)
#define MAX_LINE_LENGTH	(CONSOLE_WIDTH / FONT_HWIDTH - 4)
#define CONSOLE_X	(SCREEN_WIDTH - CONSOLE_WIDTH) / 2
#define CONSOLE_Y	(SCREEN_HEIGHT - CONSOLE_HEIGHT) / 2

void ConsoleInit();
void ConsoleShow();
void ConsoleFlush();
void ConsoleAddText(wchar_t* str);
void ConsoleSetBackgroundColor(int color);
int ConsoleGetBackgroundColor();
void ConsoleSetBorderColor(int color);
int ConsoleGetBorderColor();
void ConsoleSetTextColor(int color);
int ConsoleGetTextColor();
void ConsoleSetXY(int x, int y);
void ConsoleGetXY(int *x, int *y);
void ConsoleSetWH(int width, int height);
void ConsoleSetTitle(const wchar_t *format, ...);
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

void print(const wchar_t *format, ...);
#endif
