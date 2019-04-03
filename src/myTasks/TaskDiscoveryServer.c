/*
 * TaskDiscoveryServer.c
 *
 *  Created on: 21.02.2019
 *      Author: E6430
 */

#include "TaskDiscoveryServer.h"

const uint8_t uiDiscoveryPhrase[] = {"hi?\0"};
const uint8_t uiReplyPhrase[] = {"ho!\0"};

void vTaskDiscoveryServer(void *pvParameters){
	sDiscoveryConfig config = *((sDiscoveryConfig*)pvParameters);
	uint8_t	uiInputBuffer[DISCOVERY_INPUT_BUFF_LEN+1];
	uint8_t remote_ip[] = {0x00, 0x00, 0x00, 0x00};
	uint16_t remote_port = 0;
	uint8_t newdatacount = 0;
	//wiz_NetInfo netinfo;
	//uint8_t my_ip[] = {0x00, 0x00, 0x00, 0x00};
	//uint8_t my_mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t reply[sizeof(uiReplyPhrase)/*+6+4*/];

	strcpy(reply, uiReplyPhrase);

	vTaskDelay(pdMS_TO_TICKS(3000));
	// set up socket - assign source address, port, and open it
	socket(config.SocketNumber, Sn_MR_UDP, config.PortNumber,0x00);



	while(1){
		vTaskDelay(pdMS_TO_TICKS(1000));
		// check if something was received
		// newdatacount includes 8 bytes of source info: 4 bytes of sender IP, 2 bytes of source port no, and 2 bytes of datagram payload length
		getsockopt(config.SocketNumber, SO_RECVBUF, &newdatacount);
		if(newdatacount){
			// prepare buffer
			memset(uiInputBuffer, 0x00, sizeof(uiInputBuffer));
			// fetch data to uiInputBuffer

			recvfrom(config.SocketNumber, uiInputBuffer, DISCOVERY_INPUT_BUFF_LEN, remote_ip, &remote_port);
			// check if it is a discovery request - must start with certain phrase
			if(!strcmp(uiInputBuffer, uiDiscoveryPhrase)){
				// compare match - discovery request! Need to send back own IP address
				//wizchip_getnetinfo(&netinfo);
				// assemble reply - first part already filled in - the phrase
				//memcpy(reply+sizeof(uiReplyPhrase) ,netinfo.ip, sizeof(netinfo.ip));
				//memcpy(reply+sizeof(uiReplyPhrase)+sizeof(netinfo.ip), netinfo.mac, sizeof(netinfo.mac));
				// send it
				sendto(config.SocketNumber, reply, sizeof(reply), remote_ip, remote_port);

			}else{
				// do nothing
			}

		}


		// when data received and need to reply, set up a destination address
	}
}

