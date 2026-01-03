# Runtime State Machines

This document sketches the key state machines and sequences in the firmware based on the current implementation.

## Main Application State Machine

```mermaid
stateDiagram-v2
    [*] --> INIT
    INIT --> RUNNING : init_earrings()
    RUNNING --> LOW_BATTERY : battery_good_flag == 0
    LOW_BATTERY --> RUNNING : battery_good_flag == 1
    RUNNING --> SLEEP : enter LPM0
    SLEEP --> RUNNING : timer/comparator/switch interrupt
```

- **INIT**: Hardware and global state are initialised.
- **RUNNING**: Normal operating state; periodic brightness and battery checks plus LED updates.
- **LOW_BATTERY**: Twinkle animation may be disabled and a low-battery LED enabled.
- **SLEEP**: CPU is in low-power mode waiting for interrupts.

## Timer-Driven Animation Sequence

```mermaid
sequenceDiagram
    participant Timer0_B0_ISR as 1 ms Timer ISR
    participant main as run_earrings()
    participant LED as twinkle()/sine_single_led()

    Timer0_B0_ISR->>Timer0_B0_ISR: timer_1ms_flag_set()
    Timer0_B0_ISR->>main: Exit LPM0 (bic_SR_on_exit)
    main->>main: timer_1ms_flag_get()
    alt battery_good_flag == 1
        main->>LED: twinkle(brightness)
        LED->>LED: Update per-LED iterators and GPIOs
    end
    main->>main: timer_1ms_flag_reset()
```

## Battery and Brightness Maintenance Sequence

```mermaid
sequenceDiagram
    participant Timer1_B0_ISR as 1 s Timer ISR
    participant main as run_earrings()
    participant ADC as ADC driver
    participant BATT as batt_low_handler()
    participant BR as brightness_check()

    Timer1_B0_ISR->>Timer1_B0_ISR: timer_1s_flag_set()
    Timer1_B0_ISR->>main: Exit LPM0 (bic_SR_on_exit)
    main->>main: timer_1s_flag_get()
    main->>ADC: is_conversion_ready()
    alt ADC ready
        main->>ADC: get_adc_value()
        main->>BATT: batt_low_handler(battery_voltage)
        main->>BR: brightness_check()
        main->>main: get_scaled_brightness(brightness)
    end
```
