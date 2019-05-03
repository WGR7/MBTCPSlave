/*
 * typewriter.h
 *
 *  Created on: 06.04.2018
 *      Author: E6430
 */

#ifndef TYPEWRITER_H_
#define TYPEWRITER_H_

#include <string.h>
#include <inttypes.h>
#include <Typewriter/gfxfont.h>



extern void DisplaySetPixelHorizAdr(uint8_t, uint8_t);
extern void TypeWriterSetPix(uint8_t, uint8_t);

typedef struct{
	uint8_t XPos, YPos, MaxLen, Length;
	char *Buffer;
	const GFXfont *LineFont;
}sTextLine;

void sTextLineInit(sTextLine*, char *, uint8_t, uint8_t, uint8_t, const GFXfont*);
void sTextLineSetText(sTextLine*, char*);
void sTextLineDraw(sTextLine*);

uint8_t TypeWriterDrawChar(uint8_t, uint8_t, char, GFXfont);
//void TypeWriterSetPix(uint8_t, uint8_t);

uint8_t TypeWriterInt2Ascii(char *, int);
uint8_t TypeWriterInt2AsciiLength(int);
void TypeWriterFloat2Ascii(char *, float, uint8_t);

#endif /* TYPEWRITER_H_ */
