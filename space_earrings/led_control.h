
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>

void simple_blink(void);
void init_twinkle(void);
void sine_single_led(uint8_t led_num);
void twinkle(void);

#endif //LED_CONTROL_H