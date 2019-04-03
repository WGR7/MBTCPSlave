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
	DHCP_init(SOCKETNO_DHCP, workingbuffer);

	uint8_t counter=0;
	while(1){
		state = DHCP_run();
		counter++;
		if(counter>=10){
			counter=0;
			DHCP_time_handler();
		}
		vTaskDelay(pdMS_TO_TICKS(100));

		getIPfromDHCP(current_ip);
		if(current_ip[0]!=0){
			uint8_t dummy=0;
			dummy++;
		}
	}

}
