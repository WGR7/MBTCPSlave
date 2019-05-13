/*
 * AT24Cxxx.h
 *
 *  Created on: 11.05.2019
 *      Author: E6430
 */

#ifndef UTILS_AT24CXXX_AT24CXXX_H_
#define UTILS_AT24CXXX_AT24CXXX_H_

// User-selectable device address
#define DEVICE_ADDRESS	3	// a0=1, a1=1

#define ADDRESS_PREFIX	0b10100000
#define WRITE_MASK		0b00000000
#define READ_MASK		0b00000001
#define READ_ADDR(x)	ADDRESS_PREFIX|(x<<1)|READ_MASK
#define WRITE_ADDR(x)	ADDRESS_PREFIX|(x<<1)

typedef enum {ERROR = 0, OK = !ERROR} eResult;

void 	AT24Cxxx_HWSetup();
//uint8_t	AT24C_RandomRead(uint8_t uDeviceAddr);
eResult AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer);

#endif /* UTILS_AT24CXXX_AT24CXXX_H_ */
