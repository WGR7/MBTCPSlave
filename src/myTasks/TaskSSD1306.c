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
#include "utils/Typewriter/TomThumb.h"

void vTaskSSD1306(void *pvParameters){
	sSSDdisplay *pDisplay = (sSSDdisplay *)pvParameters;
	SSD1306InitDisplay(pDisplay);
	SSD1306ClearBuffer(pDisplay);
	SSD1306SendBuffer(pDisplay);

	char consolebuffer[8*25];
	sConsole console;
	ConsoleInit(&console, consolebuffer, 8, 25, 0, 6, 5, 8, TomThumb);
	ConsoleClear(&console);

	uint8_t asciipointer = 21;

	while(1){
		SSD1306ClearBuffer(pDisplay);

		CONSOLEPRINT(&console, "dec=%u hex=%x asc=%c\n", asciipointer, asciipointer, asciipointer);
		ConsoleRender(&console);

		if(asciipointer++ > 126){
			asciipointer = 21;
		}

		SSD1306SendBuffer(pDisplay);
		vTaskDelay(pdMS_TO_TICKS(REFRESH_PERIOD_MS));
	}

}
