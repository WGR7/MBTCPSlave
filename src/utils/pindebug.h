/*
 * pindebug.h
 *
 *  Created on: 13.05.2019
 *      Author: E6430
 */

#ifndef UTILS_PINDEBUG_H_
#define UTILS_PINDEBUG_H_

#include "stm32f10x_gpio.h"
#include "app_config.h"

#ifdef USE_PIN_DEBUG
void PinDebug_HWConfig();
#define PINDEBUG_CH_A_ON 	GPIO_SetBits(GPIOA, GPIO_Pin_0);
#define PINDEBUG_CH_A_OFF 	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
#define PINDEBUG_CH_B_ON 	GPIO_SetBits(GPIOA, GPIO_Pin_1);
#define PINDEBUG_CH_B_OFF 	GPIO_ResetBits(GPIOA, GPIO_Pin_1);
#else
#define PINDEBUG_CH_A_ON 	;
#define PINDEBUG_CH_A_OFF 	;
#define PINDEBUG_CH_B_ON 	;
#define PINDEBUG_CH_B_OFF 	;
#endif

#endif /* UTILS_PINDEBUG_H_ */
