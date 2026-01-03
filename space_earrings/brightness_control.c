/**
 * @file brightness_control.c
 * @brief Brightness control specific functionalities.
 * @ingroup BRIGHTNESS_CONTROL
 */
#include "drivers/opamp.h"
#include <stdint.h>

 
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

 
uint8_t get_scaled_brightness(uint8_t brightness)
{
    //brightness max = 255 as scaled to 63
    //brightness lowest = 255-(63*3) = 66 as scaled to 0.
    // this is only done once a second so can be relatively inefficient.
    int scaled_brightness = 66 + brightness * 3;
    if (scaled_brightness > 255) scaled_brightness = 255;
    return (uint8_t) scaled_brightness;

}


