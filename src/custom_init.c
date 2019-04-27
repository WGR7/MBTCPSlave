/*
 * custom_init.c
 *
 *  Created on: 14.08.2018
 *      Author: E6430
 */
#include "custom_init.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
#include "misc.h"

/**
  * @brief  Configures all the hardware for custom needs
  * @param  None
  * @retval None
  */
void CustomInit(void){
	CustomClockConfigHSI64();
	//CustomGPIOConfig();
	CustomSPIConfig();

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);


	// Nucleo LD2 user led config - PA5 (GPIOA @APB2)
	/* PA5 will be used for SPI1 SCK!
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef ledconf;
	ledconf.GPIO_Mode = GPIO_Mode_Out_PP;
	ledconf.GPIO_Pin = GPIO_Pin_5;
	ledconf.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &ledconf);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	 */

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	//64MHz AHB -> /8
	// 80000 ticks 	-> 100Hz SysTick
	// 800000 ticks -> 10Hz SysTicks
	SysTick_Config(80000);


}

/**
  * @brief  Configures the System clock frequency to use HSI with PLL*16
  * so that SYSCLK = HCLK = PCLK2 = APB2CLK = 64MHz
  * and PCLK1 = APB1CLK = 32MHz
  * @param  None
  * @retval None
  */
void CustomClockConfigHSI64(void){
	RCC_DeInit();
	RCC_HSICmd(ENABLE);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
	RCC_PLLCmd(ENABLE);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div2);

	//apparently necessary
	FLASH_SetLatency(FLASH_Latency_2);

	// update SystemCoreClock variable
	SystemCoreClockUpdate();
}

void CustomSPIConfig(void){
	// SPI configuration:
	// PA_5 - SCK
	// PA_6 - MISO
	// PA_7 - MOSI
	// PC_0 - CS for Wiznet
	// PC_1 - RESET for Wiznet
	// PC_2 - CS for SSD1306
	// PC_3 - RESET for SSD1306
	// PA_15 - Data/Command for SSD1306
	// CPOL=1, CPHA=1
	// 8bit frame

	// Set clock to SPI1 interface @APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	// Set clock to AFIO module
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	// Set clock to PA pins - SPI pins
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// Set clock to PC pins - Chip select
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//Configure PA_5/6/7 as output 50MHz pushpull alternate function. Although why PA_6 as output when it is Master Input???
	GPIO_InitTypeDef pins;
	pins.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	pins.GPIO_Speed = GPIO_Speed_50MHz;
	pins.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &pins);

	// Initially set SCK to starting state (HIGH)
	GPIO_SetBits(GPIOA, GPIO_Pin_5);

	// Configure PC0, PC1, PC2, PC4 as output 50MHz push-pull
	pins.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	pins.GPIO_Speed = GPIO_Speed_50MHz;
	pins.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &pins);
	// Initially set to HIGH - chip not selected, no reset
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	GPIO_SetBits(GPIOC, GPIO_Pin_1);
	GPIO_SetBits(GPIOC, GPIO_Pin_2);
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOC, GPIO_Pin_15);

	// Configure PA0 as output 50MHz push-pull
	pins.GPIO_Pin = GPIO_Pin_0;
	pins.GPIO_Speed = GPIO_Speed_50MHz;
	pins.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &pins);

	// Initially set to HIGH - chip not selected, no reset
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	GPIO_SetBits(GPIOC, GPIO_Pin_1);
	GPIO_SetBits(GPIOC, GPIO_Pin_2);
	GPIO_SetBits(GPIOC, GPIO_Pin_3);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);

	// Configure SPI baudrate - PCLK2(64MHz) /32 = 2MHz
	SPI_InitTypeDef spiconf;
	spiconf.SPI_Mode = SPI_Mode_Master;
	spiconf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	spiconf.SPI_CPOL = SPI_CPOL_High;
	spiconf.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_Init(SPI1, &spiconf);
	// SPI enable
	SPI_Cmd(SPI1, ENABLE);

}

/**
  * @brief  Configures the GPIOs, its APB clocks, and interrupts
  * @param  None
  * @retval None
  */
void CustomGPIOConfig(void){
	//  -------------------------------------------------------------
	//  ------------- GPIO port config (port C @APB2) ---------------
	//  -------------------------------------------------------------

	//  --------- Clock for GPIO port C (APB2) -----------
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	//  --------- Clock for GPIO port A (APB2) -----------
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;


	//  --------- PC13 Pin mode - both bits cleared - input mode -----------
	GPIOC->CRH &= ~GPIO_CRH_MODE13;
	//  --------- PC0 Pin mode - both bits set - output mode 50MHz -----------
	GPIOC->CRL |= GPIO_CRL_MODE0;
	//  --------- PA5 Pin mode - both bits set - output mode 50MHz -----------
	// ---------- Comment this when using SPI1 -------------------------------
	GPIOA->CRL |= GPIO_CRL_MODE5;

	//  -------------- PC13 Pin config - 01 - floating input ---------------
	GPIOC->CRH &= ~GPIO_CRH_CNF13;
	GPIOC->CRH |= GPIO_CRH_CNF13_0;
	//  -------------- PC0 Pin config - 00 - push-pull output ---------------
	GPIOC->CRL &= ~GPIO_CRL_CNF0;
	//  ------------- Immidiately set to high -----------------
	GPIOC->BSRR |= GPIO_BSRR_BS0;

	//  -------------- PA5 Pin config - 00 - push-pull output ---------------
	//  -------------- Comment it when using SPI1!
	//GPIOA->CRL &= ~GPIO_CRL_CNF5;
	//  ------------- Immidiately set to low -----------------
	//GPIOA->BSRR |= GPIO_BSRR_BR5;



	//  -------------------------------------------------------------
	//  ---------------- AFIO for interrupt config ------------------
	//  -------------------------------------------------------------

	//  -------- Clock for AFIO interface (APB2) ---------
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	// -------- Select and configure EXTI13 line ---------
	// -------- in EXTICR4 (array index 3!)
	// -------- 0010: PC[13] pin
	AFIO->EXTICR[3] &= ~AFIO_EXTICR4_EXTI13;
	AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI13_PC;

	//  -------------------------------------------------------------
	//  -------------------- Interrupt config -----------------------
	//  -------------------------------------------------------------

	// -------- Interrupt masks ---------
	EXTI->IMR |= EXTI_IMR_MR13;

	// -------- Edge detection selection - falling ---------
	EXTI->FTSR |= EXTI_FTSR_TR13;

	// -------- Activate EXTI15_10_IRQn interrupt ---------
	// -------- EXTI15_10_IRQn falls into ISER[1], and it's
	// -------- correspondong bit is bit[8]
	// -------- In general, as in NVIC_EnableIRQ(IRQn_Type IRQn):
	// -------- NVIC->ISER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
	NVIC->ISER[1] |= 0x0100;

}


