/*
 * TaskW5500.c
 *
 *  Created on: 20.01.2019
 *      Author: E6430
 */

#include "TaskW5500.h"



uint8_t W5500_Init(sW5500Config *config){
	W5500CB.ActualState = W5500_INIT;
	W5500CB.ErrorCode = W5500_ERRORCODE_NOERROR;
	W5500CB.PhyCfgrStatus = 0;
	// 1. HW Reset device for some time
	W5500_RESET_ON;
	vTaskDelay(pdMS_TO_TICKS(50));
	W5500_RESET_OFF;
	vTaskDelay(pdMS_TO_TICKS(50));
	// 2. Verify that actual W5500 is present - check for version register
	uint8_t ucReadValue = W5500_ReadByte(W5500_VERSIONR, COMMON_REGISTER);
	//W5500_ReadRegisters(W5500_VERSIONR, COMMON_REGISTER, 1, &ucReadValue);
	if(ucReadValue == 0x04){
		// proper VERSIONR value
		// 3. Send IP config values
		// 3.a Send MAC
		W5500_WriteRegisters(W5500_SHAR0, COMMON_REGISTER, 6, config->MAC);
		// 3.b Send subnet mask
		W5500_WriteRegisters(W5500_SUBR0, COMMON_REGISTER, 4, config->Subnet);
		// 3.c Send gateway
		W5500_WriteRegisters(W5500_GAR0, COMMON_REGISTER, 4, config->Gateway);
		// 3.d Send IP
		W5500_WriteRegisters(W5500_SIPR0, COMMON_REGISTER, 4, config->IP);
		// 3. Set PHY config and assert SW reset
		//uint8_t phycfgbyte = 0b01111000;
		//W5500_WriteRegisters(W5500_PHYCFGR, COMMON_REGISTER, 1, &phycfgbyte);
		W5500_WriteByte(W5500_PHYCFGR, COMMON_REGISTER, 0b01111000);
		vTaskDelay(pdMS_TO_TICKS(10));
		//phycfgbyte = 0b11111000;
		//W5500_WriteRegisters(W5500_PHYCFGR, COMMON_REGISTER, 1, &phycfgbyte);
		W5500_WriteByte(W5500_PHYCFGR, COMMON_REGISTER, 0b11111000);
		W5500CB.ActualState = W5500_RUN;
		W5500CB.ErrorCode = W5500_ERRORCODE_NOERROR;
		return 1;
	}else{
		W5500CB.ActualState = W5500_ERROR;
		W5500CB.ErrorCode = W5500_ERRORCODE_SPIERROR;
		return 0;
	}
}

