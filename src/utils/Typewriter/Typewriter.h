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


uint8_t TypeWriterDrawChar(uint8_t, uint8_t, char, GFXfont);

#endif /* TYPEWRITER_H_ */
