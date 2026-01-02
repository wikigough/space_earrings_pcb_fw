/**
 * @file led_control.h
 * @brief High-level twinkle animation and per-LED PWM waveforms.
 */

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>

/**
 * @defgroup LED_CONTROL LED animation control
 * @brief High-level twinkle animation and per-LED PWM waveforms.
 * @{
 */
typedef struct 
{
    uint16_t led1_iter;
    uint16_t led2_iter;
    uint16_t led3_iter;
    uint16_t led4_iter;
    uint16_t led5_iter;
    uint16_t led6_iter;
    uint16_t led7_iter;
    uint16_t led8_iter;
    uint16_t led9_iter;
} LedIters;

typedef struct 
{
    uint8_t led1_active;
    uint8_t led2_active;
    uint8_t led3_active;
    uint8_t led4_active;
    uint8_t led5_active;
    uint8_t led6_active;
    uint8_t led7_active;
    uint8_t led8_active;
    uint8_t led9_active;
} LedActiveTracker;

/**
 * @brief Drive all LEDs with a simple on/off blink pattern (used mainly for testing).
 * @ingroup LED_CONTROL
 */
void simple_blink(void);

/**
 * @brief Initialise twinkle animation state, lookup tables and per-LED iterators.
 * @ingroup LED_CONTROL
 */
void init_twinkle(void);

/**
 * @brief Update one LED with a sinusoidal PWM pattern for the twinkle effect.
 * @ingroup LED_CONTROL
 * @param led_num Index of the LED being updated (1-based).
 * @param iter Pointer to the animation iterator for this specific LED.
 * @param brightness Current logical brightness level or PWM scaling factor - where 255 = 1, 127 = 0.5, etc.
 * @return Returns an "end" bool - if the LED input and its associated iterator have reached the end of the animation instance.
 */
uint8_t sine_single_led(uint8_t led_num, uint16_t *iter, uint8_t brightness);

/**
 * @brief Advance the multi-LED twinkle animation based on the current brightness level. Three LEDs on at a time.
 * @ingroup LED_CONTROL
 * @param brightness Current logical brightness level or PWM scaling factor.
 */
void twinkle_three(uint8_t brightness);

/**
 * @brief Advance the multi-LED twinkle animation based on the current brightness level. Two LEDs on at a time. 
 * @ingroup LED_CONTROL
 * @param brightness Current logical brightness level or PWM scaling factor.
 */
void twinkle_two(uint8_t brightness);

/** @} */
#endif //LED_CONTROL_H
