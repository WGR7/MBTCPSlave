/*
 * pindebug.c
 *
 *  Created on: 13.05.2019
 *      Author: E6430
 */

#include "pindebug.h"

void PinDebug_HWConfig(){
	// Prepares 2 output pins to connect to the logic analyzer: PA_0, PA_1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef pins;
	pins.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	pins.GPIO_Speed = GPIO_Speed_50MHz;
	pins.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &pins);
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}
