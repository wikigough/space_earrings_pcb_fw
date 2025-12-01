/**
 * @file earrings.h
 * @brief Top-level application loop, battery and brightness management.
 */

#ifndef EARRINGS_H
#define EARRINGS_H

#include <stdint.h>


/**
 * @defgroup EARRINGS_APP Earrings application
 * @brief Top-level application loop, battery and brightness management.
 * @{
 */
/**
 * @brief Initialise the earrings application, clocks, GPIOs, ADC and analog front-end.
 * @ingroup EARRINGS_APP
 */
void init_earrings(void);

/**
 * @brief Main low-power run loop that drives animations, battery checks and brightness updates.
 * @ingroup EARRINGS_APP
 */
void run_earrings(void);

/**
 * @brief Handle battery-low detection and update internal state and the low-battery indicator LED.
 * @ingroup EARRINGS_APP
 * @param battery_voltage ADC reading of the battery voltage in raw counts.
 * @return Return value as described in the detailed design.
 */

uint8_t batt_low_handler(uint16_t battery_voltage);

#endif //EARRINGS_H
