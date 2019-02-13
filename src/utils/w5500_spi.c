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
	uint16_t miso_counter = 0;
	uint16_t mosi_counter = 0;
	while(!((miso_counter>2) && (mosi_counter>2))){
		if((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==SET) && mosi_counter<3){
			SPI_I2S_SendData(SPI1, header_buff[mosi_counter]);
			mosi_counter++;
		}
		if((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==SET) && miso_counter<3){
			SPI_I2S_ReceiveData(SPI1);
			miso_counter++;
		}
	}

	// transmit the data
	miso_counter = 0;
	mosi_counter = 0;
	while(!((miso_counter>reg_count-1) && (mosi_counter>reg_count-1))){
		if((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==SET) && mosi_counter<reg_count){
			// transmit 0x00 just to roll the clock
			SPI_I2S_SendData(SPI1, 0x00);
			mosi_counter++;
		}
		if((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==SET) && miso_counter<reg_count){
			dest[miso_counter] = SPI_I2S_ReceiveData(SPI1);
			miso_counter++;
		}
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


void W5500_IF_Select(){
	W5500_SELECT;
}

void W5500_IF_DeSelect(){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
	W5500_DESELECT;
}

uint8_t	W5500_IF_ReadByte(){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
	// transmit data (=0)
	SPI_I2S_SendData(SPI1, 0x00);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET){}
	return SPI_I2S_ReceiveData(SPI1);
}

void W5500_IF_WriteByte(uint8_t byte){
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
	// transmit data
	SPI_I2S_SendData(SPI1, byte);
}

void W5500_IF_ReadBurst(uint8_t *pBuff, uint16_t len){
	// write first data (data = 0's)
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
	SPI_I2S_SendData(SPI1, 0x00);
	for(;len>1;len--){
		// write next data
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_I2S_SendData(SPI1, 0x00);
		// wait for and read rx data
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET){}
		*pBuff++ = SPI_I2S_ReceiveData(SPI1);
	}
	// wait for and read last rx data
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET){}
	*pBuff++ = SPI_I2S_ReceiveData(SPI1);

}

void W5500_IF_WriteBurst(uint8_t *pBuff, uint16_t len){
	// tx first byte
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
	SPI_I2S_SendData(SPI1, *pBuff++);
	for(;len>1;len--){
		// write next data
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
		SPI_I2S_SendData(SPI1, *pBuff++);
		// wait for and read rx data just to clear RX buffer
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET){}
		SPI_I2S_ReceiveData(SPI1);
	}
	// wait for and read last rx data
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET){}
	SPI_I2S_ReceiveData(SPI1);
}
