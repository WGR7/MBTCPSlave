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
		sADUFrame aduframe;
		// start of frame - wait for MBAP (7bytes) + function code (1byte) = 8bytes
		while(xStreamBufferBytesAvailable(inputStreamHandle) < 8){vTaskDelay(pdMS_TO_TICKS(10));}

		uint8_t	temp[8];
		xStreamBufferReceive(inputStreamHandle, temp, 8, portMAX_DELAY);
		memcpy(&(aduframe),temp,8);		// bytes up to uint id
		// need to swap bytes in uint16's cause stm is holding them LSB first
		aduframe.MBAP.TransID = BYTES_IN_WORD_SWAP(aduframe.MBAP.TransID);
		//aduframe.MBAP.ProtocolID = BYTES_IN_WORD_SWAP(aduframe.MBAP.ProtocolID); // not really necessary as always =0
		aduframe.MBAP.Length = BYTES_IN_WORD_SWAP(aduframe.MBAP.Length);
		aduframe.PDU.FunctionCode = (eFcnCode)temp[7];
		// now ADU is filled up to function code and data byte count is known (=Length-1)
		uint16_t data_bytes_received = 0;
		while(data_bytes_received < aduframe.MBAP.Length - 2){
			data_bytes_received += xStreamBufferReceive(inputStreamHandle, &(aduframe.PDU.Data)+data_bytes_received, 10, pdMS_TO_TICKS(20));
		}
		// now aduframe should be complete


		//xQueueSend(outputMessageQueueHandle, &replymsg, pdMS_TO_TICKS(500));

	}
}
