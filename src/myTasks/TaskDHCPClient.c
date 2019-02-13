/*
 * TaskDHCPClient.c
 *
 *  Created on: 13.02.2019
 *      Author: E6430
 */

#include "TaskDHCPClient.h"
#include "dhcp.h"

uint8_t workingbuffer[550];

void vTaskDHCPClient(void *pvParameters){
	uint8_t state=0;
	uint8_t current_ip[]={0,0,0,0};
	DHCP_init(DHCP_SOCKET_NO, workingbuffer);

	while(1){

		for(uint8_t counter=0;counter<9;counter++){
			state = DHCP_run();
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		//vTaskDelay(pdMS_TO_TICKS(1000));
		DHCP_time_handler();
		getIPfromDHCP(current_ip);
	}

}
