/*
 * ---------------------------------------------------------------------
 * Author:      Tomas Fryza
 *              Dept. of Radio Electronics, Brno Univ. of Technology
 * Created:     2018-10-16
 * Last update: 2019-10-25
 * Platform:    ATmega328P, 16 MHz, AVR 8-bit Toolchain 3.6.2
 * ---------------------------------------------------------------------
 * Description:
 *    Decimal counter with data output on LCD display.
 * 
 * Note:
 *    Modified version of Peter Fleury's LCD library with R/W pin 
 *    connected to GND. Newline symbol "\n" is not implemented, use
 *    lcd_gotoxy() function instead.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>             // itoa() function
#include <string.h>
#include "timer.h"
#include "lcd.h"

/* Typedef -----------------------------------------------------------*/
/* Define ------------------------------------------------------------*/
#define MAX_VALUE 255
#define BAR_GRAPH 0

/* Variables ---------------------------------------------------------*/
// Custom LCD chars            // First Char
const uint8_t lcd_charset[] = {0b10000,
                               0b10000,
                               0b10000,
                               0b10000,
                               0b10000,
                               0b10000,
                               0b10000,
                               0b10000,
                               // Second Char
                               0b10100,
                               0b10100,
                               0b10100,
                               0b10100,
                               0b10100,
                               0b10100,
                               0b10100,
                               0b10100,
                               // Third Char
                               0b10101,
                               0b10101,
                               0b10101,
                               0b10101,
                               0b10101,
                               0b10101,
                               0b10101,
                               0b10101,
                               // Fourth Char
                               0b00100,
                               0b01010,
                               0b00100,
                               0b11111,
                               0b00100,
                               0b00100,
                               0b01010,
                               0b01010,
                               // Fifth Char
                               0b00000,
                               0b01110,
                               0b11111,
                               0b10101,
                               0b01110,
                               0b11111,
                               0b10101,
                               0b00000};
/* Function prototypes -----------------------------------------------*/

/* Functions ---------------------------------------------------------*/
void lcd_init_custom(void)
{
    lcd_init(LCD_DISP_ON);

    // adding custom symbols
    lcd_command(_BV(LCD_CGRAM));

    // Load custom characters into memory
    for (uint8_t i = 0; i < sizeof(lcd_charset); i++)
    {
        lcd_data(lcd_charset[i]);
    }

    // Clear screen for first use
    lcd_clrscr();
}

void printBargraph(int16_t value)
{
    for(uint8_t i = 1; i <= LCD_DISP_LENGTH; i++)
    {
        lcd_gotoxy(i - 1, 1);

        if (value >= ((MAX_VALUE * i)/LCD_DISP_LENGTH) - 1)
        {
            lcd_putc(0x02);
        }
        else if (value >= ((MAX_VALUE * i)/LCD_DISP_LENGTH) - (MAX_VALUE / LCD_DISP_LENGTH / 3 - 1))
        {
            lcd_putc(0x01);
        }
        else if (value >= ((MAX_VALUE * i)/LCD_DISP_LENGTH) - (MAX_VALUE / LCD_DISP_LENGTH * 2 / 3 - 1))
        {
            lcd_putc(0x00);
        }
        else if (value >= ((MAX_VALUE * i)/LCD_DISP_LENGTH) - (MAX_VALUE / LCD_DISP_LENGTH - 1))
        {
            lcd_putc(' ');
        }
    }
}


/**
 *  Brief:  Main program. Shows decimal values ​​on LCD display.
 *  Input:  None
 *  Return: None
 */
int main(void)
{
    /* LCD display
     * http://homepage.hispeed.ch/peterfleury/avr-software.html
     * Initialize display and test different types of cursor */
    lcd_init_custom();
    lcd_clrscr();

    // Display string without auto linefeed
    lcd_gotoxy(0, 0);
    lcd_puts("Counter:");
    if (BAR_GRAPH == 0)
    {
        lcd_gotoxy(0, 1);
        lcd_putc('$');
        lcd_gotoxy(6, 1);
        lcd_puts("0b");
        lcd_gotoxy(8, 1);
        lcd_puts("00000000");
    }
    lcd_gotoxy(14, 0);
    lcd_putc(0x03);
    lcd_putc(0x04);

    /* Timer 1 configuration for value incrementation */
    TIM_config_prescaler(TIM1, TIM_PRESC_8);
    TIM_config_interrupt(TIM1, TIM_OVERFLOW_ENABLE);

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    for (;;) {
    }

    // Will never reach this
    return (0);
}

/**
 *  Brief: Timer1 overflow interrupt routine. Increment counter value.
 */
ISR(TIMER1_OVF_vect)
{
    static uint16_t value = 0;
    static char lcd_string[9];

    itoa(value, lcd_string, 10);
    lcd_gotoxy(9,0);
    lcd_puts(lcd_string);
    
    if (BAR_GRAPH == 1)
    {
        printBargraph(value);
    }
    else
    {
        itoa(value, lcd_string, 16);
        lcd_gotoxy(1, 1);
        lcd_puts(lcd_string);

        itoa(value, lcd_string, 2);
        lcd_gotoxy(8,1);
        lcd_gotoxy(16 - strlen(lcd_string), 1);
        lcd_puts(lcd_string);
    }

    value++;
    if(value > MAX_VALUE)
    {
        value = 0;
        lcd_gotoxy(9, 0);
        lcd_puts("    ");
        lcd_gotoxy(1, 1);
        lcd_puts("  ");
        lcd_gotoxy(8, 1);
        lcd_puts("00000000");
        if (BAR_GRAPH == 1)
        {
            lcd_gotoxy(0, 1);
            lcd_puts("                ");
        }
    }
}