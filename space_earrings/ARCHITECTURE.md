# LED Earrings Firmware Architecture

This document gives a high-level overview of the LED earrings firmware architecture as inferred from the source code.

## Modules

- **EARRINGS_APP** (`earrings.c`, `earrings.h`)
  - Owns the top-level application logic.
  - Initialises clocks, GPIO, ADC and the analog front-end.
  - Implements the low-power main loop:
    - Sleeps in LPM0 and wakes on timer and GPIO/comparator interrupts.
    - On every **1 ms tick** it advances the LED twinkle animation (if the battery is healthy).
    - On every **1 s tick** it:
      - Samples the battery voltage via the ADC driver.
      - Updates low-battery state and drives the low-battery indicator LED.
      - Measures ambient brightness using the comparator / op-amp front-end.
      - Updates the global brightness level and derived 8-bit PWM scaling.

- **LED_CONTROL** (`led_control.c`, `led_control.h`)
  - Encapsulates the LED animations.
  - Maintains per-LED iterator state and active/inactive flags.
  - Uses an integer sinusoid lookup table to generate smooth PWM waveforms without floating point.
  - Provides:
    - `twinkle()` for the main twinkling animation.
    - `sine_single_led()` to drive an individual LED along the waveform.
    - Simple blink patterns for testing.

- **ADC_DRIVER** (`drivers/adc.c`, `drivers/adc.h`)
  - Configures the MSP430 ADC to read the battery voltage on the VBAT sense pin.
  - Provides a simple API:
    - `init_adc()`, `adc_start()`, `get_adc_value()`.
    - `is_conversion_ready()`, `clear_conversion_ready()`.
  - Uses an ADC ISR to latch conversion results and flag completion.

- **CLOCK_DRIVER** (`drivers/clock.c`, `drivers/clock.h`)
  - Configures the DCO and crystal source for a 1 MHz MCLK/SMCLK and 32.768 kHz ACLK.
  - Sets up two timer channels:
    - A **1 ms tick** timer for the animation scheduler.
    - A **1 s tick** timer for slower housekeeping tasks (battery and brightness).
  - Exposes flag-get / flag-reset functions used by the main loop instead of busy waiting in ISRs.

- **GPIO_DRIVER** (`drivers/gpio.c`, `drivers/gpio.h`)
  - Owns all pin direction and function configuration for LEDs and the user switch.
  - Provides helpers to:
    - Set / clear individual logical LED pins.
    - Turn off all LEDs.
    - Track a debounced switch flag from the Port 4 ISR.

- **OPAMP_DRIVER** (`drivers/opamp.c`, `drivers/opamp.h`)
  - Configures the SAC/op-amp block as a gain stage for the light sensor.
  - Configures the comparator plus DAC for threshold-based brightness detection.
  - Tracks low-to-high and high-to-low threshold crossings via an ISR and exposes them as latched flags.

## Data Flow Overview

1. **Battery voltage sensing**
   - The 1 s timer tick triggers an ADC conversion.
   - `batt_low_handler()` evaluates the converted voltage against a low-battery threshold.
   - A low-battery LED is driven and a flag disables the twinkle animation if the battery is too low.

2. **Ambient brightness sensing**
   - The comparator front-end monitors the light sensor.
   - `brightness_check()` sweeps DAC thresholds to estimate ambient brightness.
   - The resulting brightness level is mapped by `get_scaled_brightness()` to a 0–255 PWM scaling value.

3. **Animation**
   - On every 1 ms tick, `twinkle()` is called with the current brightness scaling.
   - `twinkle()` calls `sine_single_led()` for the currently active LEDs.
   - Under the hood, integer math is used to index into a sinusoid table and compute on/off windows for GPIO updates.

4. **Low power operation**
   - The main loop spends most of its time in LPM0.
   - Timer and comparator/GPIO interrupts wake the CPU, which performs a small amount of work and returns to sleep.
