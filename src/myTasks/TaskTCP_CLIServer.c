/*
 * TaskTCP_CLIServer.c
 *
 *  Created on: 19.02.2019
 *      Author: E6430
 */

#include "TaskTCP_CLIServer.h"
// FreeRTOS related
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
// Wiznet related
#include "socket.h"
#include "w5500.h"

uint8_t uiInputBuffer[INPUT_BUFF_LENGTH+1];
uint8_t uiGreetMessage[] = "Hello, greetings!\0";

void vTaskTCP_CLIServer(void *pvParameters){
	sCLIConfig config = *((sCLIConfig*)pvParameters);
	uint8_t prev_status = SOCK_CLOSED;

	vTaskDelay(pdMS_TO_TICKS(4000));

	while(1){
		uint8_t soc_status;
		getsockopt(config.SocketNumber, SO_STATUS, &soc_status);
		switch(soc_status){
			case SOCK_CLOSED:
				// open socket
				socket(config.SocketNumber, Sn_MR_TCP, config.PortNumber, 0x00);
				listen(config.SocketNumber);
				break;
			case SOCK_ESTABLISHED:
				if(prev_status<soc_status){
					// Just established a connection - send greetings!
					send(config.SocketNumber, uiGreetMessage, strlen(uiGreetMessage));
				}
				// check if there is some new data...
				uint16_t newdatacount=0;
				getsockopt(config.SocketNumber, SO_RECVBUF, &newdatacount);
				if(newdatacount>INPUT_BUFF_LENGTH){
					newdatacount=INPUT_BUFF_LENGTH;
				}
				if(newdatacount){
					// fetch it to uiInputBuffer
					recv(config.SocketNumber, uiInputBuffer, newdatacount);
				}

				break;
			case SOCK_CLOSE_WAIT:
				// send disconnect command
				disconnect(config.SocketNumber);
				break;
		}
		prev_status = soc_status;
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
