


#include "earrings.h"
#include "drivers/gpio.h"
#include "drivers/clock.h"
#include <stdint.h>

void init_earrings(void)
{
    // disable the watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // allow changes to port registers
    PM5CTL0 &= ~LOCKLPM5;

    // initialise clock
    clock_init();

    // initialise gpios
    init_gpios();

}

void run_earrings(void)
{
    uint8_t led_list[9] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED9};
    uint8_t led_port_list[9] = {LED1_PORT, LED2_PORT, LED3_PORT, LED4_PORT, LED5_PORT, LED6_PORT, LED7_PORT, LED8_PORT, LED9_PORT};
    uint16_t i;

    while(1)
    {
        for (i=0; i<9; i++)
        {
            DELAY_US(1000000);
            turn_off_all_leds();
            set_gpio(led_list[i], led_port_list[i]);
        }
        
    }
}