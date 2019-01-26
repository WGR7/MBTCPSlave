/*
 * TaskSPITxRx.c
 *
 *  Created on: 28.12.2018
 *      Author: E6430
 */

#include <_TaskSPITxRx.h>


void vTaskSPITxRxSetup(void){
	//SPIReadStreamHandle = xStreamBufferCreateStatic(READSTREAM_SIZE, 1, ReadBufferMem, &SPIReadStream);
	//SPIWriteStreamHandle = xStreamBufferCreateStatic(WRITESTREAM_SIZE, 1, WriteBufferMem, &SPIWriteStream);
	SPIReadStreamHandle = xStreamBufferCreate(READSTREAM_SIZE, 1);
	SPIWriteStreamHandle = xStreamBufferCreate(WRITESTREAM_SIZE, 1);
}

void vTaskSPITxRx(void *pvParameters){
	uint8_t rdbuff=0;
	// some testing
	// read version

	//uint8_t readvalue = 0;
	//W5500_ReadRegisters(W5500_VERSIONR, COMMON_REGISTER, 1, &readvalue);

	while(1){
		// quick test:
		  //W5500_RESET_ON
		  //vTaskDelay(pdMS_TO_TICKS(50));
		  //W5500_RESET_OFF
		  uint8_t readvalue = 0;
		  W5500_ReadRegisters(W5500_VERSIONR, COMMON_REGISTER, 1, &readvalue);

		  vTaskDelay(pdMS_TO_TICKS(1000));


		/*
		// read any data if available
		if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET){
			rdbuff = SPI_I2S_ReceiveData(SPI1);
			xStreamBufferSend(SPIReadStreamHandle, &rdbuff, 1, pdMS_TO_TICKS(10));
		}
		// write if any data available for writing and if writing itself possible
		if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == SET){
			if(xStreamBufferReceive(SPIWriteStreamHandle, &rdbuff, 1, pdMS_TO_TICKS(10)) > 0){
				// there was smth to write...
				SPI_I2S_SendData(SPI1, rdbuff);
			}
		}
		vTaskDelay(pdMS_TO_TICKS(50));
		*/
	}
}

