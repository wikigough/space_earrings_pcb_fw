#ifndef OPAMP_H
#define OPAMP_H

#include <stdint.h>

void init_opamp(void);
void init_comp(void);

uint8_t get_comp_low_to_high(void);
uint8_t get_comp_high_to_low(void);
void reset_comp_low_to_high(void);
void reset_comp_high_to_low(void);
void set_dac_multiplier(uint8_t m);

void enable_comp_interrupts(void);
void disable_comp_interrupts(void);

#endif //OPAMP_H
