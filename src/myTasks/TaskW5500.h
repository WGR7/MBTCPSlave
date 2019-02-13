/*
 * TaskW5500.h
 *
 *  Created on: 20.01.2019
 *      Author: E6430
 */

#ifndef MYTASKS_TASKW5500_H_
#define MYTASKS_TASKW5500_H_

#include "stdlib.h"
#include "FreeRTOS.h"
#include "string.h"
#include "task.h"
#include "stream_buffer.h"
#include "message_buffer.h"
#include "queue.h"
#include "w5500_spi.h"
#include "w5500_spiregisters.h"

#define TCP_SOCKETS_COUNT	1
#define MAX_RX_CHUNK		20		// max bytes per 1 RX data read from socket memory
#define SOCKET_RXBUFFER_LEN	100
#define CONTROL_MESSAGE_BUFFER_LEN 260

typedef enum{
	SOCKET_CLOSED 			= 0x00,
	SOCKET_INIT				= 0x13,
	SOCKET_LISTEN			= 0x14,
	SOCKET_ESTABLISHED		= 0x17,
	SOCKET_CLOSE_WAIT		= 0x1C,
	SOCKET_UDP				= 0x22,
	SOCKET_UNKNOWN_STATE	= 0xFF
}eSocketState;

typedef enum{
	W5500_OFF				= 0x00,
	W5500_INIT				= 0x01,
	W5500_RUN				= 0x02,
	W5500_ERROR				= 0x10
}eW5500State;

typedef enum{
	W5500_ERRORCODE_NOERROR			= 0x00,
	W5500_ERRORCODE_SPIERROR		= 0x01
}eW5500Error;


typedef struct{
	uint8_t 				SocketNo;					// socket number in W5500 socket array (0, 1, ..7)
	uint16_t 				PortNo;						// port number to open/connect to
	eSocketState 			State;						// current state of the socket - as in Sn_SR register
	StreamBufferHandle_t 	RXStreamHandle;				// handler of socket's output stream (RX from W5500)
	uint32_t				StatsTxBytes;				// for statistics
	uint32_t				StatsRxBytes;
}sTCPSocketBlock;

typedef enum{
	W5500_MESSAGE_SEND			= 0x01,
	W5500_MESSAGE_UPDATE_RX		= 0x02,
	W5500_KILL_CONNECTION		= 0x10
}eControlMessageType;

typedef struct{
	uint8_t 			IP[4];
	uint8_t 			Gateway[4];
	uint8_t 			Subnet[4];
	uint8_t 			MAC[6];
	uint8_t				UseDHCP;
	uint16_t			ModbusPortNo;
}sW5500Config;

typedef struct{
	eW5500State				ActualState;
	uint8_t					PhyCfgrStatus;
	uint8_t					ErrorCode;
	sTCPSocketBlock			Sockets[8];
	QueueHandle_t  			ControlMessageQueue;
}sW5500ControlBlock;

typedef struct{
	eControlMessageType		MessageType;
	uint16_t				MessageLength;		// byte count of frame to be sent
	uint8_t					SocketNo;
	void					*DataPointer;
}sW5500ControlMessage;

// globals
sW5500Config 		W5500Conf;
sW5500ControlBlock 	W5500CB;

// streambuffers for sockets
uint8_t Socket0RXStreamStorage[SOCKET_RXBUFFER_LEN+1];
StaticStreamBuffer_t Socket0RXStreamStruct;
StreamBufferHandle_t Socket0RXStreamHandle;


// helper macro for determining if link is up
#define LINK_IS_UP (W5500CB.PhyCfgrStatus & 0x01)

void vTaskW5500(void *pvParameters);
void W5500_ProcessTCPSocket(uint8_t socket_no);
void W5500_ReadoutTCPSocketToStream(uint8_t socket_no);
void W5500_WriteToSocket(uint8_t socket_no, uint8_t *source, uint16_t length);
void W5500_ProcessControlMessages();
void W5500_SetupTCPSocket(sTCPSocketBlock* socket, uint8_t socket_no, uint16_t port_no);
uint8_t W5500_Init(sW5500Config *config);
sW5500Config W5500_DHCPRequest();

#endif /* MYTASKS_TASKW5500_H_ */
