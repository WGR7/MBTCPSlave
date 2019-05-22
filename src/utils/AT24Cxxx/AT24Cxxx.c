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
extern uint8_t port_AT24Cxxx_GenerateStart();
extern void port_AT24Cxxx_GenerateStop();
extern uint8_t port_AT24Cxxx_SendAddress(uint8_t uAddress, uint8_t Read);
extern uint8_t port_AT24Cxxx_SendData(uint8_t uDataByte);
extern int16_t port_AT24Cxxx_RecvData();

#define TRUE 1
#define FALSE 0
#define SEND_ADDR_WRITE 0
#define SEND_ADDR_READ 1

void 	AT24Cxxx_HWSetup(){
	port_AT24Cxxx_HWSetup();
}


int32_t AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer){
	if(uDevAddress>3){
		return AT_WRONG_ADDR;
	}
	if(uLength==0){
		return AT_0_LENGTH;
	}
	if(!puTargetBuffer){
		return AT_BUFFER_ERROR;
	}

	uint8_t retry_count = 0;
	port_AT24Cxxx_ACKSetOn();
	uint16_t uReadBytes=0;

	while(1){
		if(port_AT24Cxxx_GenerateStart()==0){
			//port_AT24Cxxx_GenerateStop();
			return AT_START_TIMEOUT;
		}
		if(port_AT24Cxxx_SendAddress(WRITE_ADDR(uDevAddress), SEND_ADDR_WRITE)==0){
			port_AT24Cxxx_GenerateStop();
			if(++retry_count>RETRY_MAX_COUNT){
				return AT_DEV_ADDR_TIMEOUT;
			}
		}else{
			break;
		}
	}
	if(port_AT24Cxxx_SendData((uint8_t)(uMemAddress>>8))==0){
		port_AT24Cxxx_GenerateStop();
		return AT_SEND_DATA_TIMEOUT;
	}
	if(port_AT24Cxxx_SendData((uint8_t)(uMemAddress&0x00ff))==0){
		port_AT24Cxxx_GenerateStop();
		return AT_SEND_DATA_TIMEOUT;
	}
	port_AT24Cxxx_GenerateStop();
	if(port_AT24Cxxx_GenerateStart()==0){
		return AT_START_TIMEOUT;
	}

	if(port_AT24Cxxx_SendAddress(READ_ADDR(uDevAddress), SEND_ADDR_READ)==0){
		return AT_RECV_TIMEOUT;
	}

	// Apparently don't need to wait for ACK now, go straight to receiving data
	int16_t ret_value = -1;
	for(uint16_t byte_counter = 0; byte_counter<uLength-1; byte_counter++){
		ret_value = port_AT24Cxxx_RecvData();
		if(ret_value<0){
			port_AT24Cxxx_GenerateStop();
			return AT_RECV_TIMEOUT;
		}
		*puTargetBuffer++ = (uint8_t)ret_value;
		uReadBytes++;
	}
	port_AT24Cxxx_ACKSetOff();
	ret_value = port_AT24Cxxx_RecvData();
	if(ret_value<0){
		port_AT24Cxxx_GenerateStop();
		return AT_RECV_TIMEOUT;
	}
	*puTargetBuffer++ = (uint8_t)ret_value;
	port_AT24Cxxx_GenerateStop();
	uReadBytes++;
	return (int32_t)uReadBytes;
}


int8_t AT24Cxxx_PageWriteFromBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puSourceBuffer){
	if(uDevAddress>3){
		return AT_WRONG_ADDR;
	}
	if(uLength==0){
		return AT_0_LENGTH;
	}
	if(!puSourceBuffer){
		return AT_BUFFER_ERROR;
	}

	uint16_t uCurrentWord = uMemAddress;
	uint16_t uLastWord = uCurrentWord + uLength - 1;
	uint8_t retry_count = 0;
	port_AT24Cxxx_ACKSetOn();

	while(1){
		if(port_AT24Cxxx_GenerateStart()==0){
			//port_AT24Cxxx_GenerateStop();
			return AT_START_TIMEOUT;
		}
		if(port_AT24Cxxx_SendAddress(WRITE_ADDR(uDevAddress), SEND_ADDR_WRITE)==0){
			port_AT24Cxxx_GenerateStop();
			if(++retry_count>RETRY_MAX_COUNT){
				return AT_DEV_ADDR_TIMEOUT;
			}
		}else{
			break;
		}
	}
	if(port_AT24Cxxx_SendData((uint8_t)(uMemAddress>>8))==0){
		port_AT24Cxxx_GenerateStop();
		return AT_SEND_DATA_TIMEOUT;
	}
	if(port_AT24Cxxx_SendData((uint8_t)(uMemAddress&0x00ff))==0){
		port_AT24Cxxx_GenerateStop();
		return AT_SEND_DATA_TIMEOUT;
	}
	int8_t iWrittenBytes = 0;
	while(uCurrentWord<=uLastWord){
		if(port_AT24Cxxx_SendData(*(puSourceBuffer++))==0){
			port_AT24Cxxx_GenerateStop();
			return AT_SEND_DATA_TIMEOUT;
		}
		iWrittenBytes++;

		if((++uCurrentWord)%PAGES_SIZE == 0){
			// means we run to the end of page
			break;
		}
	}
	port_AT24Cxxx_GenerateStop();
	return iWrittenBytes;
}

