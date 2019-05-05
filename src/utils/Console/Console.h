/*
 * Console.h
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */

#ifndef UTILS_CONSOLE_CONSOLE_H_
#define UTILS_CONSOLE_CONSOLE_H_

// This is a pixel-by-pixel, line-by-line app-specific console. Utilizes Adafruits GFX library GFXfont structure for storing fonts

#include <stdio.h>
#include <Console/gfxfont.h>

// To be delivered by user:
// A functions that sets pixel in [x,y] on the display
extern void ConsoleSetPix(uint8_t, uint8_t);

// Easy print for console output
#define CONSOLEPRINTMAXLEN 30
#define CONSOLEPRINT(con,...) char _tmpprintbuffer[CONSOLEPRINTMAXLEN]; snprintf(_tmpprintbuffer, sizeof(_tmpprintbuffer), __VA_ARGS__); ConsoleFeedBuffer(con, _tmpprintbuffer, sizeof(_tmpprintbuffer));

// Main console structure
typedef struct {
	char *cCharacterBuffer;
	uint8_t uCurrentColumn, uCurrentRow;
	uint8_t uColumnsCount, uRowsCount;
	uint8_t uXStart, uYStart, uXAdvance, uYAdvance;
	GFXfont sFont;
}sConsole;

// Interface functions:
sConsole* ConsoleInit(sConsole *Console, char *cBuffer, uint8_t uRows, uint8_t uColumns, uint8_t uXStart, uint8_t uYStart, uint8_t uXAdvance, uint8_t uYAdvance, GFXfont sFont);
void ConsoleRender(sConsole *Console);
void ConsoleSetCursor(sConsole *Console, uint8_t uColumn, uint8_t uRow);
void ConsolePutChar(sConsole *Console, char NewChar);
void ConsoleFeedBuffer(sConsole *Console, char *Buffer, uint8_t Size);
void ConsoleClear(sConsole *Console);

#endif /* UTILS_CONSOLE_CONSOLE_H_ */
