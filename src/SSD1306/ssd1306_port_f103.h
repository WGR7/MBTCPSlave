/*
 * ssd1306_port_f103.h
 *
 *  Created on: 11.06.2018
 *      Author: E6430
 */

#ifndef SSD1306_SSD1306_PORT_F103_H_
#define SSD1306_SSD1306_PORT_F103_H_

#include "stm32f10x.h"
#include <inttypes.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_spi.h>

#define USE_DMA		1

#define SSD_CS_PIN			GPIO_Pin_2
#define SSD_DC_PIN			GPIO_Pin_0
#define SSD_RES_PIN			GPIO_Pin_3

// Functions that will be given to SSD1306Init
void SSDSelectChip();
void SSDDeSelectChip();
void SSDSetDataMode();
void SSDSetCommandMode();
void SSDSendByte(uint8_t);
void SSDSendBuffer();
void SSDResetActive();
void SSDResetInactive();
void SSDDelay(uint16_t);

#ifdef USE_DMA
void SSDSendDMA(uint8_t *pData, uint16_t uLength);
#endif


#endif /* SSD1306_SSD1306_PORT_F103_H_ */
