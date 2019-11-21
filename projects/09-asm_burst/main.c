/***********************************************************************
 * 
 * Implementation of LFSR-based (Linear Feedback Shift Register) 
 * pseudo-random generator in AVR assembly.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-2019 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "gpio.h"
/* Typedef -----------------------------------------------------------*/

/* Define ------------------------------------------------------------*/

/* Variables ---------------------------------------------------------*/

/* Function prototypes -----------------------------------------------*/
extern void burst_asm(uint8_t value);

/* Functions ---------------------------------------------------------*/

/* Main --------------------------------------------------------------*/
/* Generate a sequence of LFSR preudo-random values using 4- and 8-bit
 * structure. */
int main(void)
{
    GPIO_config_output(&DDRB, PB5);
    /* Timer1 */
    TIM_config_prescaler(TIM1, TIM_PRESC_8);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    sei();

    for (;;) {
    }

    return (0);
}

/* Interrupts --------------------------------------------------------*/
/* Timer1 overflow interrupt routine. */
ISR(TIMER1_OVF_vect)
{
    burst_asm(10);
}
