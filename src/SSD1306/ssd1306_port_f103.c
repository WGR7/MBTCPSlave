/*
 * ssd1306_port_f103.c
 *
 *  Created on: 14.06.2018
 *      Author: E6430
 */

#include "ssd1306_port_f103.h"
#include "FreeRTOS.h"
#include "semphr.h"

#ifdef USE_DMA
#include "stm32f10x_dma.h"
#endif

extern SemaphoreHandle_t SPI_Mutex;

#ifdef USE_DMA


void SSDSendDMA(uint8_t *pData, uint16_t uLength){
	SSDSetDataMode();
	SSDSelectChip();

	DMA_InitTypeDef dmaconf;
	dmaconf.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
	dmaconf.DMA_MemoryBaseAddr = (uint32_t)pData;
	dmaconf.DMA_DIR = DMA_DIR_PeripheralDST;
	dmaconf.DMA_BufferSize = uLength;
	dmaconf.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaconf.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaconf.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmaconf.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaconf.DMA_Mode = DMA_Mode_Normal;
	dmaconf.DMA_Priority = DMA_Priority_Medium;
	dmaconf.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3 , &dmaconf);
	DMA_Cmd(DMA1_Channel3, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

	while(!(DMA_GetFlagStatus(DMA1_FLAG_TC3))){};
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
	DMA_Cmd(DMA1_Channel3, DISABLE);

	SSDDeSelectChip();
	return;
}

#endif

void SSDSelectChip(){
	if(xSemaphoreTake(SPI_Mutex, pdMS_TO_TICKS(1000)) == pdTRUE){
		GPIO_ResetBits(GPIOC, SSD_CS_PIN);
	}
	return;
}

void SSDDeSelectChip(){
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
	GPIO_SetBits(GPIOC, SSD_CS_PIN);
	xSemaphoreGive(SPI_Mutex);
	return;
}

void SSDSetDataMode(){
	GPIO_SetBits(GPIOA, SSD_DC_PIN);
	return;
}

void SSDSetCommandMode(){
	GPIO_ResetBits(GPIOA, SSD_DC_PIN);
	return;
}

void SSDSendByte(uint8_t byte){
	while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)){}
	SPI_I2S_SendData(SPI1, byte);
	return;
}

void SSDResetActive(){
	GPIO_ResetBits(GPIOC, SSD_RES_PIN);
	return;
}

void SSDResetInactive(){
	GPIO_SetBits(GPIOC, SSD_RES_PIN);
	return;
}
void SSDDelay(uint16_t milis){
	vTaskDelay(pdMS_TO_TICKS(milis));
	return;
}
