/*
 * Console.h
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */

#ifndef UTILS_TYPEWRITER_CONSOLE_H_
#define UTILS_TYPEWRITER_CONSOLE_H_

// This is a pixel-by-pixel, line-by-line app-specific console

#include "Typewriter.h"
#include <stdio.h>


//#define LINES_COUNT			5
//#define MAX_LINE_CHARS		20
//#define CONSOLEROWS		5
//#define CONSOLECOLUMNS	30

#define CONSOLEPRINTMAXLEN 30
#define CONSOLEPRINT(con,...) char _tmpprintbuffer[CONSOLEPRINTMAXLEN]; snprintf(_tmpprintbuffer, sizeof(_tmpprintbuffer), __VA_ARGS__); ConsoleFeedBuffer(con, _tmpprintbuffer, sizeof(_tmpprintbuffer));


typedef struct {
	char *cCharacterBuffer;
	uint8_t uCurrentColumn, uCurrentRow;
	uint8_t uColumnsCount, uRowsCount;
	uint8_t uXStart, uYStart, uXAdvance, uYAdvance;
	GFXfont sFont;
}sConsole;

sConsole* ConsoleInit(sConsole *Console, char *cBuffer, uint8_t uRows, uint8_t uColumns, uint8_t uXStart, uint8_t uYStart, uint8_t uXAdvance, uint8_t uYAdvance, GFXfont sFont);
void ConsoleRender(sConsole *Console);
void ConsoleSetCursor(sConsole *Console, uint8_t uColumn, uint8_t uRow);
void ConsolePutChar(sConsole *Console, char NewChar);
void ConsoleFeedBuffer(sConsole *Console, char *Buffer, uint8_t Size);
void ConsoleClear(sConsole *Console);

#endif /* UTILS_TYPEWRITER_CONSOLE_H_ */
