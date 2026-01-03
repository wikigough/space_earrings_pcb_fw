#include "drivers/opamp.h"
#include "msp430fr2355.h"
#include <stdint.h>

volatile uint8_t comp_detect_low_to_high = 0; // ie. bright to not bright
volatile uint8_t comp_detect_high_to_low = 0; // ie. not bright to bright

void set_comp_low_to_high(void);
void set_comp_high_to_low(void);


void init_opamp(void)
{
    //  SAC2 is set as first stage amplifier in inverting PGA mode and its gain is 1 (external gain available)
    // this has been adapted from here: https://dev.ti.com/tirex/explore/node?devices=MSP430FR2355&isTheia=false&node=A__AJ9qH4DKrOCALeZXX455vw__msp430ware__IOGqZri__LATEST&placeholder=true


    // initialise P3.1 = 0A20, P3.2 = 0A2-, P3.3 = 0A2+: see table 6-65 in datasheet
    // note this is SAC2 - this will be our external opamp
    P3SEL0 |= BIT1 + BIT2 + BIT3;
    P3SEL1 |= BIT1 + BIT2 + BIT3;

    // see Table 20-6 in user guide - selecting "external source selected" for both positive and negative inputs. 
    SAC2OA |= NMUXEN + PMUXEN;
    
    // Select low speed and low power mode
    SAC2OA |= OAPM;

    // enable SAC2 OA and SAC2
    SAC2OA |= OAEN + SACEN;

    // note we cannot initialise SAC0, as we are already using it for LEDs... whoops!
    
}


void init_comp(void)
{
    // inspo from here: https://dev.ti.com/tirex/explore/node?devices=MSP430FR2355&isTheia=false&node=A__AJn3qmVHz3MV34jXS8XUSA__msp430ware__IOGqZri__LATEST&placeholder=true
    // Setup eCOMP: we want DAC as an internal reference to one input of the comparator, then SAC2 as the other. 
    P2DIR |= BIT1;
    P2SEL1 |= BIT1;                           // Select CPOUT function on P2.1/COUT


    // I think we need the internal reference set up first = this is roughly 1.5V.
    PMMCTL0_H = PMMPW_H;                      // Unlock the PMM registers
    PMMCTL2 |= INTREFEN;                      // Enable internal reference
    while(!(PMMCTL2 & REFGENRDY));            // Poll till internal reference settles

    // See table 6-23 of datasheet for CPPSEL
    // P3.1/OA2O is available on CPNSEL = 0b101 only = 0x05
    // this means we want CPPSEL = 0b110 = eCOMP0 6-bit DAC = 0x06
    // UPDATE: We cannot use COMP0.. :( So we are stuck with COMP1, and it doesnt have OA2O as an input. 
    // CAN ONLY USE THIS IF WE HAVE A WIRE LINK BETWEEN OA2O AND P2.4/P2.5 (pins 9 and 10 on the package we are using)
    // currently this is set up as P2.4, but can also be P2.5. 
    // for dim lighting = comparator output high, bright lighting = comparator output low.

    //CP1CTL0 = CPPSEL_6 | CPNSEL_1;             // Select SAC0 and DAC output as eCOMP0 inputs - note SAC0 needs to be fed in via a wire link to P2.4
    CP1CTL0 = CPPSEL_6 | CPNSEL_0;             // Select SAC0 and DAC output as eCOMP0 inputs - with wire link to P2.5.
    CP1CTL0 |= CPPEN | CPNEN;                  // Enable eCOMP inputs
    CP1DACCTL |= CPDACREFS | CPDACEN;          // Select on-chip VREF and enable DAC
    CP1DACDATA |= 20;                          // CPDACBUF1=On-chip VREF * 20/64
    //CP1CTL1 |= CPIIE | CPIE;                   // Enable eCOMP dual edge interrupt
    CP1CTL1 |= CPEN | CPMSEL;                  // Turn on eCOMP, in low power mode
    CP1CTL1 |= CPHSEL1 | CPHSEL0;               // max hysteresis of 30mV. 

}

void enable_comp_interrupts(void)
{
    CP1CTL1 |= CPIIE | CPIE;                   // Enable eCOMP dual edge interrupt
}

void disable_comp_interrupts(void)
{
    CP1CTL1 &= ~ (CPIIE | CPIE);                   // Enable eCOMP dual edge interrupt
}

uint8_t get_comp_low_to_high(void)
{
    return comp_detect_low_to_high;
}

uint8_t get_comp_high_to_low(void)
{
    return comp_detect_high_to_low;
}

void set_comp_low_to_high(void)
{
    comp_detect_low_to_high = 1;
}

void set_comp_high_to_low(void)
{
    comp_detect_high_to_low = 1;
}

void reset_comp_low_to_high(void)
{
    comp_detect_low_to_high = 0;
}

void reset_comp_high_to_low(void)
{
    comp_detect_high_to_low = 0;
}

void set_dac_multiplier(uint8_t m)
{
    if (m < 64)
    {
        CP1DACDATA = m; // note this assumes that DACBUF2 is not used. 
    }
}

// eCOMP interrupt service routine
#pragma vector = ECOMP0_ECOMP1_VECTOR
__interrupt void ECOMP1_ISR(void)
{
    switch(__even_in_range(CP1IV, CPIV__CPIIFG))
    {
        case CPIV__NONE:
            break;
        case CPIV__CPIFG: // The interrupt flag CPIFG is set on a noninverted edge of the eCOMP output. (low -> high) : which is bright to dim. 
            set_comp_low_to_high(); // not used
            break;
        case CPIV__CPIIFG: // The interrupt flag CPIIFG is set on an inverted edge of the eCOMP output (high-> low) : which is dim to bright. 
            set_comp_high_to_low();
            break;
        default:
            break;
    }
}

