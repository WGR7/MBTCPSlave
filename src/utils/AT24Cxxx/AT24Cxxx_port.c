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

#define AFTER_START_TIMEOUT			10		// really 8
#define AFTER_DEV_ADDRESS_TIMEOUT	150		// really 140
#define AFTER_SEND_DATA_TIMEOUT		150		// really 140
#define RETRY_DELAY					5000	// will count up to that number before retrying to probe device again

typedef enum{
		GENERAL_ERROR 		= 1,
		OK 					= 0,
		START_TIMEOUT 		= 2,
		ADRESSING_TIMEOUT 	= 3,
		DATA_SEND_TIMEOUT 	= 4,
		NOT_READY			= 5,
}eResult;

eResult AT24Cxxx_CheckIfReady(uint8_t DeviceAddress);



uint32_t s_dummycounter=0;

int16_t AT24Cxxx_RandomRead(uint8_t DeviceAddress, uint16_t Register){
	int16_t ret_val=-1;
	DeviceAddress = DeviceAddress << 1;
	DeviceAddress |= 0b10100000;

	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

	I2C_Send7bitAddress(I2C1, DeviceAddress, I2C_Direction_Transmitter);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);

	I2C_SendData(I2C1, (uint8_t)(Register>>8));
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);

	I2C_SendData(I2C1, (uint8_t)(Register&0x00ff));
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);

	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);

	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

	I2C_Send7bitAddress(I2C1, DeviceAddress, I2C_Direction_Receiver);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);

	I2C_AcknowledgeConfig(I2C1, DISABLE);

	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	I2C_GenerateSTOP(I2C1, ENABLE);
	ret_val = I2C_ReceiveData(I2C1);

	return ret_val;
}

eResult AT24Cxxx_ByteWrite(uint8_t DeviceAddress, uint16_t Register, uint8_t Data){
	eResult ret_val = GENERAL_ERROR;

	uint8_t addr = DeviceAddress << 1;
	addr |= 0b10100000;


	while(AT24Cxxx_CheckIfReady(DeviceAddress) != OK){
		for(uint16_t i=0; i<RETRY_DELAY; i++){}
	}
	PINDEBUG_CH_A_ON;
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	I2C_SendData(I2C1, (uint8_t)(Register>>8));
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	I2C_SendData(I2C1, (uint8_t)(Register&0x00ff));
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	I2C_SendData(I2C1, Data);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);

	I2C_GenerateSTOP(I2C1, ENABLE);
	PINDEBUG_CH_A_OFF;
	return ret_val;
}


eResult AT24Cxxx_CheckIfReady(uint8_t DeviceAddress){
	PINDEBUG_CH_B_ON;
	DeviceAddress = DeviceAddress << 1;
	DeviceAddress |= 0b10100000;

	uint16_t timeout=0;
	// generate start condition
	I2C_GenerateSTART(I2C1, ENABLE);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);

	// try to address the device
	timeout=0;
	I2C_Send7bitAddress(I2C1, DeviceAddress, I2C_Direction_Transmitter);
	while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS){
		if(++timeout>AFTER_DEV_ADDRESS_TIMEOUT){
			I2C_GenerateSTOP(I2C1, ENABLE);
			return NOT_READY;
		}
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
	PINDEBUG_CH_B_OFF;
	return OK;
}

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
	i2c.I2C_ClockSpeed = 50000;
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

void port_AT24Cxxx_GenerateStart(){

	I2C_GenerateSTART(I2C1, ENABLE);
}

void port_AT24Cxxx_GenerateStop(){
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void port_AT24Cxxx_SendAddress(uint8_t uAddress, uint8_t Read){
	if(Read == 0){
		I2C_Send7bitAddress(I2C1, uAddress, I2C_Direction_Transmitter);
	}else{
		I2C_Send7bitAddress(I2C1, uAddress, I2C_Direction_Receiver);
	}

}

void port_AT24Cxxx_SendData(uint8_t uDataByte){
	I2C_SendData(I2C1, uDataByte);
}

uint8_t port_AT24Cxxx_RecvData(){
	return I2C_ReceiveData(I2C1);
}
