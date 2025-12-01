#ifndef EARRINGS_H
#define EARRINGS_H

#include <stdint.h>

void init_earrings(void);

void run_earrings(void);

uint8_t batt_low_handler(uint16_t battery_voltage);

#endif //EARRINGS_H