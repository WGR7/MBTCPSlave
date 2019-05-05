/*
 * typewriter.c
 *
 *  Created on: 06.04.2018
 *      Author: E6430
 */


#include <Typewriter/Typewriter.h>



uint8_t TypeWriterDrawChar(uint8_t base_x, uint8_t base_y, char character, GFXfont font){
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
			TypeWriterSetPix(act_x + start_x, act_y + start_y);
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

