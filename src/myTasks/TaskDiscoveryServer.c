/*
 * TaskDiscoveryServer.c
 *
 *  Created on: 21.02.2019
 *      Author: E6430
 */

#include "TaskDiscoveryServer.h"

void vTaskDiscoveryServer(void *pvParameters){
	sDiscoveryConfig config = *((sDiscoveryConfig*)pvParameters);

	vTaskDelay(pdMS_TO_TICKS(3000));
	// set up socket - assign source address, port...

	while(1){



		// when data received and need to reply, set up a destination address
	}
}

