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

#define CLOCK_FREQ  1u //clock frequency in MHz

// port 2
#define XOUT        BIT6
#define XIN         BIT7

#define DELAY_US(X) (__delay_cycles(X*CLOCK_FREQ))

void clock_init(void);

void enable_millis_timer(void);

#endif //CLOCK_H