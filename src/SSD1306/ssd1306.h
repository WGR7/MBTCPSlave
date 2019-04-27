/*
 * ssd1306.h
 *
 *  Created on: 11.06.2018
 *      Author: E6430
 */

#ifndef SSD1306_SSD1306_H_
#define SSD1306_SSD1306_H_

#include <inttypes.h>
#include <string.h>

// Display config - try not to change it
#define SSD1306_PIXELS_X 		128	// horizontal resolution = # columns
#define SSD1306_PAGES 			8
#define SSD1306_PIX_PER_PAGE	8
#define SSD1306_PIXELS_Y 		64		// vertical resolution = PAGES x PIXELSperPAGE

// Pixel location in [in_buffer_index, in_page_index]
typedef struct{
	uint32_t buffIndex;
	uint8_t pageIndex;
} sPixLoc;

typedef struct {
	// Interface functions, to be assigned during init:
	void (*fpSelectChip)();
	void (*fpDeSelectChip)();
	void (*fpSetData)();
	void (*fpSetCommand)();
	void (*fpSendByte)(uint8_t);
	void (*fpSendBytesBurst)(uint8_t *pData, uint16_t uLength);
	void (*fpResetActive)();
	void (*fpResetInactive)();
	void (*fpDelay)(uint16_t);
	uint8_t PixBuffer[SSD1306_PIXELS_X * SSD1306_PAGES];
}sSSDdisplay;

// External interface
void SSD1306InitStruct(sSSDdisplay *,void (*)(), void (*)(), void (*)(), void (*)(), void (*)(uint8_t), void (*)(uint8_t*, uint16_t) , void (*)(), void (*)(), void (*)(uint16_t));
void SSD1306InitDisplay(sSSDdisplay *);
void SSD1306ClearBuffer(sSSDdisplay *);
void SSD1306SendBuffer(sSSDdisplay *);
uint8_t SSD1306SetPixelHorizAdr(sSSDdisplay *, uint8_t, uint8_t);


// Functions for internal use
void SSD1306SendCommand(sSSDdisplay *, uint8_t);
void SSD1306SendData(sSSDdisplay *, uint8_t);
sPixLoc SSD1306GetPixLocHorizAdr(uint8_t, uint8_t);

#endif /* SSD1306_SSD1306_H_ */
