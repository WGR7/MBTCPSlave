/*
 * AT24Cxxx_port.c
 *
 *  Created on: 11.05.2019
 *      Author: E6430
 */


#include <stm32f10x_i2c.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "utils/pindebug.h"

#define AFTER_START_TIMEOUT			1000
#define AFTER_DEV_ADDRESS_TIMEOUT	1000
#define AFTER_SEND_DATA_TIMEOUT		1000
#define RECV_DATA_TIMEOUT			1000
#define RETRY_DELAY					5000	// will count up to that number before retrying to probe device again



//
// Interface functions for higher level library
//
void port_AT24Cxxx_HWSetup(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpio;
	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; // SCL, SDA but remaped!
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	gpio.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpio);
	// I2C1_REMAP = 1
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

	I2C_InitTypeDef i2c;
	I2C_StructInit(&i2c);
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_ClockSpeed = 30000;
	I2C_Init(I2C1, &i2c);
	I2C_Cmd(I2C1, ENABLE);

}

uint8_t port_AT24Cxxx_CheckStartConditionOK(){
	return (uint8_t)I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT);
}

uint8_t port_AT24Cxxx_CheckAddressACK(){
	return (uint8_t)I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
}

uint8_t port_AT24Cxxx_CheckDataSendACK(){
	return (uint8_t)I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
}

uint8_t port_AT24Cxxx_CheckDataReceived(){
	return (uint8_t)I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED);
}

void port_AT24Cxxx_ACKSetOn(){
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

void port_AT24Cxxx_ACKSetOff(){
	I2C_AcknowledgeConfig(I2C1, DISABLE);
}

uint8_t port_AT24Cxxx_GenerateStart(){
	uint16_t timer=0;
	I2C_GenerateSTART(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)!=SUCCESS){
		if(++timer>AFTER_START_TIMEOUT){
			return 0;
		}
	}
	return 1;
}

void port_AT24Cxxx_GenerateStop(){
	I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t port_AT24Cxxx_SendAddress(uint8_t uAddress, uint8_t Read){
	uint16_t timer=0;
	if(Read == 0){
		I2C_Send7bitAddress(I2C1, uAddress, I2C_Direction_Transmitter);
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)!=SUCCESS){
			if(++timer>AFTER_DEV_ADDRESS_TIMEOUT){
				return 0;
			}
		}
	}else{
		I2C_Send7bitAddress(I2C1, uAddress, I2C_Direction_Receiver);
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)!=SUCCESS){
			if(++timer>AFTER_DEV_ADDRESS_TIMEOUT){
				return 0;
			}
		}
	}

}

uint8_t port_AT24Cxxx_SendData(uint8_t uDataByte){
	uint16_t timer=0;
	I2C_SendData(I2C1, uDataByte);
	while(port_AT24Cxxx_CheckDataSendACK()==0){
		if(++timer > AFTER_SEND_DATA_TIMEOUT){
			return 0;
		}
	}
	return 1;
}

int16_t port_AT24Cxxx_RecvData(){
	uint16_t timer=0;
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR){
		if(++timer > RECV_DATA_TIMEOUT){
			return -1;
		}
	}
	return (int16_t)I2C_ReceiveData(I2C1);
}
