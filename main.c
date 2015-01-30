/* Name: uart.c
 * Author: Jari Tulilahti
 *
 * UART TEST
 */

#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

int main (void)
{
	uint8_t b;
    init_serial();

    tx(0x55);
    tx(0x55);
    tx(0x55);
    tx(0x55);
    tx(0x55);

    for(;;) {
    	while (rx_avail()) {
    		b = rx();
    		tx(b);
    		if (b == 13) tx(10);
    	}
    }

    return 1;
}
