/*
 * Console.h
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */

#ifndef UTILS_TYPEWRITER_CONSOLE_H_
#define UTILS_TYPEWRITER_CONSOLE_H_

// This is a pixel-by-pixel, line-by-line app-specific console, designed to work with "Typewriter" library

#include "Typewriter.h"
#include <stdio.h>


#define LINES_COUNT			5
#define MAX_LINE_CHARS		20

typedef struct {
	sTextLine line[LINES_COUNT];
}sConsole;

void ConsoleInit(sConsole *);
void ConsoleRender(sConsole *);

#endif /* UTILS_TYPEWRITER_CONSOLE_H_ */
