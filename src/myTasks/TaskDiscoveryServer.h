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
//#include "string.h"
#include "task.h"

#define DISCOVERY_SOCKET_NO		5

typedef struct{
	uint8_t		SocketNumber;
	uint16_t	PortNumber;
}sDiscoveryConfig;

void vTaskDiscoveryServer(void *pvParameters);

#endif /* MYTASKS_TASKDISCOVERYSERVER_H_ */
