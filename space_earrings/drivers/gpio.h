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

/**
 * @defgroup GPIO_DRIVER GPIO driver
 * @brief GPIO configuration, LED control and switch handling.
 * @{
 */
// Pin Definitions
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

// PORT 4
#define SW1                 BIT1

// Port Definitions
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

// PORT 4
#define SW1_PORT             4



/**
 * @brief Initialise GPIO directions, pull configuration and default states for LEDs and switches.
 * @ingroup GPIO_DRIVER
 */
void init_gpios(void); 

/**
 * @brief Drive a GPIO pin high for a given logical pin index and port.
 * @ingroup GPIO_DRIVER
 * @param pin Logical pin enumeration used to look up the GPIO bit.
 * @param port Logical port index corresponding to the MCU GPIO port.
 */
void set_gpio(uint8_t pin, uint8_t port);

/**
 * @brief Drive a GPIO pin low for a given logical pin index and port.
 * @ingroup GPIO_DRIVER
 * @param pin Logical pin enumeration used to look up the GPIO bit.
 * @param port Logical port index corresponding to the MCU GPIO port.
 */
void clear_gpio(uint8_t pin, uint8_t port);

/**
 * @brief Read the logic level of the requested GPIO pin.
 * @ingroup GPIO_DRIVER
 * @param pin Logical pin enumeration used to look up the GPIO bit.
 * @param port Logical port index corresponding to the MCU GPIO port.
 * @return Returns state of GPIO input (HIGH (1) or LOW (0))
 */
uint8_t read_gpio(uint8_t pin, uint8_t port);

/**
 * @brief Turn off all LED outputs: LED1-9.
 * @ingroup GPIO_DRIVER
 */
void turn_off_all_leds(void);

/**
 * @brief Clear the software switch-pressed flag.
 * @ingroup GPIO_DRIVER
 */
void clear_switch_flag(void);

/**
 * @brief Return the current value of the switch-pressed flag.
 * @ingroup GPIO_DRIVER
 * @return Return value as described in the detailed design.
 */
uint8_t get_switch_flag(void);


/** @} */
#endif //DRIVERS_GPIO_H
