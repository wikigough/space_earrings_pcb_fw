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
The project has been compiled within CCS Version 20.3.1.5_1.9.1.
 - To build, go to: `Project >> Build Projects`
 - To run, go to: `Run >> Flash Project`
Command line building has not been attempted. 

## Documentation
Run:

```
doxygen Doxyfile
```

Output: `docs/html/index.html`
