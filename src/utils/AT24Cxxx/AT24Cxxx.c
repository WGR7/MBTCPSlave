/*
 * AT24Cxxx.c
 *
 *  Created on: 11.05.2019
 *      Author: E6430
 */


#include "AT24Cxxx.h"
#include "utils/pindebug.h"

extern void port_AT24Cxxx_HWSetup();
extern uint8_t port_AT24Cxxx_CheckStartConditionOK();
extern uint8_t port_AT24Cxxx_CheckAddressACK();
extern uint8_t port_AT24Cxxx_CheckDataSendACK();
extern uint8_t port_AT24Cxxx_CheckDataReceived();
extern void port_AT24Cxxx_ACKSetOn();
extern void port_AT24Cxxx_ACKSetOff();
extern void port_AT24Cxxx_GenerateStart();
extern void port_AT24Cxxx_GenerateStop();
extern void port_AT24Cxxx_SendAddress(uint8_t uAddress, uint8_t Read);
extern void port_AT24Cxxx_SendData(uint8_t uDataByte);
extern uint8_t port_AT24Cxxx_RecvData();

typedef enum {
	IDLE				= 0,
	STEP1_START			= 1,
	STEP1_START_W		= 2,
	STEP1_DEVADDR		= 3,
	STEP1_DEVADDR_W 	= 4,
	STEP1_RETRY			= 5,
	STEP1_RETRY_W		= 6,
	STEP1_REGADDR1		= 7,
	STEP1_REGADDR1_W	= 8,
	STEP1_REGADDR2		= 9,
	STEP1_REGADDR2_W	= 10,
	STEP1_STOP			= 11,
	STEP2_START			= 20,
	STEP2_START_W		= 21,
	STEP2_RECV			= 22,
	STEP2_RECV_W		= 23,
	STEP2_RECV_LAST		= 24,
	STEP2_STOP			= 25,
	_ERROR				= 100
}eReadStates;

#define TRUE 1
#define FALSE 0
#define SEND_ADDR_WRITE 0
#define SEND_ADDR_READ 1

void 	AT24Cxxx_HWSetup(){
	port_AT24Cxxx_HWSetup();
}

