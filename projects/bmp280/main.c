/*
 * This is an example on how to use the BMP280 library.
 *
 * This example uses the I2C layer and UART code by Peter Fleury
 * (see the i2chw/ and uart/ subdirectories).
 * 
 * This file is distributable under the terms of the GNU General
 * Public License, version 2 only.
 *
 * Written by Jan "Yenya" Kasprzak, https://www.fi.muni.cz/~kas/
 */

#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"

#define UART_BAUD_RATE 9600
#include "uart.h"

//#include "i2cmaster.h"
#include "bmp280.h"

// this function can also be used as BMP280_DEBUG(name, val) in bmp280.c.
#define UART_BUFLEN 10
void uart_print(char *name, long val)
{
	char debug_buffer[UART_BUFLEN];

        uart_puts(name);
        uart_puts(" = ");

        ltoa((val), debug_buffer, UART_BUFLEN);
        uart_puts(debug_buffer);
        uart_puts("\n\r");
}

int main(void) {
	uart_init(UART_BAUD_SELECT(UART_BAUD_RATE, F_CPU));
	uart_puts("start\n\r");
	// i2c_init(); not needed - called from bmp280_init()
	bmp280_init();

    TIM_config_prescaler(TIM1, 64);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);
	// enable IRQs
	sei();

	while(1) {

		_delay_ms(500);
	}

	return 0;
}

ISR(TIMER1_OVF_vect)
{
    uart_print("status", bmp280_get_status());
    bmp280_measure();
    uart_print("temperature", bmp280_gettemperature()/100);
    uart_print("pressure   ", bmp280_getpressure()/100);
    uart_print("altitude   ", bmp280_getaltitude());
    uart_puts("\n\r");
}