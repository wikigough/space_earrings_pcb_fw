
/**
 * @file earrings.c
 * @brief Top-level application loop, battery and brightness management.
 * @ingroup EARRINGS_APP
 */

#include "earrings.h"
#include "drivers/gpio.h"
#include "drivers/clock.h"
#include "drivers/adc.h"
#include "drivers/opamp.h"
#include "led_control.h"
#include "brightness_control.h"
#include <stdint.h>

// private variables
uint8_t batt_low_counter = 0;
uint8_t battery_good_flag = 1;

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

    // initialise low battery detect ADC
    init_adc();

    //init opamp
    init_opamp();

    // init comparator
    init_comp();

    // init variables for twinkle animation
    init_twinkle();

}

void run_earrings(void)
{
    // for battery monitoring functionality
    uint16_t battery_voltage;

    // for using functions within battery_control.h
    static uint8_t brightness = 255; // initial brightness setting, variable changed by photodiode measurement
    static uint8_t brightness_ring_buff[8] = {0}; // ring buff for moving average purposes
    static uint8_t brightness_ring_buff_iter = 0; // ring buff iterator

    while(1)
    {
        // go to sleep and wait for interrupt wakeup. 
        __bis_SR_register(LPM0_bits | GIE);          // Enter LPM0 w/ interrupt
        
        // check if interrupt is the 1ms timer interrupt and we are not in low power mode.
        if (timer_1ms_flag_get() & battery_good_flag)
        {
            twinkle_two(brightness);
            timer_1ms_flag_reset();

        }

        if (timer_1s_flag_get())
        {
            // check battery voltage input - get most recent result if ready
            if (is_conversion_ready())
            {
                battery_voltage = get_adc_value();
                battery_good_flag = batt_low_handler(battery_voltage);

                if (!battery_good_flag)
                {
                    turn_off_all_leds();
                }
                timer_1s_flag_reset();
            }
            
            // execute brightness check to adjust global brightness - comment out if photodiode not connected!
            uint8_t temp_brightness = brightness_check();
            uint8_t temp_brightness_ma = update_ma_size_8(temp_brightness, brightness_ring_buff, &brightness_ring_buff_iter);
            brightness = get_scaled_brightness(temp_brightness_ma);

        }
        //for debug only
        if(get_switch_flag())
        {
            // do something here
            // DEBUG 
           
            // clear switch flag
            clear_switch_flag();
        }

           
    }
   
}

uint8_t batt_low_handler(uint16_t battery_voltage)
{
    uint8_t battery_good = 1; // assume ok to start with
    if (battery_voltage < BATT_LOW)
    {
        batt_low_counter += 1;
    }
    else
    {
        batt_low_counter = 0;
    }


    if (batt_low_counter > 5)
    {
        batt_low_counter -= 1; // to prevent this from overflowing
        set_gpio(LOW_BATT_LED, LOW_BATT_LED_PORT);
        battery_good = 0;
    }
    else {
        clear_gpio(LOW_BATT_LED, LOW_BATT_LED_PORT);
        battery_good = 1;
    }
    clear_conversion_ready();
    adc_start();
    return battery_good;
}

