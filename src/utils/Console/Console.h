/*
 * Console.h
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */

// Outside of ifndef guards - every module that uses console.h needs it during its own compilation
#define CONSOLEPRINTMAXLEN 30
extern char _tmpprintbuffer[CONSOLEPRINTMAXLEN];

#ifndef UTILS_CONSOLE_CONSOLE_H_
#define UTILS_CONSOLE_CONSOLE_H_

/** This is a pixel-by-pixel, line-by-line app-specific console. Utilizes Adafruits GFX library GFXfont structure for storing fonts
*	Example usage:
*
*	// prepare buffer for characters
*	char consolebuffer[8*25];
*	// init console struct with some settings and font object
*	sConsole console;
*	ConsoleInit(&console, consolebuffer, 8, 25, 0, 6, 5, 8, TomThumb);
*	// clear the buffer
*	ConsoleClear(&console);
*	// feed some characters
*	ConsolePutChar(&console, 'a');
*	// move the "cursor" to eg. column 3 and row 5
*	ConsoleSetCursor(&console, 3, 5);
*	// if you already have a buffer you want to display, feed it
*	ConsoleFeedBuffer(&console, ready_buffer, its_size);
*	// finally render it via pixel [x,y] interface provided in ConsoleSetPix(uint8_t x, uint8_t y)
*	ConsoleRender(&console);
*	// now it is up to display driver how to display it...
*
*/

#include <stdio.h>
#include <Console/gfxfont.h>

// To be delivered by user:
// A functions that sets pixel in [x,y] on the display
extern void ConsoleSetPix(uint8_t, uint8_t);

// Easy print for console output


//#define CONSOLEPRINT(con,...) char _tmpprintbuffer[CONSOLEPRINTMAXLEN]; snprintf(_tmpprintbuffer, sizeof(_tmpprintbuffer), __VA_ARGS__); ConsoleFeedBuffer(con, _tmpprintbuffer, sizeof(_tmpprintbuffer));
#define CONSOLEPRINT(con,...) snprintf(_tmpprintbuffer, sizeof(_tmpprintbuffer), __VA_ARGS__); ConsoleFeedBuffer(con, _tmpprintbuffer, sizeof(_tmpprintbuffer));

// Main console structure
typedef struct {
	char *cCharacterBuffer;
	uint8_t uCurrentColumn, uCurrentRow;
	uint8_t uColumnsCount, uRowsCount;
	uint8_t uXStart, uYStart, uXAdvance, uYAdvance;
	GFXfont sFont;
}sConsole;

// Some higher up piece of code will allocate it
extern sConsole console;

// Interface functions:

/**
 * ConsoleInit
 * Initializes a console structure
 * Params:
 * *Console - pointer to empty sConsole struct that will be used
 * *cBuffer - pointer to already allocated char buffer, big enough to accomodate all the console's characters
 * uRows - describes how many rows of text a console can fit
 * uColumns - as above, but columns
 * uXStart, uYStart - starting point in pixels, it indicated a lower-left corner of first letter that will be rendered (column=0,row=0)
 * uXAdvance - a fixed constant width in pixels of every character, should be large enough so characters do not overlap
 * uYAdvance - a fixed constant distance in pixels between consecutive lines of text
 * sFont - a struct containing font information
 *
 */
sConsole* ConsoleInit(sConsole *Console, char *cBuffer, uint8_t uRows, uint8_t uColumns, uint8_t uXStart, uint8_t uYStart, uint8_t uXAdvance, uint8_t uYAdvance, GFXfont sFont);
void ConsoleRender(sConsole *Console);
void ConsoleSetCursor(sConsole *Console, uint8_t uColumn, uint8_t uRow);
void ConsolePutChar(sConsole *Console, char NewChar);
void ConsoleFeedBuffer(sConsole *Console, char *Buffer, uint8_t Size);
void ConsoleClear(sConsole *Console);

#endif /* UTILS_CONSOLE_CONSOLE_H_ */
