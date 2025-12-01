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

/**
 * @defgroup CLOCK_DRIVER Clock and timers
 * @brief System clock setup and 1 ms / 1 s tick timers.
 * @{
 */

#define MCLK_FREQ_MHZ  2 //clock frequency in MHz

// port 2
#define XOUT        BIT6
#define XIN         BIT7

#define DELAY_US(X) (__delay_cycles(X*MCLK_FREQ_MHZ))

/**
 * @brief Set up the system clocks and DCO to run at MCLK_FREQ_MHZ.
 * @ingroup CLOCK_DRIVER
 * @note software trim not used, as precise clock is not required. 
 */
void clock_init(void);

/**
 * @brief Return the 1 ms tick flag value used by the main loop.
 * @ingroup CLOCK_DRIVER
 * @return Returns flag value.
 */
uint8_t timer_1ms_flag_get(void);

/**
 * @brief Clear the 1 ms tick flag once it has been handled.
 * @ingroup CLOCK_DRIVER
 */
void timer_1ms_flag_reset(void);

/**
 * @brief Return the 1 s tick flag value used by the main loop.
 * @ingroup CLOCK_DRIVER
 * @return Returns flag value.
 */
uint8_t timer_1s_flag_get(void);

/**
 * @brief Clear the 1 s tick flag once it has been handled.
 * @ingroup CLOCK_DRIVER
 */
void timer_1s_flag_reset(void);

/** @} */
#endif //CLOCK_H
