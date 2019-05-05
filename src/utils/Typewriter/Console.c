/*
 * Console.c
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */

#include "console.h"
#include "gfxfont.h"
#include "Typewriter.h"
#include <string.h>

void ConsoleScrollUp(sConsole *Console);
uint8_t ConsoleDrawChar(uint8_t base_x, uint8_t base_y, char character, GFXfont font);

sConsole * ConsoleInit(sConsole *Console, char *cBuffer, uint8_t uRows, uint8_t uColumns, uint8_t uXStart, uint8_t uYStart, uint8_t uXAdvance, uint8_t uYAdvance, GFXfont sFont){
	Console->cCharacterBuffer 		= cBuffer;
	Console->uRowsCount 			= uRows;
	Console->uColumnsCount			= uColumns;
	Console->uCurrentRow			= 0;
	Console->uCurrentColumn			= 0;
	Console->uXAdvance				= uXAdvance;
	Console->uYAdvance				= uYAdvance;
	Console->uXStart				= uXStart;
	Console->uYStart				= uYStart;
	Console->sFont					= sFont;
	ConsoleClear(Console);
	return Console;
}

void ConsoleRender(sConsole *Console){
	for(uint8_t RowCounter = 0; RowCounter<Console->uRowsCount; RowCounter++){
		for(uint8_t ColumnCounter = 0; ColumnCounter<Console->uColumnsCount; ColumnCounter++){
			char CharToRender = Console->cCharacterBuffer[RowCounter*Console->uColumnsCount + ColumnCounter];
			if(CharToRender != '\0'){
				uint8_t uCharX = Console->uXStart + (ColumnCounter*Console->uXAdvance);
				uint8_t uCharY = Console->uYStart + (RowCounter*Console->uYAdvance);
				//TypeWriterDrawChar(uCharX, uCharY, CharToRender, Console->sFont);
				ConsoleDrawChar(uCharX, uCharY, CharToRender, Console->sFont);
			}

		}
	}
}

void ConsoleSetCursor(sConsole *Console, uint8_t uColumn, uint8_t uRow){
	if(uColumn<Console->uColumnsCount){
		Console->uCurrentColumn = uColumn;
	}else{
		Console->uCurrentColumn = Console->uColumnsCount-1;
	}

	if(uRow<Console->uRowsCount){
		Console->uCurrentRow = uRow;
	}else{
		Console->uCurrentRow = Console->uRowsCount-1;
	}
}

void ConsolePutChar(sConsole *Console, char NewChar){
	if(NewChar == '\n'){
		Console->uCurrentColumn = 0;
		if(Console->uCurrentRow >= Console->uRowsCount-1){
			// scroll all the lines!
			ConsoleScrollUp(Console);
		}else{
			Console->uCurrentRow++;
		}
	}else{
		Console->cCharacterBuffer[Console->uCurrentRow*Console->uColumnsCount + Console->uCurrentColumn] = NewChar;
		if(Console->uCurrentColumn >= Console->uColumnsCount-1){
			Console->uCurrentColumn = 0;
			if(Console->uCurrentRow >= Console->uRowsCount-1){
				// scroll all the lines!
				ConsoleScrollUp(Console);
			}else{
				Console->uCurrentRow++;
			}
		}else{
			Console->uCurrentColumn++;
		}
	}
}

void ConsoleScrollUp(sConsole *Console){
	// just move the content of buffer up (or "left") by 1 row
	memmove(Console->cCharacterBuffer, Console->cCharacterBuffer+Console->uColumnsCount, (Console->uRowsCount-1)*Console->uColumnsCount);
	//at the and clear last row
	memset(Console->cCharacterBuffer+(Console->uRowsCount-1)*Console->uColumnsCount, '\0', Console->uColumnsCount);
}

void ConsoleClear(sConsole *Console){
	ConsoleSetCursor(Console, 0, 0);
	memset(Console->cCharacterBuffer, '\0', (Console->uRowsCount)*(Console->uColumnsCount));
}

void ConsoleFeedBuffer(sConsole *Console, char *Buffer, uint8_t Size){
	uint8_t ByteCounter = 0;
	while((*Buffer != '\0') && ByteCounter < Size){
		ConsolePutChar(Console, *Buffer++);
		ByteCounter++;
	}
}

uint8_t ConsoleDrawChar(uint8_t base_x, uint8_t base_y, char character, GFXfont font){
	if((((uint8_t)character) < font.first) || (((uint8_t)character) > font.last)){
		return 0;
	}
	uint16_t char_index = (uint16_t)character - 0x20;
	uint8_t start_x = base_x + font.glyph[char_index].xOffset;
	uint8_t start_y = (int8_t)base_y + font.glyph[char_index].yOffset;
	uint8_t height = font.glyph[char_index].height;
	uint8_t width = font.glyph[char_index].width;
	uint8_t xadvance = font.glyph[char_index].xAdvance;
	uint16_t bitmapoffset = font.glyph[char_index].bitmapOffset;

	uint8_t bitcounter = 0, bytecounter = 0;
	uint8_t act_x = 0;
	uint8_t act_y = 0;
	for(uint16_t pixcounter = 0; pixcounter < width*height; pixcounter++){
		// aktualny pixel: act_x, act_y
		// aktualny bit:		font.bitmap[bitmapoffset+bytecounter] & (0x80>>bitcounter)
		if(font.bitmap[bitmapoffset+bytecounter] & (0x80>>bitcounter)){
			ConsoleSetPix(act_x + start_x, act_y + start_y);
		}
		if(++bitcounter == 8){
			bitcounter = 0;
			bytecounter++;
		}
		if(++act_x == width){
			act_x = 0;
			act_y++;
		}
	}
	return xadvance;
}
