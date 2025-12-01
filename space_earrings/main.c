/**
 * @file main.c
 * @brief Program entry point for the LED earrings firmware.
 * @ingroup EARRINGS_APP
 */

#include "earrings.h"

/**
 * @brief Program entry point that initialises the application and enters the main run loop.
 * @ingroup EARRINGS_APP
 * @return Return value as described in the detailed design.
 * @note This is an internal helper; it is not exposed in the public header.
 */
int main(void) {

    init_earrings();

    run_earrings();
}
