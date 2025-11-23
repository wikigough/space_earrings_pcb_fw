
#include "drivers/clock.h"
#include <stdint.h>
#include "msp430fr2355.h"

// private function decleration
int8_t xtal_init();

// private function decleration
int8_t xtal_init()
{
    // Description: Configure ACLK = XT1 crystal = 32768Hz,
    //               MCLK = DCO + XT1CLK REF = 1MHz,
    //               SMCLK = MCLK = 1MHz.
    // Software trimming not used here, since an accurate clock is not needed. 
    // For more information on software trimming of DCO (which is not here) please see:
    // - examples using software trimming on TI website: https://dev.ti.com/tirex/nodeContent?node=A__AJ6DhSxHmSwvIsn.HM6XYQ__msp430ware__IOGqZri__LATEST&search=MSP430FR2355
    // - the following youtube video for a good explenation on DCO trimming: https://www.youtube.com/watch?v=9Rlr1tghqtc


    // configure port 2 to accept XIN and XOUT
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P2SEL1 |= BIT6 | BIT7;                  // P2.6~P2.7: crystal pins

    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag

    __bis_SR_register(SCG0);                // Disable FLL before adjusting
    CSCTL3 = SELREF__XT1CLK;                // Set XT1 as FLL reference source
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_0;// DCOFTRIM=3, DCO Range = 1MHz
    CSCTL2 = FLLD_0 + 30;                   // DCODIV = 1MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                // Enable FLL after adjusting
    //software_trim();                      // Software Trim to get the best DCOFTRIM value after FLL re-enabled - 
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;  // set ACLK = XT1CLK = 32768Hz
                                               // DCOCLK = MCLK and SMCLK source

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings


}


// public function decleration
void clock_init()
{
    xtal_init();
    enable_millis_timer();
}

// Configure Timer B0 to trigger every 1ms. 
// for some reason cant access Timer A?
void enable_millis_timer()
{
    TB0CCTL0 |= CCIE; // TBCCR0 interrupt enabled
    //327.68 = ~1ms
    TB0CCR0 = 32768;
    TB0CTL = TBSSEL__ACLK | MC__UP; // ACLK, UP mode

}



// Timer0_B0 interrupt service routine
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
{
  P1OUT ^= BIT0;
}
