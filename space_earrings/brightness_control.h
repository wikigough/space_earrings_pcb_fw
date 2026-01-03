/**
 * @file brightness_control.h
 * @brief Top-level application loop, battery and brightness management.
 */

#ifndef BRIGHTNESS_CONTROL_H
#define BRIGHTNESS_CONTROL_H

#include <stdint.h>

/**
 * @defgroup BRIGHTNESS_CONTROL LED brightness control
 * @brief Functions for checking ambient light level and adjusting LED brightness accordingly.
 * @{
 */

 /**
 * @brief Measure ambient light using the comparator and update the internal brightness level.
 * @ingroup BRIGHTNESS_APP
 * @return Returns the internal DAC setting that feeds into the comparator, which can be scaled later for brightness.
 * @note This is an internal helper; it is not exposed in the public header.
 */
uint8_t brightness_check(void);

/**
 * @brief Maintain a rolling average of the incoming brightness values.
 * @ingroup BRIGHTNESS_APP
 * @param new_item: new item to be placed in the ring_buff array.
 * @param ring_buff: An array passed as a pointer to be updated with the newest new_item value.
 * @param ring_buff_iter: the iterator that is incremented inside this function
 * @return Return the moving average of the new updated buffer.
 * @note *This moving average function only works for buffer size 8.* This is because we want to speed up the division by using bitshifting instead
 *        so we are limited to buffer sizes 2, 4, 8, 16... etc. 
 */
uint8_t update_ma_size_8(uint8_t new_item, uint8_t* ring_buff, uint8_t* ring_buff_iter);


/**
 * @brief Convert the logical brightness level as indicated by the DAC setting into an 8-bit PWM brightness scaling factor.
 * @ingroup BRIGHTNESS_APP
 * @param brightness Current logical brightness level or PWM scaling factor.
 * @return Return scaled value where max DAC of 63 = 255 and min DAC of 0 = 66. 
 * @note This is an internal helper; it is not exposed in the public header.
 */
uint8_t get_scaled_brightness(uint8_t brightness);

/** @} */
#endif //BRIGHTNESS_CONTROL_H