void vTaskW5500(void *pvParameters){
	// initial configuration

	// some variables
	uint16_t scratch=0;

	// device init
	W5500_Init(&W5500Conf);
	vTaskDelay(pdMS_TO_TICKS(100));
	// If initialization did not succeed, just loop here
	if(W5500CB.ActualState != W5500_RUN){
		while(1){}
	}

	// config socket
	W5500_SetupTCPSocket(&(W5500CB.Sockets[0]), 0, 502);


	while(1){
		// check if link is up
		//uint8_t tmp=0;
		W5500CB.PhyCfgrStatus = W5500_ReadByte(W5500_PHYCFGR, COMMON_REGISTER);
		//W5500_ReadRegisters(W5500_PHYCFGR, COMMON_REGISTER, 1, &(W5500Status.PhyCfgrStatus));

		if(LINK_IS_UP){
			// sockets maintainance
			W5500_ProcessTCPSocket(0);
			W5500_ReadoutTCPSocketToStream(0);
		}else{

		}

		// control messages processing
		W5500_ProcessControlMessages();

		// This routine every 50ms
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

void W5500_SetupTCPSocket(sTCPSocketBlock* socket, uint8_t socket_no, uint16_t port_no){
	// set socket_no
	socket->SocketNo = socket_no;
	// set port number in W5500
	W5500_WriteWord(W5500_S_PORT0, BSB_SOCKET_REG(socket_no), port_no);
	// and in socket struct
	socket->PortNo = port_no;
	// set protocol - TCP
	W5500_WriteByte(W5500_S_MR, BSB_SOCKET_REG(socket_no), (0b00100000 | W5500_SOCKET_P_TCP));


}

void W5500_ProcessTCPSocket(uint8_t socket_no){
	// check if socket is open and listening or connected
	W5500CB.Sockets[socket_no].State = (eSocketState)W5500_ReadByte(W5500_S_SR, BSB_SOCKET_REG(socket_no));
	switch(W5500CB.Sockets[socket_no].State){
		case SOCKET_CLOSED:
			// clear stream rx buffer...
			xStreamBufferReset(W5500CB.Sockets[socket_no].RXStreamHandle);
			// if close, need to init and start listening
			//uint8_t tmp[] = {W5500_S_OPEN, W5500_S_LISTEN};
			//W5500_WriteRegisters(W5500_S_CR, BSB_SOCKET_REG(socket_no), 1, tmp);
			//W5500_WriteRegisters(W5500_S_CR, BSB_SOCKET_REG(socket_no), 1, tmp+1);
			W5500_WriteByte(W5500_S_CR, BSB_SOCKET_REG(socket_no), W5500_S_OPEN);
			W5500_WriteByte(W5500_S_CR, BSB_SOCKET_REG(socket_no), W5500_S_LISTEN);
			break;
		case SOCKET_CLOSE_WAIT:
			// issue DISCON command
			//uint8_t tmp = W5500_S_DISCON;
			//W5500_WriteRegisters(W5500_S_CR, BSB_SOCKET_REG(socket_no), 1, &tmp);
			W5500_WriteByte(W5500_S_CR, BSB_SOCKET_REG(socket_no), W5500_S_DISCON);
			break;
	}

}

void W5500_ReadoutTCPSocketToStream(uint8_t socket_no){
	if(W5500CB.Sockets[socket_no].State == SOCKET_ESTABLISHED){
		// check if any new data is to be read...
		uint16_t s_rx_rsr = W5500_ReadWord(W5500_S_RX_RSR0, BSB_SOCKET_REG(socket_no));
		while(s_rx_rsr>0){
			// there is smth to read
			// limit s_rx_rsr to MAX_RX_CHUNK - so no more bytes than that will be read
			s_rx_rsr = (s_rx_rsr>MAX_RX_CHUNK)?MAX_RX_CHUNK:s_rx_rsr;
			// s_rx_rsr becomes number of byte that we will actually read
			// read starting adress Sn_RX_RD
			uint16_t s_rx_rd = W5500_ReadWord(W5500_S_RX_RD0, BSB_SOCKET_REG(socket_no));
			// prepare temporary storage for read data, no longer than MAX_RX_CHUNK
			uint8_t temp_storage[MAX_RX_CHUNK];
			memset(temp_storage, 0, MAX_RX_CHUNK);
			// do actual read
			W5500_ReadRegisters(s_rx_rd, BSB_SOCKET_RX(socket_no), s_rx_rsr, temp_storage);
			// increment Sn_RX_RD by the number of bytes read
			s_rx_rd += s_rx_rsr;
			// and update it in device
			W5500_WriteWord(W5500_S_RX_RD0, BSB_SOCKET_REG(socket_no), s_rx_rd);
			// apply RECV command
			W5500_WriteByte(W5500_S_CR, BSB_SOCKET_REG(socket_no), W5500_S_RECV);
			// push temp_storage into byte stream
			xStreamBufferSend(W5500CB.Sockets[socket_no].RXStreamHandle, temp_storage, s_rx_rsr, portMAX_DELAY);
			// refresh s_rx_rsr
			s_rx_rsr = W5500_ReadWord(W5500_S_RX_RSR0, BSB_SOCKET_REG(socket_no));
		}
	}
}

void W5500_ProcessControlMessages(){
	// check if any new messages
	if(uxQueueMessagesWaiting(W5500CB.ControlMessageQueue) > 0){
		sW5500ControlMessage newmessage;

		xQueueReceive(W5500CB.ControlMessageQueue, &newmessage, pdMS_TO_TICKS(100));
		// check type of message

		switch(newmessage.MessageType){
			case W5500_MESSAGE_SEND:
				; // because C
				// <message type> <target socket no> <frame len> < ... actual frame ... >
				// check which socket to write to:
				uint8_t target_socket = newmessage.SocketNo;
				// check length of frame to send
				uint16_t frame_len = newmessage.MessageLength;
				W5500_WriteToSocket(target_socket, (uint8_t*)(newmessage.DataPointer), frame_len);
				// Important! Destroy allocated message data!
				// No! Not really! It should be the message's source bussines to do it, or to not do it
				// free(newmessage.DataPointer);
				break;
			case W5500_MESSAGE_UPDATE_RX:
				; // because C
				break;
			case W5500_KILL_CONNECTION:
				W5500_WriteByte(W5500_S_CR, BSB_SOCKET_REG(newmessage.SocketNo), W5500_S_DISCON);
				xStreamBufferReset(W5500CB.Sockets[newmessage.SocketNo].RXStreamHandle);
				break;
		}

	}
}

void W5500_WriteToSocket(uint8_t socket_no, uint8_t *source, uint16_t length){
	// only if socket is connected
	if(W5500CB.Sockets[socket_no].State == SOCKET_ESTABLISHED){
		// read Sn_TX_WR to know where to write
		uint16_t s_tx_wr = W5500_ReadWord(W5500_S_TX_WR0, BSB_SOCKET_REG(socket_no));
		// do actual write
		W5500_WriteRegisters(s_tx_wr, BSB_SOCKET_TX(socket_no), length, source);
		// advance s_tx_wr
		s_tx_wr += length;
		W5500_WriteWord(W5500_S_TX_WR0, BSB_SOCKET_REG(socket_no), s_tx_wr);
		// apply SEND command
		W5500_WriteByte(W5500_S_CR, BSB_SOCKET_REG(socket_no), W5500_S_SEND);
	}
}
