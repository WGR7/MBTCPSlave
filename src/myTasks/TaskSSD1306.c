/*
 * TaskSSD1306.c
 *
 *  Created on: 14.04.2019
 *      Author: E6430
 */

#include "FreeRTOS.h"
#include "TaskSSD1306.h"
#include "ssd1306.h"
#include "utils/Typewriter/Console.h"

void vTaskSSD1306(void *pvParameters){
	sSSDdisplay *pDisplay = (sSSDdisplay *)pvParameters;
	SSD1306InitDisplay(pDisplay);

	// quick test:
	SSD1306ClearBuffer(pDisplay);
	//SSD1306SetPixelHorizAdr(pDisplay, 5, 13);
	//SSD1306SetPixelHorizAdr(pDisplay, 5, 14);
	//SSD1306SetPixelHorizAdr(pDisplay, 5, 15);
	//SSD1306SetPixelHorizAdr(pDisplay, 6, 16);

	sConsole console;
	ConsoleInit(&console);
	ConsoleRender(&console);

	while(1){
		SSD1306SendBuffer(pDisplay);
		vTaskDelay(pdMS_TO_TICKS(REFRESH_PERIOD_MS));
	}

}
