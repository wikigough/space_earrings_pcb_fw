


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

    // init variables for twinkle animation
    init_twinkle();

}

void run_earrings(void)
{
    uint16_t i;

    while(1)
    {
        // go to sleep and wait for interrupt wakeup. 
        __bis_SR_register(LPM0_bits | GIE);          // Enter LPM0 w/ interrupt
        // check if interrupt is the 1ms timer interrupt.  

        if (timer_1ms_flag_get())
        //if(get_switch_flag())
        {
            //simple_blink();

                        
            twinkle();
            //clear_switch_flag();
            timer_1ms_flag_reset();


        }
           
    }
   
}