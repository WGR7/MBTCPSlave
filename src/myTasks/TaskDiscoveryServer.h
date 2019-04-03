/*
 * TaskDiscoveryServer.h
 *
 *  Created on: 21.02.2019
 *      Author: E6430
 */

#ifndef MYTASKS_TASKDISCOVERYSERVER_H_
#define MYTASKS_TASKDISCOVERYSERVER_H_

#include "stdlib.h"
#include "FreeRTOS.h"
#include "string.h"
#include "task.h"
#include "socket.h"
#include "w5500.h"


#define DISCOVERY_INPUT_BUFF_LEN	30



typedef struct{
	uint8_t		SocketNumber;
	uint16_t	PortNumber;
}sDiscoveryConfig;

void vTaskDiscoveryServer(void *pvParameters);

#endif /* MYTASKS_TASKDISCOVERYSERVER_H_ */
