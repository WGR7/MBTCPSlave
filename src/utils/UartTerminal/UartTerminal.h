/*
 * UartTerminal.h
 *
 *  Created on: 10.05.2019
 *      Author: E6430
 */

#ifndef UTILS_UARTTERMINAL_UARTTERMINAL_H_
#define UTILS_UARTTERMINAL_UARTTERMINAL_H_

#include <inttypes.h>


void Uart2Init(uint32_t	baudrate);
void Uart2SendChar(char);
void Uart2DMATx(char *txt, uint8_t len);

#endif /* UTILS_UARTTERMINAL_UARTTERMINAL_H_ */
