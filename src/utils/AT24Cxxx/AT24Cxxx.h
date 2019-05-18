/*
 * AT24Cxxx.h
 *
 *  Created on: 11.05.2019
 *      Author: E6430
 */

#ifndef UTILS_AT24CXXX_AT24CXXX_H_
#define UTILS_AT24CXXX_AT24CXXX_H_

#include <inttypes.h>

// User-selectable device address
#define DEVICE_ADDRESS	3	// a0=1, a1=1

#define ADDRESS_PREFIX	0b10100000
#define WRITE_MASK		0b00000000
#define READ_MASK		0b00000001
#define READ_ADDR(x)	ADDRESS_PREFIX|(x<<1)|READ_MASK
#define WRITE_ADDR(x)	ADDRESS_PREFIX|(x<<1)

// timeouts for transmission error control
#define AFTER_START_TIMEOUT			30		// really 8
#define AFTER_DEV_ADDRESS_TIMEOUT	150		// really 140
#define AFTER_SEND_DATA_TIMEOUT		150		// really 140
#define RECV_DATA_TIMEOUT			100
#define RETRY_DELAY					5000	// will count up to that number before retrying to probe device again
#define RETRY_MAX_COUNT				5		// this many times AT24C will be asked again for ACK

typedef enum {	AT_ERROR = 0,
				AT_OK = !AT_ERROR,
				AT_WRONG_ADDR,
				AT_0_LENGTH,
				AT_BUFFER_ERROR,
				AT_START_TIMEOUT,
				AT_DEV_ADDR_TIMEOUT,
				AT_SEND_DATA_TIMEOUT,
				AT_RECV_TIMEOUT
} eResult;



void 	AT24Cxxx_HWSetup();
//uint8_t	AT24C_RandomRead(uint8_t uDeviceAddr);
eResult AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer);

#endif /* UTILS_AT24CXXX_AT24CXXX_H_ */
