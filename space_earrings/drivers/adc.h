/**
 * @file adc.h
 * @brief Battery voltage sampling using the on-chip ADC.
 */

#ifndef ADC_H
#define ADC_H

/**
 * @defgroup ADC_DRIVER ADC driver
 * @brief Battery voltage sampling using the on-chip ADC.
 * @{
 */

#include <stdint.h>

#define VBAT_SENSE_PIN      BIT1
#define VBAT_SENSE_PORT     1

#define BATT_LOW            3100 // 2.5V / 3.3V * 4095

/**
 * @brief Initialise the ADC peripheral to measure battery voltage on the VBAT sense pin.
 * @ingroup ADC_DRIVER
 */
void init_adc(void);

/**
 * @brief Start a new ADC conversion on the configured channel.
 * @ingroup ADC_DRIVER
 */
void adc_start(void);

/**
 * @brief Return the most recent ADC conversion result for the battery voltage.
 * @ingroup ADC_DRIVER
 * @return Returns 12-bit ADC value.
 */
uint16_t get_adc_value(void);

/**
 * @brief Check whether a new ADC conversion result is available.
 * @ingroup ADC_DRIVER
 * @return Return conversion ready bool.
 */
uint8_t is_conversion_ready(void);

/**
 * @brief Clear the ADC conversion-ready flag after the result has been consumed.
 * @ingroup ADC_DRIVER
 */
void clear_conversion_ready(void);

/** @} */
#endif //ADC_H
