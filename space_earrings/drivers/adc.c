#include "drivers/adc.h"
#include <stdint.h>
#include "msp430fr2355.h"

volatile uint16_t ADC_Result = 0xFFFF; // start full range
volatile uint8_t conversion_ready = 0; // start at not conversion ready

void init_adc()
{
    // Configure ADC A1 pin
    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;

    PM5CTL0 &= ~LOCKLPM5;

    // Configure ADC12
    ADCCTL0 &= ~ADCENC;                                       // Disable ADC for now
    ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
    ADCCTL2 &= ~ADCRES;                                      // clear ADCRES in ADCCTL
    ADCCTL2 |= ADCRES_2;                                     // 12-bit conversion results
    ADCIE |= ADCIE0;                                         // Enable ADC conv complete interrupt
    ADCMCTL0 |= ADCINCH_1 | ADCSREF_0;                        // A1 ADC input select; Vref=DVCC

    // kick off first conversion
    adc_start();

}

void adc_start()
{
    ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start
}

uint16_t get_adc_value()
{
    return ADC_Result;
}

uint8_t is_conversion_ready()
{
    return conversion_ready;
}

void clear_conversion_ready()
{
    conversion_ready = 0;
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:
            break;
        case ADCIV_ADCOVIFG:
            break;
        case ADCIV_ADCTOVIFG:
            break;
        case ADCIV_ADCHIIFG:
            break;
        case ADCIV_ADCLOIFG:
            break;
        case ADCIV_ADCINIFG:
            break;
        case ADCIV_ADCIFG:
            ADC_Result = ADCMEM0;
            conversion_ready = 1;
            break;
        default:
            break;
    }
}