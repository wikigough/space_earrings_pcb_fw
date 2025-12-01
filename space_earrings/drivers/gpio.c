/**
 * @file gpio.c
 * @brief GPIO configuration, LED control and switch handling.
 * @ingroup GPIO_DRIVER
 */

#include "drivers/gpio.h"

// private variable declerations
static volatile uint8_t switch_flag = 0;

// private functions
void set_switch_flag(void);

 void init_gpios()
 {
    // configure LEDs as outputs.
    P1DIR |= (LOW_BATT_LED | LED1 | LED2 | LED6 | LED7 | LED8 | LED9);
    P3DIR |= (LED3 | LED4 | LED5);

    // set initial states as low (leds off)
    P1OUT &= ~(LOW_BATT_LED | LED1 | LED2 | LED6 | LED7 | LED8 | LED9);
    P3OUT &= ~(LED3 | LED4 | LED5);

    // configure debug switch
    P4DIR &= ~SW1;
    P4REN = SW1;
    P4OUT = SW1;

    P4IES |= SW1; // low to high transistion for interrupt
    P4IFG &= ~ SW1; // clear flag
    P4IE |= SW1; // Interrupt enable



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

uint8_t read_gpio(uint8_t pin, uint8_t port)
{
    // MSP430FR2355 only has 4 ports: 1, 2, 3, 4.
    uint8_t result = 0; 
    if (port == 1)
    {
        result = P1IN & pin;
    }

    if (port == 2)
    {
        result = P2IN & pin;
    }

    if (port == 3)
    {
        result = P3IN & pin;
    }

    if (port == 4)
    {
        result = P4IN & pin;
    }

    return result;
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

/**
 * @brief Set the software flag indicating that the user switch has been pressed.
 * @ingroup GPIO_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
void set_switch_flag()
{
    switch_flag = 1;
}

void clear_switch_flag()
{
    switch_flag = 0;
    clear_gpio(LED9, LED9_PORT);
}

uint8_t get_switch_flag()
{
    return switch_flag;
}


#pragma vector = PORT4_VECTOR
/**
 * @brief Port 4 interrupt service routine that debounces the switch and flags the event.
 * @ingroup GPIO_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
__interrupt void Port_4_ISR(void)
{
    if (P4IFG & BIT1) {
        set_gpio(LED9, LED9_PORT);          // Toggle LED
        P4IFG &= ~SW1;         // Clear interrupt flag
        set_switch_flag(); // flag for main loop.
        __bic_SR_register_on_exit(LPM0_bits); // wakeup main CPU
    }
}
