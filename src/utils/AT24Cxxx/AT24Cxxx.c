/*
 * AT24Cxxx.c
 *
 *  Created on: 11.05.2019
 *      Author: E6430
 */


#include "AT24Cxxx.h"

extern void port_AT24Cxxx_HWSetup();
extern uint8_t port_AT24Cxxx_CheckStartConditionOK();
extern uint8_t port_AT24Cxxx_CheckAddressACK();
extern uint8_t port_AT24Cxxx_CheckDataSendACK();
extern void port_AT24Cxxx_ACKSetOn();
extern void port_AT24Cxxx_ACKSetOff();
extern void port_AT24Cxxx_GenerateStart();
extern void port_AT24Cxxx_GenerateStop();
extern void port_AT24Cxxx_SendAddress(uint8_t uAddress);
extern void port_AT24Cxxx_SendData(uint8_t uDataByte);
extern uint8_t port_AT24Cxxx_RecvData();

void 	AT24Cxxx_HWSetup(){
	port_AT24Cxxx_HWSetup();
}

eResult AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer){

}
