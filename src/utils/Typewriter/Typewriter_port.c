/*
 * Typewriter_port.c
 *
 *  Created on: 03.05.2019
 *      Author: E6430
 */
#include <inttypes.h>
#include "SSD1306/ssd1306.h"

extern sSSDdisplay display;

void TypeWriterSetPix(uint8_t x, uint8_t y){

	SSD1306SetPixelHorizAdr(&display, x, y);
}
