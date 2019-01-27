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

#define MAX_REGS_IN_FRAME	126

typedef struct{
	StreamBufferHandle_t 	inputStream;
	QueueHandle_t 			outputMessageQueue;
	uint8_t					sourceSocketNo;
}sParserInOutParameter;

typedef enum{
	FCN_READ_HOLD_REG			= (uint8_t)0x03,
	FCN_WRITE_SING_REG			= (uint8_t)0x06,
	FCN_WRITE_MULT_REGS			= (uint8_t)0x10
}eFcnCode;

typedef struct{
	uint16_t		TransID;
	uint16_t		ProtocolID;
	uint16_t		Length;		// number of bytes of data + 1 byte for unit id
	uint8_t			UnitID;
}sMBAPHeader;

typedef struct{
	eFcnCode		FunctionCode;
	uint16_t		Data[MAX_REGS_IN_FRAME];
}sPDU;

typedef struct{
	sMBAPHeader		MBAP;
	sPDU			PDU;
}sADUFrame;

#define BYTES_IN_WORD_SWAP(word) ((word>>8) | (word<<8))

void vTaskMBParser(void *pvParameters);


#endif /* MYTASKS_TASKMBPARSER_H_ */
