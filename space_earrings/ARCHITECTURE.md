# LED Earrings Firmware Architecture

## Modules

- **EARRINGS_APP** (`earrings.c`, `earrings.h`)
  - Owns the top-level application logic.
  - Initialises clocks, GPIO, ADC and the analog front-end.
  - Implements the low-power main loop:
    - Sleeps in LPM0 and wakes on timer and GPIO/comparator interrupts.
    - On every **0.5 ms tick** it advances the LED twinkle animation (if the battery is healthy).
    - On every **1 s tick** it:
      - Samples the battery voltage via the ADC driver.
      - Updates low-battery state and drives the low-battery indicator LED.
      - Measures ambient brightness using the comparator / op-amp front-end.
      - Updates the global brightness level and derived 8-bit PWM scaling.
    - On every **GPIO interrupt** it executes code for debug purposes only. This section should be left blank except for the clear_switch_flag() function in normal operation.

- **LED_CONTROL** (`led_control.c`, `led_control.h`)
  - Encapsulates the LED animations.
  - Maintains per-LED iterator state and active/inactive flags.
  - Uses an integer sinusoid lookup table to generate smooth PWM waveforms without floating point.
  - Provides:
    - `twinkle_two()` for the main twinkling animation - which has two LEDs twinkling at once.
    - `twinkle_three()` for having three LEDs on at once, but uses more power. 
    - `sine_single_led()` to drive an individual LED along the waveform.
    - Simple blink patterns for testing.

- **BRIGHTNESS_CONTROL** (`brightness_control.c`, `brightness_control.h`)
  - Uses the SAC/op-amp block configured in the OPAMP_DRIVER module to obtain the ambient light level. 
  - Provides function for maintaining a ring buffer for measurements and getting the moving average of the brightness measurements in the ring buffer.
  - scales the op-amp DAC input settings from brightness measurements to PWM duty cycle values for LED_CONTROL.
  
- **ADC_DRIVER** (`drivers/adc.c`, `drivers/adc.h`)
  - Configures the MSP430 ADC to read the battery voltage on the VBAT sense pin.
  - Provides a simple API:
    - `init_adc()`, `adc_start()`, `get_adc_value()`.
    - `is_conversion_ready()`, `clear_conversion_ready()`.
  - Uses an ADC ISR to latch conversion results and flag completion.

- **CLOCK_DRIVER** (`drivers/clock.c`, `drivers/clock.h`)
  - Configures the DCO and crystal source for a 1 MHz MCLK/SMCLK and 32.768 kHz ACLK.
  - Sets up two timer channels:
    - A **0.5 ms tick** timer for the animation scheduler.
    - A **1 s tick** timer for slower housekeeping tasks (battery and brightness).
  - Exposes flag-get / flag-reset functions used by the main loop instead of busy waiting in ISRs.

- **GPIO_DRIVER** (`drivers/gpio.c`, `drivers/gpio.h`)
  - Owns all pin direction and function configuration for LEDs and the user switch.
  - Provides helpers to:
    - Set / clear individual logical LED pins.
    - Turn off all LEDs.
    - Track a debounced switch flag from the Port 4 ISR.

- **OPAMP_DRIVER** (`drivers/opamp.c`, `drivers/opamp.h`)
  - Configures the SAC/op-amp block as a gain stage for the photodiode.
  - Configures the comparator plus DAC for threshold-based brightness detection.
  - Tracks low-to-high and high-to-low threshold crossings via an ISR and exposes them as latched flags.

## Data Flow Overview

1. **Low power operation**
   - The main loop spends some of its time in LPM0.
   - Timer and comparator/GPIO interrupts wake the CPU, which performs a small amount of work and returns to sleep.

2. **Animation**
   - On every 0.5 ms tick, `twinkle_two()` is called with the current brightness scaling, with the brightness set as maximum to start with as default.
   - `twinkle_two()` calls `sine_single_led()` for the currently active LEDs.
   - Under the hood, integer math is used to index into a sinusoid table and compute on/off windows for GPIO updates.

3. **Battery voltage sensing**
   - The 1 s timer tick triggers an ADC conversion.
   - `batt_low_handler()` evaluates the converted voltage against a low-battery threshold.
   - A low-battery LED is driven and a flag disables the twinkle animation if the battery is too low.

4. **Ambient brightness sensing**
   - The comparator front-end monitors the light sensor.
   - `brightness_check()` sweeps DAC thresholds to estimate ambient brightness.
   - The resulting brightness level is mapped by `get_scaled_brightness()` to a 0–255 PWM scaling value.

