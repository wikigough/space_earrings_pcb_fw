
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>

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

void simple_blink(void);
void init_twinkle(void);
uint8_t sine_single_led(uint8_t led_num, uint16_t *iter, uint8_t brightness);
void twinkle(uint8_t brightness);

#endif //LED_CONTROL_H