/*
 * AT24Cxxx.h
 *
 *  Created on: 11.05.2019
 *      Author: E6430
 */

#ifndef UTILS_AT24CXXX_AT24CXXX_H_
#define UTILS_AT24CXXX_AT24CXXX_H_

#include <inttypes.h>

/* User-selectable device address. Corresponds with A0, A1 pins. */
#define DEVICE_ADDRESS	3	// a0=1, a1=1

/* AT24 chip page size in bytes */
#define PAGES_SIZE				64

/* Values return by library functions
 */
#define	AT_ERROR 				-1		/* General error occured			*/
#define	AT_OK 					0		/* No error							*/
#define	AT_WRONG_ADDR			-2		/* Wrong device address				*/
#define	AT_0_LENGTH				-3		/* 0 length read/write requested	*/
#define	AT_BUFFER_ERROR			-4		/* Target/source buffer NULL		*/
#define	AT_START_TIMEOUT		-5		/* \								*/
#define	AT_DEV_ADDR_TIMEOUT		-6		/* | Any of these means				*/
#define	AT_SEND_DATA_TIMEOUT	-7		/* | some timeout occured			*/
#define	AT_RECV_TIMEOUT			-8		/* /								*/

/*****************************************
 ***** Library interface functions *******
 *****************************************
 */

/* \brief Hardware setup
 *
 * Call this function to setup the hardware for I2C operation
 * \param none
 * \return none
 */
void 	AT24Cxxx_HWSetup();

/* \brief Read from EEPROM to buffer
 *
 * Call this function to perform a random position random length read from EEPROM to data buffer
 * \param 	uDevAddress - I2C bus address of EEPROM to read, can be of value 0, 1, 2, 3 only
 * \param 	uMemAddress - starting address to read in EEPROM memory
 * \param 	uLength - how many bytes to read
 * \param 	puTargetBuffer - pointer to a data buffer that is at least uLength long
 * \return 	Number of read bytes or one of the negative return values
 */
int32_t AT24Cxxx_ReadToBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puTargetBuffer);

/* \brief Write to EEPROM from buffer
 *
 * Call this function to perform a random position random length write to EEPROM from data buffer
 * \param 	uDevAddress - I2C bus address of EEPROM to read, can be of value 0, 1, 2, 3 only
 * \param 	uMemAddress - starting address to write in EEPROM memory
 * \param 	uLength - how many bytes to write
 * \param 	puSourceBuffer - pointer to a data buffer that is at least uLength long and contains data to be written
 * \return 	Number of read bytes or one of the negative return values
 */
int32_t AT24Cxxx_WriteFromBuffer(uint8_t uDevAddress, uint16_t uMemAddress, uint16_t uLength, uint8_t *puSourceBuffer);

// unit test?
//#define INCLUDE_UNIT_TESTS

#ifdef INCLUDE_UNIT_TESTS
uint8_t UT_WriteReadTest();
#endif

#endif /* UTILS_AT24CXXX_AT24CXXX_H_ */
