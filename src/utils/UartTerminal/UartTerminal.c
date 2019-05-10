/*
 * UartTerminal.c
 *
 *  Created on: 10.05.2019
 *      Author: E6430
 */

#include <UartTerminal/UartTerminal.h>
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

void Uart2Init(uint32_t	baudrate){
	// Enable clock signals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	// Setup hardware pins
	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_2;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_3;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);
	// Setup UART itself
	USART_InitTypeDef uart;
	USART_StructInit(&uart);
	uart.USART_BaudRate = baudrate;
	USART_Init(USART2, &uart);
	USART_Cmd(USART2, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

}

void Uart2SendChar(char c){
	volatile uint32_t shit = 1000000;
	//while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	while(!((USART2->SR)&USART_SR_TXE));
	//USART_SendData(USART2, c);
	USART2->DR = (uint8_t)c;
}

int __io_putchar(int c)
{
	Uart2SendChar((char)c);
 return c;
}


void Uart2DMATx(char *txt, uint8_t uLength){
	DMA_InitTypeDef dmaconf;
		dmaconf.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));
		dmaconf.DMA_MemoryBaseAddr = (uint32_t)txt;
		dmaconf.DMA_DIR = DMA_DIR_PeripheralDST;
		dmaconf.DMA_BufferSize = uLength;
		dmaconf.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		dmaconf.DMA_MemoryInc = DMA_MemoryInc_Enable;
		dmaconf.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		dmaconf.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		dmaconf.DMA_Mode = DMA_Mode_Normal;
		dmaconf.DMA_Priority = DMA_Priority_Medium;
		dmaconf.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel7 , &dmaconf);
		DMA_Cmd(DMA1_Channel7, ENABLE);
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
		while(!(DMA_GetFlagStatus(DMA1_FLAG_TC7))){};
		USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
		DMA_Cmd(DMA1_Channel7, DISABLE);
}


int _write(int dummy, char *s, int length){
	int ret=0;
/*
	while(ret<length){
		Uart2SendChar(s[ret]);
		ret++;
	}
*/
	Uart2DMATx(s, length);

	ret = length;
	return ret;
}
