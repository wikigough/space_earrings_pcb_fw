/*
 * @file gpio.c
 * @brief Implementation of gpio initialisation functions. 
 */

#include "drivers/gpio.h"

 void init_gpios()
 {
    // configure LEDs as outputs.
    P1DIR |= (LOW_BATT_LED | LED1 | LED2 | LED6 | LED7 | LED8 | LED9);
    P3DIR |= (LED3 | LED4 | LED5);

    // set initial states as low (leds off)
    P1OUT &= ~(LOW_BATT_LED | LED1 | LED2 | LED6 | LED7 | LED8 | LED9);
    P3OUT &= ~(LED3 | LED4 | LED5);

 }

void set_gpio(uint8_t pin, uint8_t port)
{
    // MSP430FR2355 only has 4 ports: 1, 2, 3, 4. 
    if (port == 1)
    {
        P1OUT |= pin;
    }

    if (port == 2)
    {
        P2OUT |= pin;
    }

    if (port == 3)
    {
        P3OUT |= pin;
    }

    if (port == 4)
    {
        P4OUT |= pin;
    }


}

void clear_gpio(uint8_t pin, uint8_t port)
{
    // MSP430FR2355 only has 4 ports: 1, 2, 3, 4. 
    if (port == 1)
    {
        P1OUT &= ~pin;
    }

    if (port == 2)
    {
        P2OUT &= ~pin;
    }

    if (port == 3)
    {
        P3OUT &= ~pin;
    }

    if (port == 4)
    {
        P4OUT &= ~pin;
    }

}

void turn_off_all_leds()
{
    clear_gpio(LED1, LED1_PORT);
    clear_gpio(LED2, LED2_PORT);
    clear_gpio(LED3, LED3_PORT);
    clear_gpio(LED4, LED4_PORT);
    clear_gpio(LED5, LED5_PORT);
    clear_gpio(LED6, LED6_PORT);
    clear_gpio(LED7, LED7_PORT);
    clear_gpio(LED8, LED8_PORT);
    clear_gpio(LED9, LED9_PORT);

}
