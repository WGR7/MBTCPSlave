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

void sTextLineInit(sTextLine* textline, char* buffer, uint8_t posx, uint8_t posy, uint8_t maxlen, const GFXfont* font){
	textline->XPos = posx;
	textline->YPos = posy;
	textline->MaxLen = maxlen;
	textline->Length = 0;
	textline->Buffer = buffer;
	textline->LineFont = font;
}

void sTextLineSetText(sTextLine* textline, char* source){
	uint8_t charcounter = 0;
	while((source[charcounter] != '\0') && (charcounter < textline->MaxLen) ){
		textline->Buffer[charcounter] = source[charcounter];
		charcounter++;
	}
	textline->Length = charcounter;
}

void sTextLineDraw(sTextLine* textline){
	uint8_t xpos = textline->XPos;
	for(uint8_t charcounter = 0; charcounter < textline->Length; charcounter++){
		xpos += TypeWriterDrawChar(xpos, textline->YPos, textline->Buffer[charcounter], *(textline->LineFont));
	}
}

uint8_t TypeWriterInt2Ascii(char *buf, int value){
	uint8_t len = 0;
	if(value == 0){
		*buf++ = 0x30;
		len++;
	}else{
		if(value<0){
			value = -value;
			*buf++ = '-';
			len++;
		}
		int tens = 1000000;
		uint8_t trailing = 1;
		while((tens /= 10)>0){
			int digit = value/tens;
			if((digit == 0 && trailing == 0)||(digit > 0)){
				trailing = 0;
				*buf++ = (char)(digit+0x30);
				len++;
				value %= tens;
			}
		}
	}
	*buf = '\0';
	return len;
}

uint8_t TypeWriterInt2AsciiLength(int value){
	uint8_t len = 0;
	if(value == 0){
		len = 1;
	}else{
		if(value<0){
			len++;
			value = -value;
		}
		int tens = 1000000;
		uint8_t trailing = 1;
		while((tens /= 10)>0){
			int digit = value/tens;
			if((digit == 0 && trailing == 0)||(digit > 0)){
				trailing = 0;
				len++;
				value %= tens;
			}
		}
	}
	return len;
}

void TypeWriterFloat2Ascii(char *buf, float value, uint8_t precision){
	if(value < 0){
		value = -value;
		*buf++ = '-';
	}
	int value_int = (int)value;
	int value_point = (int)((value-value_int)*pow(10,precision));

	uint8_t adv_i = TypeWriterInt2Ascii(buf, value_int);
	buf += adv_i;
	*buf++ = '.';
	uint8_t adv_f = TypeWriterInt2AsciiLength(value_point);
	for(uint8_t counter=0; counter<(precision-adv_f); counter++){
		*buf++ = 0x30;		// add '0' to extend to precision
	}
	buf += TypeWriterInt2Ascii(buf, value_point);
	*buf = '\0';
}

uint8_t sTextLinePutChar(sTextLine* line, char newchar){
	if(line->Length >= line->MaxLen){
		// No more space! Return 0!
		return 0;
	}else{
		if(newchar == '\b'){
			if(line->Length > 0){
				//line->Length-=1;
				line->Buffer[--line->Length] = '\0';
			}
		}else{
			line->Buffer[line->Length++] = newchar;
		}
		return 1;
	}
}

uint8_t sTextLineCopyFrom(sTextLine* dest, sTextLine* source){

}

void sTextLineClear(sTextLine *line){

}
