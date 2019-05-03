/*
 * Console.c
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */

#include "console.h"
#include "gfxfont.h"
#include "Typewriter.h"
#include "TomThumb.h"

const char linebuffer1[MAX_LINE_CHARS];
const char linebuffer2[MAX_LINE_CHARS];
const char linebuffer3[MAX_LINE_CHARS];
const char linebuffer4[MAX_LINE_CHARS];

void ConsoleInit(sConsole *con){
	sTextLineInit(&con->line[0], linebuffer1, 0, 10, 30, &TomThumb);
	sTextLineInit(&con->line[1], linebuffer2, 0, 20, 30, &TomThumb);
	sTextLineInit(&con->line[2], linebuffer3, 0, 30, 30, &TomThumb);
	sTextLineInit(&con->line[3], linebuffer4, 0, 40, 30, &TomThumb);
	sTextLineInit(&con->line[4], linebuffer4, 0, 50, 30, &TomThumb);

	sTextLineSetText(&con->line[0], "012345678901234567890123456789");
	sTextLineSetText(&con->line[1], "012345678901234567890123456789");
	sTextLineSetText(&con->line[2], "012345678901234567890123456789");
	sTextLineSetText(&con->line[3], "012345678901234567890123456789");
	sTextLineSetText(&con->line[4], "012345678901234567890123456789");
}

void ConsoleRender(sConsole *con){
	sTextLineDraw(&con->line[0]);
	sTextLineDraw(&con->line[1]);
	sTextLineDraw(&con->line[2]);
	sTextLineDraw(&con->line[3]);
	sTextLineDraw(&con->line[4]);
}
