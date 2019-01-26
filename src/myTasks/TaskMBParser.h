/*
 * TaskMBParser.h
 *
 *  Created on: 25.01.2019
 *      Author: E6430
 */

#ifndef MYTASKS_TASKMBPARSER_H_
#define MYTASKS_TASKMBPARSER_H_

#include "stdlib.h"
#include "FreeRTOS.h"
#include "string.h"
#include "task.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "queue.h"
#include "TaskW5500.h"		// <-- for common message declaration

typedef struct{
	StreamBufferHandle_t 	inputStream;
	QueueHandle_t 			outputMessageQueue;
	uint8_t					sourceSocketNo;
}sParserInOutParameter;

typedef enum{
	FCN_READ_HOLD_REG			= 0x03,
	FCN_WRITE_SING_REG			= 0x06,
	FCN_WRITE_MULT_REGS			= 0x10
}eFcnCode;

typedef struct{
	uint16_t		TransID;
	uint16_t		ProtocolID;
	uint16_t		Length;
	uint8_t			UnitI;
}sMBAPHeader;

typedef struct{
	eFcnCode		FunctionCode;
	uint16_t		*Data;
}sPDU;

typedef struct{
	sMBAPHeader		MBAP;
	sPDU			PDU;
}sADUFrame;

void vTaskMBParser(void *pvParameters);


#endif /* MYTASKS_TASKMBPARSER_H_ */
