/*
 * TaskDHCPClient.h
 *
 *  Created on: 13.02.2019
 *      Author: E6430
 */

#ifndef MYTASKS_TASKDHCPCLIENT_H_
#define MYTASKS_TASKDHCPCLIENT_H_

#include "stdlib.h"
#include "FreeRTOS.h"
//#include "string.h"
#include "task.h"
//#include "stream_buffer.h"
//#include "message_buffer.h"
//#include "queue.h"

#include "dhcp.h"

#define DHCP_SOCKET_NO			7
#define	MAX_DHCP_RETRY          2        ///< Maximum retry count
#define	DHCP_WAIT_TIME          10       ///< Wait Time 10s
#define DHCP_SERVER_PORT      	67	      ///< DHCP server port number
#define DHCP_CLIENT_PORT         68	      ///< DHCP client port number
#define MAGIC_COOKIE             0x63825363  ///< You should not modify it number.
#define DCHP_HOST_NAME           "MBSlave\0"

void vTaskDHCPClient(void *pvParameters);

#endif /* MYTASKS_TASKDHCPCLIENT_H_ */
