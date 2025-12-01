


#include "earrings.h"
#include "drivers/gpio.h"
#include "drivers/clock.h"
#include "drivers/adc.h"
#include "drivers/opamp.h"
#include <stdint.h>

uint8_t batt_low_counter = 0;
uint8_t battery_good_flag = 1;
uint8_t brightness = 50;
uint16_t scaled_brightness = 255; // max brightness

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
    uint16_t i;
    uint16_t battery_voltage;

    while(1)
    {
        // go to sleep and wait for interrupt wakeup. 
        __bis_SR_register(LPM0_bits | GIE);          // Enter LPM0 w/ interrupt
        
        // check if interrupt is the 1ms timer interrupt and we are not in low power mode.
        if (timer_1ms_flag_get() & battery_good_flag)
        {
            twinkle(brightness);
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

            uint8_t temp_brightness = brightness_check();
            brightness = get_scaled_brightness(temp_brightness);

        }
        //for debug only
        if(get_switch_flag())
        {
            // do something here
            
           
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


uint8_t brightness_check(void)
{
    // firstly clear any comparator flags
    reset_comp_high_to_low();
    reset_comp_low_to_high();
    enable_comp_interrupts();

    uint8_t dac_settings[7] = {24, 20, 16, 12, 8, 4, 0};
    uint8_t i = 0;

    // we want to go from bright to dim - so decrease the DAC setting and look for a high to low transition.
    do{
        set_dac_multiplier(dac_settings[i]);
        i++;
    } while (!get_comp_high_to_low() & (i<6));

    // set dac back to something really high so it is not constantly triggering. 
    set_dac_multiplier(63);
    disable_comp_interrupts();
    return dac_settings[i];

}

uint8_t get_scaled_brightness(uint8_t brightness)
{
    uint8_t scaled_brightness;
    //brightness = 30 is factor 1
    //brightness = 0 is factor 0.8
    // this is only done once a second so can be relatively inefficient.
    if (brightness >= 10)
    {
        scaled_brightness = 255;
    }
    else
    {
        scaled_brightness = 80;
    }
    return scaled_brightness;
}