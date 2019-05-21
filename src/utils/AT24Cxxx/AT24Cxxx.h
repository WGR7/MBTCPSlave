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
#define AFTER_START_TIMEOUT			30000		// really 8
#define AFTER_DEV_ADDRESS_TIMEOUT	500		// really 140
#define AFTER_SEND_DATA_TIMEOUT		200		// really 140
#define RECV_DATA_TIMEOUT			200
#define RETRY_DELAY					5000	// will count up to that number before retrying to probe device again
#define RETRY_MAX_COUNT				5		// this many times AT24C will be asked again for ACK

// page size in bytes
#define PAGES_SIZE					64

// unit tests
#define INCLUDE_UNIT_TESTS		1

#define	AT_ERROR 				-1
#define	AT_OK 					0
#define	AT_WRONG_ADDR			-2
#define	AT_0_LENGTH				-3
#define	AT_BUFFER_ERROR			-4
#define	AT_START_TIMEOUT		-5
#define	AT_DEV_ADDR_TIMEOUT		-6
#define	AT_SEND_DATA_TIMEOUT	-7
#define	AT_RECV_TIMEOUT			-8


void 	AT24Cxxx_HWSetup();
//uint8_t	AT24C_RandomRead(uint8_t uDeviceAddr);
int32_t AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer);
int8_t AR24Cxxx_PageWriteFromBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puSourceBuffer);

#ifdef INCLUDE_UNIT_TESTS
uint8_t UT_WriteRead1Page();
#endif

#endif /* UTILS_AT24CXXX_AT24CXXX_H_ */
