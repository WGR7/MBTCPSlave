/*
 * w5500.h
 *
 *  Created on: 28.12.2018
 *      Author: E6430
 */

#ifndef UTILS_W5500_SPI_H_
#define UTILS_W5500_SPI_H_

#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "w5500_spiregisters.h"


#define W5500_SELECT 	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
#define W5500_DESELECT 	GPIO_SetBits(GPIOC, GPIO_Pin_0);
#define W5500_RESET_ON 	GPIO_ResetBits(GPIOC, GPIO_Pin_1);
#define W5500_RESET_OFF GPIO_SetBits(GPIOC, GPIO_Pin_1);

typedef struct{
	uint16_t 	AddressPhase;
	uint8_t		BSB;
	uint8_t		RWMode;
	uint8_t		OpMode;
}tDataFrameHeader;

void W5500_ReadRegisters(uint16_t reg_offset, uint8_t block_no, uint16_t reg_count, uint8_t *dest);
uint8_t W5500_ReadByte(uint16_t reg_offset, uint8_t block_no);
uint16_t W5500_ReadWord(uint16_t reg_offset, uint8_t block_no);
void W5500_WriteRegisters(uint16_t reg_offset, uint8_t block_no, uint16_t reg_count, uint8_t *source);
void W5500_WriteByte(uint16_t reg_offset, uint8_t block_no, uint8_t value);
void W5500_WriteWord(uint16_t reg_offset, uint8_t block_no, uint16_t value);
void W5500_SerializeHeader(tDataFrameHeader header, uint8_t *dest);

#endif /* UTILS_W5500_SPI_H_ */
