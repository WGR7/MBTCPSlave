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

// helpers
#define ADDRESS_PREFIX	0b10100000
#define WRITE_MASK		0b00000000
#define READ_MASK		0b00000001
#define READ_ADDR(x)	ADDRESS_PREFIX|(x<<1)|READ_MASK
#define WRITE_ADDR(x)	ADDRESS_PREFIX|(x<<1)

// this many times AT24C will be asked again for ACK after addressing
#define RETRY_MAX_COUNT			5

// AT24 chip page size in bytes
#define PAGES_SIZE				64

// unit tests?
#define INCLUDE_UNIT_TESTS		1

// Values return by library functions
#define	AT_ERROR 				-1
#define	AT_OK 					0
#define	AT_WRONG_ADDR			-2
#define	AT_0_LENGTH				-3
#define	AT_BUFFER_ERROR			-4
#define	AT_START_TIMEOUT		-5
#define	AT_DEV_ADDR_TIMEOUT		-6
#define	AT_SEND_DATA_TIMEOUT	-7
#define	AT_RECV_TIMEOUT			-8

// Library interface functions
void 	AT24Cxxx_HWSetup();
int32_t AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer);
int8_t AT24Cxxx_PageWriteFromBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puSourceBuffer);
int32_t AT24Cxxx_WriteFromBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puSourceBuffer);

#ifdef INCLUDE_UNIT_TESTS
uint8_t UT_WriteReadTest();
#endif

#endif /* UTILS_AT24CXXX_AT24CXXX_H_ */
