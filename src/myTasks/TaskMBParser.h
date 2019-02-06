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
// data model defines
#define HOLDING_REGS_COUNT	200
#define EXC_CODE_ILLEGAL_FCN		0x01
#define EXC_CODE_ILLEGAL_DATA_ADDR	0x02
#define EXC_CODE_ILLEGAL_DATA_VAL	0x03


typedef struct{
	StreamBufferHandle_t 	inputStream;
	QueueHandle_t 			outputMessageQueue;
	uint8_t					sourceSocketNo;
	uint32_t				StatsOKResp;
	uint32_t				StatsExcResp;
	uint32_t				StatsErrors;
}sParserInOutParameter;

typedef enum{
	FCN_READ_HOLD_REGS			= (uint8_t)0x03,
	FCN_EXC_READ_HOLD_REGS		= (uint8_t)0x83,
	FCN_WRITE_SING_REG			= (uint8_t)0x06,
	FCN_EXC_WRITE_SING_REG		= (uint8_t)0x86,
	FCN_WRITE_MULT_REGS			= (uint8_t)0x10,
	FCN_EXC_WRITE_MULT_REGS		= (uint8_t)0x90,
}eFcnCode;



typedef struct __attribute__((packed)){
	uint16_t		TransID;
	uint16_t		ProtocolID;
	uint16_t		Length;		// number of bytes of data + 1 byte for unit id
	uint8_t			UnitID;
}sMBAPHeader;

typedef struct __attribute__((packed)){
	eFcnCode		FunctionCode;
	uint8_t		Data[2*MAX_REGS_IN_FRAME + 1];
}sPDU;

typedef struct __attribute__((packed)){
	sMBAPHeader		MBAP;
	sPDU			PDU;
}sADUFrame;

typedef struct{
	int16_t	HoldingRegs[HOLDING_REGS_COUNT];
}sDataModel;

#define BYTES_IN_WORD_SWAP(word) ((word>>8) | (word<<8))

// global data model
sDataModel		DataModel;

// Task for processing incoming requests and replying with answer
void vTaskMBParser(void *pvParameters);

// dealing with exceptions
void vProcessException(sADUFrame *request_frame, uint8_t exception_code);

// processing different modbus function codes
void vProcessReadHoldingRegs(sADUFrame *request_frame);
void vProcessWriteSingleHoldingReg(sADUFrame *request_frame);
void vProcessWriteHoldingRegs(sADUFrame *request_frame);

// Data model functions for interacting with controlled device/functionality

/**
 * Initializes modbus data model register to 0's
 */
void DataModelInit();

/**
 * Returns a 2 byte integer value associated with particular holding register
 * @param register_no Number of requested register, starting from 0, up to HOLDING_REGS_COUNT-1
 * @return Register value
 */
int16_t uiGetRegisterValue(uint16_t register_no);

/**
 * Sets a register int value
 * @param register_no A register number, starting from 0, up to HOLDING_REGS_COUNT-1
 * @param value New value of register, from a range of int16
 */
void uiSetRegisterValue(uint16_t register_no, int16_t value);		// register_no 0... HOLDING_REGS_COUNT-1

#endif /* MYTASKS_TASKMBPARSER_H_ */
