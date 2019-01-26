/*
 * TaskMBParser.c
 *
 *  Created on: 25.01.2019
 *      Author: E6430
 */

#include "TaskMBParser.h"



void vTaskMBParser(void *pvParameters){
	// assign input stream buffer
	sParserInOutParameter *params;
	params = (sParserInOutParameter*)pvParameters;
	StreamBufferHandle_t inputStreamHandle = params->inputStream;
	QueueHandle_t outputMessageQueueHandle = params->outputMessageQueue;
	uint8_t sourceSocket = params->sourceSocketNo;

	while(1){
		uint8_t buff[20];
		memset(buff, 0, 20);
		uint16_t received_len = xStreamBufferReceive(inputStreamHandle, buff, 20, portMAX_DELAY);

		// answer...
		sW5500ControlMessage replymsg;
		replymsg.MessageType = W5500_MESSAGE_SEND;
		replymsg.SocketNo = sourceSocket;
		char replytxt[] = "dostalem!";
		replymsg.DataPointer = malloc(sizeof(replytxt));
		memcpy(replymsg.DataPointer, replytxt, sizeof(replytxt));
		replymsg.MessageLength = sizeof(replytxt);

		xQueueSend(outputMessageQueueHandle, &replymsg, pdMS_TO_TICKS(500));

	}
}