int32_t AT24Cxxx_WriteFromBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puSourceBuffer){
	if(uDevAddress>3){
		return AT_WRONG_ADDR;
	}
	if(uLength==0){
		return AT_0_LENGTH;
	}
	if(!puSourceBuffer){
		return AT_BUFFER_ERROR;
	}

	uint16_t uBytesLeft = uLength;
	uint8_t	*puWorkingPointer = puSourceBuffer;
	uint16_t uMemIndex = uMemAddress;
	while(uBytesLeft > 0){
		int8_t write_result = AT24Cxxx_PageWriteFromBuffer(uDevAddress, uMemIndex, uBytesLeft, puWorkingPointer);
		if(write_result < 1){
			return AT_ERROR;
		}else{
			uBytesLeft -= (uint16_t)write_result;
			puWorkingPointer += (uint8_t)write_result;
			uMemIndex += (uint16_t)write_result;
		}
	}
	return uLength;
}



#ifdef INCLUDE_UNIT_TESTS

uint8_t UT_WriteReadTest(){
	uint8_t retval=0;
	// prepare data to write
#define PATTERN1_ADDR	0
#define PATTERN2_ADDR	20
#define PATTERN3_ADDR	50

	uint8_t source1[] = {0,1,2,3,4,5,6,7,8,9,9,8,7,6,5,4,3,2,1,0};
	uint8_t source2[] = {9,8,7,6,5,4,3,2,1,0,0,1,2,3,4,5,6,7,8,9};
	uint8_t source3[] = {100,101,102,103,104,105,106,107,108,109,110,112,113,114,115,116,117,118,119};
	uint8_t readval1[sizeof(source1)];
	uint8_t readval2[sizeof(source2)];
	uint8_t readval3[sizeof(source3)];
	memset(&readval1, 0x00, sizeof(readval1));
	memset(&readval2, 0x00, sizeof(readval2));
	memset(&readval3, 0x00, sizeof(readval3));

	// write it

	int8_t writeresult1 = -1;
	writeresult1 = AT24Cxxx_PageWriteFromBuffer(DEVICE_ADDRESS, PATTERN1_ADDR, sizeof(source1), source1);
	if(writeresult1!=sizeof(source1)){
		return 1;
	}
	int8_t writeresult2 = -1;
	writeresult2 = AT24Cxxx_PageWriteFromBuffer(DEVICE_ADDRESS, PATTERN2_ADDR, sizeof(source2), source2);
	if(writeresult2!=sizeof(source2)){
		return 2;
	}
	// cross-page write
	int8_t writeresult3 = -1;
	writeresult3 = AT24Cxxx_WriteFromBuffer(DEVICE_ADDRESS, PATTERN3_ADDR, sizeof(source3), source3);
	if(writeresult3!=sizeof(source3)){
		return 3;
	}

	// read it
	int32_t readresult1 = 0;
	int32_t readresult2 = 0;
	int32_t readresult3 = 0;

	readresult1 = AT24Cxxx_ReadToBuffer(DEVICE_ADDRESS, PATTERN1_ADDR, sizeof(source1), readval1);
	readresult2 = AT24Cxxx_ReadToBuffer(DEVICE_ADDRESS, PATTERN2_ADDR, sizeof(source2), readval2);
	readresult3 = AT24Cxxx_ReadToBuffer(DEVICE_ADDRESS, PATTERN3_ADDR, sizeof(source3), readval3);
	// compare it
	if(memcmp(source1, readval1, sizeof(source1))!=0){
		return 4;
	}
	if(memcmp(source2, readval2, sizeof(source2))!=0){
		return 5;
	}
	if(memcmp(source3, readval3, 14)!=0){
		return 6;
	}

	return retval;
}

#endif
