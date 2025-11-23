/*
 * @file gpio.h
 * 
 * @author vicky
 * @date 2025-11-23
 * @version 0.0
 * 
 * @note Please see schematic for more information. 
 */


#ifndef DRIVERS_GPIO_H
#define DRIVERS_GPIO_H

#include "msp430fr2355.h"
#include <stdint.h>

/*
 * @defgroup pin_definitions
 */

// PORT 1
#define LOW_BATT_LED        BIT0
#define LED1                BIT2
#define LED2                BIT3
#define LED6                BIT4
#define LED7                BIT5
#define LED8                BIT6
#define LED9                BIT7

// PORT 3
#define LED3                BIT5
#define LED4                BIT6
#define LED5                BIT7



/*
 * @defgroup port_definitions
 */

// PORT 1
#define LOW_BATT_LED_PORT    1
#define LED1_PORT            1
#define LED2_PORT            1
#define LED6_PORT            1
#define LED7_PORT            1
#define LED8_PORT            1
#define LED9_PORT            1

// PORT 3
#define LED3_PORT            3
#define LED4_PORT            3
#define LED5_PORT            3

/*
 * @brief GPIO initialisation function. 
 */
void init_gpios(void); 

void set_gpio(uint8_t pin, uint8_t port);

void clear_gpio(uint8_t pin, uint8_t port);

void turn_off_all_leds(void);


#endif //DRIVERS_GPIO_H