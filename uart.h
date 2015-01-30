#ifndef UART_H
#define UART_H

#include "common.h"

/* Buffer size */
#define RX_BUFFER_SIZE	64

/* Using "UART_TX" as TX pin */
#define UART_PORT 		PORTA
#define UART_DDR    	DDRA
#define UART_PIN		PINA
#define UART_TX_PAD     PA0
#define UART_RX_PAD		PA1
#define UART_REQ_PAD	PA7

/* "Magic" defines for UART */
#define UART_REQ_DIR 	BITPOINTER(UART_DDR, UART_REQ_PAD)
#define UART_TX_DIR		BITPOINTER(UART_DDR, UART_TX_PAD)
#define UART_RX_DIR		BITPOINTER(UART_DDR, UART_RX_PAD)
#define UART_REQ 	 	BITPOINTER(UART_PORT, UART_REQ_PAD)
#define UART_TX 		BITPOINTER(UART_PORT, UART_TX_PAD)
#define UART_RX			BITPOINTER(UART_PIN, UART_RX_PAD)

/* "Magic" defines */
#define COMP_1A_INT		BITPOINTER(TIMSK1, OCIE1A)
#define PC_INT 			BITPOINTER(GIMSK, PCIE0)
#define PC_INT_RX		BITPOINTER(PCMSK0, UART_RX_PAD)
#define COMP_1A_FLAG	BITPOINTER(TIFR1, OCF1A)
#define PC_INT_FLAG		BITPOINTER(GIFR, PCIF0)


#define BAUD        	9600
#define BAUD_DIV    	((F_CPU / BAUD) - 1.5)
#define BAUD_DIV_START  (BAUD_DIV * 1.4)

/* Different states of RX/TX interrupt */
enum UART_STATE {
	TX_START,
	TX_B0,
	TX_B1,
	TX_B2,
	TX_B3,
	TX_B4,
	TX_B5,
	TX_B6,
	TX_B7,
	TX_STOP,
	RX_B0,
	RX_B1,
	RX_B2,
	RX_B3,
	RX_B4,
	RX_B5,
	RX_B6,
	RX_B7,
};

/* Function prototypes */
void init_serial();
void tx(const uint8_t c);
uint8_t rx_avail();
uint8_t rx();

#endif
