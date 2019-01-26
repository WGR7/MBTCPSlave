/*
 * TaskSPITxRx.h
 *
 *  Created on: 28.12.2018
 *      Author: E6430
 */

#ifndef MYTASKS__TASKSPITXRX_H_
#define MYTASKS__TASKSPITXRX_H_

#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "stm32f10x_spi.h"
#include "w5500_spi.h"

#define READSTREAM_SIZE 	40
#define WRITESTREAM_SIZE	40

StreamBufferHandle_t SPIReadStreamHandle;
//StaticStreamBuffer_t SPIReadStream;
//static uint8_t ReadBufferMem[READSTREAM_SIZE+1];
StreamBufferHandle_t SPIWriteStreamHandle;
//StaticStreamBuffer_t SPIWriteStream;
//static uint8_t WriteBufferMem[WRITESTREAM_SIZE+1];


// powinien równie¿ w przysz³oœci byc semafor

void vTaskSPITxRx(void *pvParameters);
void vTaskSPITxRxSetup();

void vTestFunction(void);

#endif /* MYTASKS__TASKSPITXRX_H_ */
