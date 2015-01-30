#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "common.h"
#include "uart.h"


/* Ring buffer global variables */
static volatile uint8_t rx_head = 0, rx_tail = 0;
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];


#define current_byte GPIOR0
#define state GPIOR1


/* Initialize function for UART */
void init_serial() {
    /* Set pin directions */
    UART_TX_DIR = OUT;
    UART_RX_DIR = IN;
    UART_REQ_DIR = OUT;

    /* Set init state of output pins */
    UART_TX = HIGH;
    UART_REQ = HIGH;

    /* Disable interrupts */
    cli();

    /* Compare value for TX */
    OCR1A = BAUD_DIV;

    /* CTC mode & prescaler 1 for Timer1 */
    TCCR1A = 0;
    SET(TCCR1B, WGM12);
    SET(TCCR1B, CS10);

    /* Enable Pin Change interrupt */
    PC_INT = ON;
    PC_INT_RX = ON;

    sei();
}


/* Transmit a byte. Blocking, no buffering */
void tx(const uint8_t c) {
    /* Wait for buffer to empty (timer interrupt to stop) */
    while (COMP_1A_INT);

    /* Push char to buffer and init state */
    current_byte = c;
    state = TX_START;

    /* Enable timer compare interrupt and disable RX PCINT */
    PC_INT_RX = OFF;
    COMP_1A_FLAG = RESET_INT_FLAG;
    COMP_1A_INT = ON;
    TCNT1 = 0;
}


/* Returns number of available bytes in ringbuffer or 0 if empty */
uint8_t rx_avail() {
        return rx_head - rx_tail;
}


/* Store 1 byte in ringbuffer */
static inline void rx_put(const uint8_t c) {
        if (rx_avail() != RX_BUFFER_SIZE) { 
                rx_buffer[rx_head++ % RX_BUFFER_SIZE] = c;
        } 
}


/* Fetch 1 byte from ringbuffer */
uint8_t rx() {
        uint8_t b = 0;
        if (rx_avail() != 0) {
                b = rx_buffer[rx_tail++ % RX_BUFFER_SIZE];
        }
        return b;
}

 
/*
 * Timer compare interrupt vector.
 * This gets called "baud" times per second.
 * Handles TX of the bits.
 */
ISR(TIM1_COMPA_vect) {
    switch (state) {
        case TX_START: // TX Start bit. Stop PCINT RX.
            PC_INT_RX = OFF;
            UART_TX = LOW;
            break;
        case TX_B0: // TX Data bits
        case TX_B1:
        case TX_B2:
        case TX_B3:
        case TX_B4:
        case TX_B5:
        case TX_B6:
        case TX_B7:
            UART_TX = (current_byte & 0x01);
            current_byte >>= 1;
            break;
        case TX_STOP: // TX Stop bit. Enable PCINT for RX again.
            UART_TX = HIGH;
            PC_INT_RX = ON;
            COMP_1A_INT = OFF;
            break;
        case RX_B0: // RX Data bits
            OCR1A = BAUD_DIV;   // Set timer speed to baudrate after RX start bit
            TCNT1 = 0;          // Zero timer and Fall through
        case RX_B1:
        case RX_B2:
        case RX_B3:
        case RX_B4:
        case RX_B5:
        case RX_B6:
        case RX_B7:
            current_byte = (current_byte >> 1) | (UART_RX ? 0x80 : 0x00);
            if (state == RX_B7) {
                rx_put(current_byte);           // Store byte to buffer
                COMP_1A_INT = OFF;              // Stop timer interrupts
                PC_INT_RX = ON;                 // Enable PCINT for RX
                PC_INT_FLAG = RESET_INT_FLAG;   // Clear PC Interrupt flag
            }
            break;
    }
    state++;
}

/* Pin change vector. This gets (hopefully) called when start bit occurs */
ISR(PCINT0_vect) {
    /* If pin is high, we're not interested */
    if (!UART_RX) {
        /* Enable Timer interrupt & Disable Pin Change Interrupt */
        /* Compare value for RX (1.5 bits, sampe in middle of fist data bit) */
        OCR1A = BAUD_DIV_START;
        state = RX_B0;

        /* Enable timer compare interrupt and disable RX PCINT */
        PC_INT_RX = OFF;
        COMP_1A_FLAG = RESET_INT_FLAG; // Clear pending timer interrupts
        COMP_1A_INT = ON;
        TCNT1 = 0;
    }
}
