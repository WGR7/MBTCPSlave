/*
 * Console_port.c
 *
 *  Created on: 05.05.2019
 *      Author: E6430
 */


#include <inttypes.h>
#include "SSD1306/ssd1306.h"

// This is the display object that console will be displayed on
extern sSSDdisplay display;


void ConsoleSetPix(uint8_t x, uint8_t y){

	SSD1306SetPixelHorizAdr(&display, x, y);
}

