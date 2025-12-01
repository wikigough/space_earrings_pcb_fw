# LED Earrings Firmware  
Low-power animation + battery/ambient sensing firmware for MSP430-based LED earrings.

## Overview
This firmware provides:
- LED twinkle engine using timer-driven PWM
- Battery monitoring via ADC
- Ambient light measurement via comparator/op-amp
- 1 ms animation tick and 1 s maintenance tick
- Low-power LPM0 sleep operation

See ARCHITECTURE.md and STATE_MACHINES.md for full system documentation.

## Build Instructions
Using MSP430-GCC:

```
msp430-elf-gcc -mmcu=msp430frxxx -Os -Wall -o firmware.elf src/*.c src/drivers/*.c
```

Flash using mspdebug:

```
mspdebug tilib "prog firmware.elf"
```

## Documentation
Run:

```
doxygen Doxyfile
```

Output: `docs/html/index.html`
