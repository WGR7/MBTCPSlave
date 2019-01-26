/*
 * W5500register.h
 *
 *  Created on: 25.08.2018
 *      Author: E6430
 */

#ifndef W5500_W5500REGISTER_H_
#define W5500_W5500REGISTER_H_

// Block Select Bits masks definition
#define COMMON_REGISTER	0b00000000
#define SOCKET0_REG		0b00001000
#define SOCKET0_TXBUFF	0b00010000
#define SOCKET0_RXBUFF	0b00011000
#define SOCKET1_REG		0b00101000
#define SOCKET1_TXBUFF	0b00110000
#define SOCKET1_RXBUFF	0b00111000
// ... and so on ...
#define WRITEMODE		0b00000100
#define READMODE		0b00000000
#define OPMODE_VDL		0b00000000
#define OPMODE_FDL1		0b00000001
#define OPMODE_FDL2		0b00000010
#define OPMODE_FDL4		0b00000011

#define BSB_SOCKET_RX(soc) ((soc<<5)|0b00011000)
#define BSB_SOCKET_TX(soc) ((soc<<5)|0b00010000)
#define BSB_SOCKET_REG(soc) ((soc<<5)|0b00001000)


// Common register
#define W5500_MR	0x0000
#define W5500_GAR0	0x0001		// Gateway Address
#define W5500_GAR1	0x0002
#define W5500_GAR2	0x0003
#define W5500_GAR3	0x0004
#define W5500_SUBR0	0x0005		// Subnet Mask Address
#define W5500_SUBR1	0x0006
#define W5500_SUBR2	0x0007
#define W5500_SUBR3	0x0008
#define W5500_SHAR0	0x0009		// Source Hardware Address
#define W5500_SHAR1	0x000A
#define W5500_SHAR2	0x000B
#define W5500_SHAR3	0x000C
#define W5500_SHAR4	0x000D
#define W5500_SHAR5	0x000E
#define W5500_SIPR0	0x000F		// Source IP Address
#define W5500_SIPR1	0x0010
#define W5500_SIPR2	0x0011
#define W5500_SIPR3	0x0012
#define W5500_INTLEVEL0	0x0013	// Interrupt Low Level Timer
#define W5500_INTLEVEL1	0x0014
#define W5500_IR	0x0015		// Interrupt
#define W5500_IMR	0x0016		// Interrupt Mask
#define W5500_SIR	0x0017		// Socket Interrupt
#define W5500_SIMR	0x0018		// Socket Interrupt Mask
#define W5500_RTR0	0x0019		// Retry Time
#define W5500_RTR1	0x001A
#define W5500_RCR	0x001B		// Retry Count
//...
// TO DO: complete the list!
//...
#define W5500_PHYCFGR	0x002E	// PHY Configuration
#define W5500_VERSIONR	0x0039	// Chip version

// Socket register
#define W5500_S_MR			0x0000	// Mode register
#define W5500_S_CR			0x0001
#define W5500_S_IR			0x0002
#define W5500_S_SR			0x0003
#define W5500_S_PORT0		0x0004
#define W5500_S_PORT1		0x0005
#define W5500_S_DHAR0		0x0006
#define W5500_S_DHAR1		0x0007
#define W5500_S_DHAR2		0x0008
#define W5500_S_DHAR3		0x0009
#define W5500_S_DHAR4		0x000A
#define W5500_S_DHAR5		0x000B
#define W5500_S_DIPR0		0x000C
#define W5500_S_DIPR1		0x000D
#define W5500_S_DIPR2		0x000E
#define W5500_S_DIPR3		0x000F
#define W5500_S_DPORT0		0x0010
#define W5500_S_DPORT1		0x0011
#define W5500_S_MSSR0		0x0012
#define W5500_S_MSSR1		0x0013
#define W5500_S_TOS			0x0015
#define W5500_S_TTL			0x0016
#define W5500_S_RXBUF_SIZE	0x001E
#define W5500_S_TXBUF_SIZE	0x001F
#define W5500_S_TX_FSR0		0x0020
#define W5500_S_TX_FSR1		0x0021
#define W5500_S_TX_RD0		0x0022
#define W5500_S_TX_RD1		0x0023
#define W5500_S_TX_WR0		0x0024
#define W5500_S_TX_WR1		0x0025
#define W5500_S_RX_RSR0		0x0026
#define W5500_S_RX_RSR1		0x0027
#define W5500_S_RX_RD0		0x0028
#define W5500_S_RX_RD1		0x0029
#define W5500_S_RX_WR0		0x002A
#define W5500_S_RX_WR1		0x002B
#define W5500_S_IMR			0x002C
#define W5500_S_FRAG0		0x002D
#define W5500_S_FRAG1		0x002E
#define W5500_S_KPALVTR		0x002F

// Socket commands
#define W5500_S_OPEN		0x01
#define W5500_S_LISTEN		0x02
#define W5500_S_CONNECT		0x04
#define W5500_S_DISCON		0x08
#define W5500_S_CLOSE		0x10
#define W5500_S_SEND		0x20
#define W5500_S_SEND_MAC	0x21
#define W5500_S_SEND_KEEP	0x22
#define W5500_S_RECV		0x40

// Socket protocol
#define W5500_SOCKET_P_CLOSED	0x00
#define W5500_SOCKET_P_TCP		0x01
#define W5500_SOCKET_P_UDP		0x02
#define W5500_SOCKET_P_MACRAW	0x04

#endif /* W5500_W5500REGISTER_H_ */
