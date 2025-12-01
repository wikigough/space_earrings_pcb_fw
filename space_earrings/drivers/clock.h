/*
 * @file clock.h
 * 
 * @author vicky
 * @date 2025-11-23
 * @version 0.0
 * 
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

#define MCLK_FREQ_MHZ  1 //clock frequency in MHz

// port 2
#define XOUT        BIT6
#define XIN         BIT7

#define DELAY_US(X) (__delay_cycles(X*MCLK_FREQ_MHZ))

void clock_init(void);

void enable_millis_timer(void);

uint8_t timer_1ms_flag_get(void);

void timer_1ms_flag_reset(void);

uint8_t timer_1ms_flag_get(void);

void timer_1ms_flag_reset(void);

#endif //CLOCK_H