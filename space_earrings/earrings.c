
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
#include <stdint.h>

// private variables
uint8_t batt_low_counter = 0;
uint8_t battery_good_flag = 1;

//private functions
uint8_t get_scaled_brightness(uint8_t brightness);
uint8_t brightness_check(void);
uint8_t update_ma_size_8(uint8_t new_item, uint8_t* ring_buff, uint8_t* ring_buff_iter);

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
    uint16_t battery_voltage;
    static uint8_t brightness = 255; // initial brightness setting, variable changed by photodiode measurement
    static uint8_t brightness_ring_buff[8] = {0};
    static uint8_t brightness_ring_buff_iter = 0;

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

/**
 * @brief Measure ambient light using the comparator and update the internal brightness level.
 * @ingroup EARRINGS_APP
 * @return Returns the internal DAC setting that feeds into the comparator, which can be scaled later for brightness.
 * @note This is an internal helper; it is not exposed in the public header.
 */
uint8_t brightness_check(void)
{
    // firstly clear any comparator flags
    reset_comp_high_to_low();
    reset_comp_low_to_high();
    enable_comp_interrupts();

    // only going through some of the values to make it quicker. 
    //uint8_t dac_settings[8] = {28,24, 20, 16, 12, 8, 4, 0};
    //uint8_t dac_settings[8] = {63,55, 47, 39, 31, 23, 15, 7};
    uint8_t size = 17;
    uint8_t dac_settings[17] = {63, 59, 55, 51, 47, 43, 39, 35, 31, 27, 23, 19, 15, 11, 7, 3, 0};
    uint8_t i = 0;

    // we want to go from bright to dim - so decrease the DAC setting and look for a high to low transition.
    do{
        set_dac_multiplier(dac_settings[i]);
        i++;
    } while (!get_comp_high_to_low() & (i<size));

    // set dac back to something really high so it is not constantly triggering. 
    set_dac_multiplier(63);
    disable_comp_interrupts();
    
    if (i==size) return dac_settings[0];
    
    return dac_settings[i];

}


/**
 * @brief Convert the logical brightness level as indicated by the DAC setting into an 8-bit PWM brightness scaling factor.
 * @ingroup EARRINGS_APP
 * @param brightness Current logical brightness level or PWM scaling factor.
 * @return Return value as described in the detailed design.
 * @note This is an internal helper; it is not exposed in the public header. Currently very simple.
 */
uint8_t get_scaled_brightness(uint8_t brightness)
{
    
    //brightness max = 255 as scaled to 63
    //brightness lowest = 255-(63*3) = 66 as scaled to 0
    // this is only done once a second so can be relatively inefficient.
    /*
    uint8_t scaled_brightness; 
    if (brightness >= 32)
    {
        scaled_brightness = 255;
    }
    else
    {
        scaled_brightness = 66;
    }
    return scaled_brightness;
    */
    int scaled_brightness = 66 + brightness * 3;
    if (scaled_brightness > 255) scaled_brightness = 255;
    return (uint8_t) scaled_brightness;

}

/**
 * @brief Maintain a rolling average of the incoming brightness values.
 * @ingroup EARRINGS_APP
 * @param brightness Curre
 * @return Return value as described in the detailed design.
 * @note This is an internal helper; it is not exposed in the public header. Currently very simple.
 */

 uint8_t update_ma_size_8(uint8_t new_item, uint8_t* ring_buff, uint8_t* ring_buff_iter)
 {
    uint8_t ring_buff_size = 8;
    // place new item into the ring_buff with the correct location. 
    ring_buff[*ring_buff_iter] = new_item;

    // update iterator for circular buffer
    *ring_buff_iter += 1;
    if (*ring_buff_iter >= ring_buff_size)
    {
        *ring_buff_iter = 0;
    }

    // get ma of the new buffer.
    int sum = 0;
    int j = 0;
    for (j = 0; j<ring_buff_size; j++)
    {
        sum += ring_buff[j];
    }

    return (uint8_t)(sum >> 3);

 }


