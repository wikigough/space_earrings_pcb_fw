/**
 * @file opamp.h
 * @brief Front-end amplifier and comparator for light sensing.
 */

#ifndef OPAMP_H
#define OPAMP_H

#include <stdint.h>

/**
 * @defgroup OPAMP_DRIVER Op-amp and comparator
 * @brief Front-end amplifier and comparator for light sensing.
 * @{
 */

/**
 * @brief Configure the on-chip SAC / op-amp as a first-stage amplifier for the light sensor.
 * @ingroup OPAMP_DRIVER
 */
void init_opamp(void);

/**
 * @brief Configure the comparator for ambient light detection using the internal DAC reference.
 * @ingroup OPAMP_DRIVER
 */
void init_comp(void);

/**
 * @brief Return whether a low-to-high comparator transition has occurred.
 * @ingroup OPAMP_DRIVER
 * @return Returns transition state bool. HIGH = transition has occurred. 
 */
uint8_t get_comp_low_to_high(void);

/**
 * @brief Return and latch whether a high-to-low comparator transition has occurred.
 * @ingroup OPAMP_DRIVER
 * @return Returns transition state bool. HIGH = transition has occurred. 
 */
uint8_t get_comp_high_to_low(void);

/**
 * @brief Clear the latched low-to-high comparator transition flag.
 * @ingroup OPAMP_DRIVER
 */
void reset_comp_low_to_high(void);

/**
 * @brief Clear the latched high-to-low comparator transition flag.
 * @ingroup OPAMP_DRIVER
 */
void reset_comp_high_to_low(void);

/**
 * @brief Adjust the comparator DAC threshold used to determine the ambient light level.
 * @ingroup OPAMP_DRIVER
 * @param m Multiplier applied to the comparator DAC reference.
 * @note Use this function with caution - it may lead to some spurious results.
 */
void set_dac_multiplier(uint8_t m);

/**
 * @brief Enable comparator interrupts.
 * @ingroup OPAMP_DRIVER
 */
void enable_comp_interrupts(void);

/**
 * @brief Disable comparator interrupts.
 * @ingroup OPAMP_DRIVER
 * @note This is needed, as we want to limit comparator interrupts to only when the interrupts are needed in the brightness measuring function.
 */
void disable_comp_interrupts(void);

/** @} */
#endif //OPAMP_H
