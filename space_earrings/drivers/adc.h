#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define VBAT_SENSE_PIN      BIT1
#define VBAT_SENSE_PORT     1

#define BATT_LOW            3100 // 2.5V / 3.3V * 4095

void init_adc(void);
void adc_start(void);
uint16_t get_adc_value(void);
uint8_t is_conversion_ready(void);
void clear_conversion_ready(void);

#endif //ADC_H