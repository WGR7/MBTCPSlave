/*
 * w5500_spi.c
 *
 *  Created on: 28.12.2018
 *      Author: E6430
 */

#include "w5500_spi.h"
#include "FreeRTOS.h"
#include "task.h"

void W5500_ReadRegisters(uint16_t reg_offset, uint8_t block_no, uint16_t reg_count, uint8_t *dest){
	// prepare header
	tDataFrameHeader header;
	header.AddressPhase = reg_offset;
	header.BSB = block_no;
	header.OpMode = OPMODE_VDL;
	header.RWMode = READMODE;
	uint8_t header_buff[3];
	W5500_SerializeHeader(header, header_buff);


	// TODO - take semaphore
	//...

	// Select the chip
	W5500_SELECT;
	//vTaskDelay(pdMS_TO_TICKS(10));

	// if there is something remaining in RX buffer, clear it
	if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET){
		SPI_I2S_ReceiveData(SPI1);
	}

	// transmit header
	for(uint8_t i=0; i<3; i++){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_I2S_SendData(SPI1, header_buff[i]);
		// still need to read just to clear RX buffer...
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
		SPI_I2S_ReceiveData(SPI1);
	}
	// in loop for each byte to read:
	for(uint8_t i=0; i<reg_count; i++){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		// transmit 0x00 just to roll the clock
		SPI_I2S_SendData(SPI1, 0x00);
		// wait for new received data
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){}
		// read it to "dest"
		dest[i] = SPI_I2S_ReceiveData(SPI1);
	}

	// wait till tx'ing is done
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}

	// Deselect the chip
	W5500_DESELECT;

	// TODO - release semaphore
	//...
}

uint8_t W5500_ReadByte(uint16_t reg_offset, uint8_t block_no){
	uint8_t result = 0;
	W5500_ReadRegisters(reg_offset, block_no, 1, &result);
	return result;
}

uint16_t W5500_ReadWord(uint16_t reg_offset, uint8_t block_no){
	uint16_t result = 0;
	uint8_t tmp[] = {0,0};
	W5500_ReadRegisters(reg_offset, block_no, 2, &tmp);
	result = tmp[0]<<8 | tmp[1];
	return result;
}

void W5500_WriteRegisters(uint16_t reg_offset, uint8_t block_no, uint16_t reg_count, uint8_t *source){
	// prepare header
	tDataFrameHeader header;
	header.AddressPhase = reg_offset;
	header.BSB = block_no;
	header.OpMode = OPMODE_VDL;
	header.RWMode = WRITEMODE;
	uint8_t header_buff[3];
	W5500_SerializeHeader(header, header_buff);


	// TODO - take semaphore
	//...

	// Select the chip
	W5500_SELECT;
	//vTaskDelay(pdMS_TO_TICKS(10));

	// transmit header
	for(uint8_t i=0; i<3; i++){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_I2S_SendData(SPI1, header_buff[i]);
	}

	// in loop for each byte to write:
	for(uint8_t i=0; i<reg_count; i++){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		// transmit data
		SPI_I2S_SendData(SPI1, source[i]);
	}

	// wait till tx'ing is done
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}

	// Deselect the chip
	W5500_DESELECT;

	// TODO - release semaphore
	//...
}

void W5500_WriteByte(uint16_t reg_offset, uint8_t block_no, uint8_t value){
	W5500_WriteRegisters(reg_offset, block_no, 1, &value);
}

void W5500_WriteWord(uint16_t reg_offset, uint8_t block_no, uint16_t value){
	uint8_t tmp[] = {value>>8, value&0x00ff};
	W5500_WriteRegisters(reg_offset, block_no, 2, tmp);
}

void W5500_SerializeHeader(tDataFrameHeader header, uint8_t *dest){
	dest[0] = (uint8_t)(header.AddressPhase>>8);
	dest[1] = (uint8_t)(header.AddressPhase&0xFF);
	dest[2] = (uint8_t)(header.BSB | header.RWMode | header.OpMode);
}
