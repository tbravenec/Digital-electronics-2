/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-11
 * Last update: 2019-10-16
 * Platform:    ATmega328P, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description: Decimal counter with data display on 7-segment display.
 * TODO: Create and test functions to operate a 7-segment display using
 *       shift registers.
 * NOTE: Store segment.c and segment.h files in library/src and 
 *       library/inc folders.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include "gpio.h"
#include "timer.h"
#include "segment.h"
#include <avr/interrupt.h>

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define LED_D1          PB5
#define BTN_S1          PC1 // PCINT 9
#define BTN_S2          PC2 // PCINT 10
#define BTN_S3          PC3 // PCINT 11

/* Variables ---------------------------------------------------------*/
volatile uint16_t value = 0;
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
/**
 *  Brief:  Main program. Shows decimal values ​​on 7-segment display.
 *  Input:  None
 *  Return: None
 */
int main(void)
{
    /* D1 led */
    GPIO_config_output(&DDRB, LED_D1);
    GPIO_write(&PORTB, LED_D1, PIN_LOW);

    /* Pin Change Interrupts 11:9 */
    PCICR |= _BV(PCIE1); 
    PCMSK1 |= (_BV(PCINT11) | _BV(PCINT10) | _BV(PCINT9));

    /* 7-segment display interface */
    GPIO_config_output(&DDRB, SEGMENT_DATA);
    GPIO_config_output(&DDRD, SEGMENT_CLK);
    GPIO_config_output(&DDRD, SEGMENT_LATCH);

    /* Timer 0 configuration for segment switching*/
    TIM_config_prescaler(TIM0, TIM_PRESC_256);
    TIM_config_interrupt(TIM0, TIM_OVERFLOW_ENABLE);

    /* Timer 1 configuration for value incrementation */
    TIM_config_prescaler(TIM1, TIM_PRESC_8);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    /* Enable interrupts by setting the global interrupt mask */
    sei();

    /* Infinite loop */
    for (;;) {
    }

    return (0);
}

/**
 *  Brief: Pin Change Interrupt 11:9 routine. Toggle a LED.
 */
ISR(PCINT1_vect)
{
    GPIO_toggle(&PORTB, LED_D1);
}

ISR(TIMER0_OVF_vect)
{
    static uint8_t segment = 0;
    segment++;
    if(segment >= 4)
    {
        segment = 0;
    }

    uint8_t ones, tens, hundreds, thousands;

    ones = value % 10;
    tens = (value / 10) % 10;
    hundreds = (value / 100) % 10;
    thousands = value / 1000;

    switch(segment)
    {
        case 0:
            SEG_putc(ones, segment);
            break;
        case 1:
            SEG_putc(tens, segment);
            break;
        case 2:
            SEG_putc(hundreds, segment);
            break;
        case 3:
            SEG_putc(thousands, segment);
            break;
        default:
            SEG_putc(0, segment);
    }
}

ISR(TIMER1_OVF_vect)
{
    value++;
    if (value > 9999)
    {
        value = 0;
    }
}