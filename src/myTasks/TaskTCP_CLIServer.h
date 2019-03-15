/*
 * TaskTCP_CLIServer.h
 *
 *  Created on: 19.02.2019
 *      Author: E6430
 */

#ifndef MYTASKS_TASKTCP_CLISERVER_H_
#define MYTASKS_TASKTCP_CLISERVER_H_

#include "stdlib.h"
#include "FreeRTOS.h"
//#include "string.h"
#include "task.h"

#define CLI_SOCKET_NO 		6
#define INPUT_BUFF_LENGTH	20


typedef struct{
	uint8_t		SocketNumber;
	uint16_t	PortNumber;
}sCLIConfig;

void vTaskTCP_CLIServer(void *pvParameters);

#endif /* MYTASKS_TASKTCP_CLISERVER_H_ */
