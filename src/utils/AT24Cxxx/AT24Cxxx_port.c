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

/* 	Timeouts for various sequences of events. This numbers do not mean any specific time, they
*	are just counter values for empty while() loops - so values depend of execution speed, clock
*	setting, other threads etc...
*/
#define AFTER_START_TIMEOUT			1000	// for timing out after issuing start condition
#define AFTER_DEV_ADDRESS_TIMEOUT	1000	// timeout after attempt to address slave device - no ACK after that
#define AFTER_SEND_DATA_TIMEOUT		1000	// timeout after sending data and not getting ACK
#define RECV_DATA_TIMEOUT			1000	// timeout for waiting for data that is supposed to arrive
#define RETRY_DELAY					5000	// will count up to that number before retrying to probe device again, in case of failed addressing



/*
* Interface functions for higher level library
* This function will be called once by interface function AT24Cxxx_HWSetup. Provide a platform specific
* configuration of I2C peripherals
*/
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

/*	Provide a function that will make sure I2C master is issuing ACK after receiving a read data
 */
void port_AT24Cxxx_ACKSetOn(){
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

/*	Provide a function that will make sure I2C master is not issuing ACK after receiving a read data
 */
void port_AT24Cxxx_ACKSetOff(){
	I2C_AcknowledgeConfig(I2C1, DISABLE);
}

/*	Provide a function that will generate a I2C start condition
 * 	Returns:
 * 	0 - when it failed
 * 	1 - when succeeded
 */
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

/*	Provide a function that will generate a I2C stop condition
 */
void port_AT24Cxxx_GenerateStop(){
	I2C_GenerateSTOP(I2C1, ENABLE);
}

/*	Provide a function that will send out a slave address
 * 	Parameters:
 * 	uAddress - address value, raw, as it is to be transmitted
 * 	Read - a flag, when addressing a device for reading, set Read=1, when for writing, set =0
 * 	Returns:
 * 	0 - when it failed/timed out
 * 	1 - when succeeded
 */
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
	return 1;
}

/*	Provide a function that will send out data
 * 	Parameters:
 * 	uDataByte - byte to be transmitted
 * 	Returns:
 * 	0 - when it failed/timed out
 * 	1 - when succeeded
 */
uint8_t port_AT24Cxxx_SendData(uint8_t uDataByte){
	uint16_t timer=0;
	I2C_SendData(I2C1, uDataByte);

	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)==ERROR){
		if(++timer > AFTER_SEND_DATA_TIMEOUT){
			return 0;
		}
	}
	return 1;
}

/*	Provide a function that will return received data
 * 	Parameters:
 * 	Returns:
 * 	-1 - when it failed, or timed out - no data arrived within timeout
 * 	recv. data - when succeeded
 */
int16_t port_AT24Cxxx_RecvData(){
	uint16_t timer=0;
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)==ERROR){
		if(++timer > RECV_DATA_TIMEOUT){
			return -1;
		}
	}
	return (int16_t)I2C_ReceiveData(I2C1);
}