eResult AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer){
	if(uDevAddress>3){
		return AT_WRONG_ADDR;
	}
	if(uLength==0){
		return AT_0_LENGTH;
	}
	if(!puTargetBuffer){
		return AT_BUFFER_ERROR;
	}

	uint16_t timeout = 0;
	uint8_t retry_count = 0;
	port_AT24Cxxx_ACKSetOn();
	uint8_t go_on = TRUE;

	PINDEBUG_CH_A_OFF;
	PINDEBUG_CH_B_OFF;

	do{
		port_AT24Cxxx_GenerateStart();
		PINDEBUG_CH_A_ON;
		while(port_AT24Cxxx_CheckStartConditionOK()==0){
			if(timeout++ > AFTER_START_TIMEOUT){
				port_AT24Cxxx_GenerateStop();
				return AT_START_TIMEOUT;
			}
		}
		PINDEBUG_CH_A_OFF;
		port_AT24Cxxx_SendAddress(WRITE_ADDR(uDevAddress), SEND_ADDR_WRITE);
		PINDEBUG_CH_A_ON;
		timeout = 0;
		go_on = TRUE;
		while(port_AT24Cxxx_CheckAddressACK()==0){
			if(timeout++ > AFTER_DEV_ADDRESS_TIMEOUT){
				port_AT24Cxxx_GenerateStop();
				if(retry_count++>RETRY_MAX_COUNT){return AT_DEV_ADDR_TIMEOUT;}
				for(uint16_t counter=0; counter<RETRY_DELAY; counter++);
				go_on = FALSE;
				break;
			}
		}
		PINDEBUG_CH_A_OFF;
	}while(!go_on);
	port_AT24Cxxx_SendData((uint8_t)(uMemAddress>>8));
	timeout = 0;
	while(port_AT24Cxxx_CheckDataSendACK()==0){
		if(timeout > AFTER_SEND_DATA_TIMEOUT){
			port_AT24Cxxx_GenerateStop();
			return AT_SEND_DATA_TIMEOUT;
		}
	}
	port_AT24Cxxx_SendData((uint8_t)(uMemAddress&0x00ff));
	timeout = 0;
	while(port_AT24Cxxx_CheckDataSendACK()==0){
		if(timeout > AFTER_SEND_DATA_TIMEOUT){
			port_AT24Cxxx_GenerateStop();
			return AT_SEND_DATA_TIMEOUT;
		}
	}
	port_AT24Cxxx_GenerateStop();
	port_AT24Cxxx_GenerateStart();
	timeout = 0;
	while(port_AT24Cxxx_CheckStartConditionOK()==0){
		if(timeout++ > AFTER_START_TIMEOUT){return AT_START_TIMEOUT;}
	}
	port_AT24Cxxx_SendAddress(READ_ADDR(uDevAddress), SEND_ADDR_READ);
	timeout = 0;
	/*
	while(port_AT24Cxxx_CheckAddressACK()==0){
		if(timeout++ > AFTER_DEV_ADDRESS_TIMEOUT){
			return AT_ERROR;
		}
	}*/

	for(uint16_t byte_counter = 0; byte_counter<uLength-1; byte_counter++){
		timeout = 0;
		while(port_AT24Cxxx_CheckDataReceived()==0){
			if(timeout++ > RECV_DATA_TIMEOUT){
				return AT_RECV_TIMEOUT;
			}
		}
		*puTargetBuffer++ = port_AT24Cxxx_RecvData();
	}
	port_AT24Cxxx_ACKSetOff();
	timeout = 0;
	while(port_AT24Cxxx_CheckDataReceived()==0){
		if(timeout++ > RECV_DATA_TIMEOUT){
			return AT_RECV_TIMEOUT;
		}
	}
	*puTargetBuffer = port_AT24Cxxx_RecvData();
	//port_AT24Cxxx_GenerateStop();

	return AT_OK;
	/*
	eResult ret_val = OK;
	eReadStates current_state = STEP1_START;
	eReadStates next_state = IDLE;
	uint8_t uRetryCount = 0;
	while(ret_val){
		switch(current_state){
			case IDLE:
				break;
			case STEP1_START:
				port_AT24Cxxx_ACKSetOn();
				port_AT24Cxxx_GenerateStart();
				next_state = STEP1_START_W;
				break;
			case STEP1_START_W:
				uint8_t timeout = 0;
				while(port_AT24Cxxx_CheckStartConditionOK()==0){
					if(timeout++ > AFTER_START_TIMEOUT){
						next_state = ERROR;
						break;
					}
				}
				next_state = STEP1_DEVADDR;
				break;
			case STEP1_DEVADDR:
				port_AT24Cxxx_SendAddress(WRITE_ADDR(uDevAddress));
				next_state = STEP1_DEVADDR_W;
				break;
			case STEP1_DEVADDR_W:
				uint8_t timeout = 0;
				while(port_AT24Cxxx_CheckAddressACK()==0){
					if(timeout++ > AFTER_DEV_ADDRESS_TIMEOUT){
						next_state = STEP1_RETRY;
						break;
					}
				}
				next_state = STEP1_REGADDR1;
				break;
			case STEP1_RETRY:
				if(uRetryCount++ > RETRY_MAX_COUNT){
					next_state = ERROR;
				}else{
					port_AT24Cxxx_GenerateStop();
					next_state = STEP1_RETRY;
				}
				break;
			case STEP1_RETRY_W:
				uint16_t nop_counter=0;
				while(nop_counter++ < RETRY_DELAY);
				next_state = STEP1_START;
				break;
			case STEP1_REGADDR1:
				port_AT24Cxxx_SendData((uint8_t)(uMemAddress>>8));
				next_state = STEP1_REGADDR1_W;
				break;
			case STEP1_REGADDR1_W:
				uint16_t timeout=0;
				while(port_AT24Cxxx_CheckDataSendACK()==0){
					if(timeout > AFTER_SEND_DATA_TIMEOUT){
						next_state = ERROR;
						break;
					}
				}
				next_state = STEP1_REGADDR2;
				break;
			case STEP1_REGADDR2:
				port_AT24Cxxx_SendData((uint8_t)(uMemAddress&0x00ff));
				next_state = STEP1_REGADDR2_W;
				break;
			case STEP1_REGADDR2_W:
				uint16_t timeout=0;
				while(port_AT24Cxxx_CheckDataSendACK()==0){
					if(timeout > AFTER_SEND_DATA_TIMEOUT){
						next_state = ERROR;
						break;
					}
				}
				next_state = STEP1_STOP;
				break;
			case ERROR:
				port_AT24Cxxx_GenerateStop();
				ret_val = ERROR;
				break;

			default:
				break;
		}
		current_state = next_state;
	}
	return ret_val;*/
}
