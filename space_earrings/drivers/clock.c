
#include "drivers/clock.h"
#include <stdint.h>
#include "msp430fr2355.h"

// private variable declerations
static volatile uint8_t timer_1ms_flag = 0;
static volatile uint8_t timer_1s_flag = 0;

// private function decleration
void xtal_init();
void timer_1ms_flag_set();
void timer_1s_flag_set();
void enable_millis_timer();
void enable_second_timer();

// private function decleration
void xtal_init()
{
    // Description: Configure ACLK = XT1 crystal = 32768Hz,
    //               MCLK = DCO + XT1CLK REF = 1MHz,
    //               SMCLK = MCLK = 1MHz.
    // Software trimming not used here, since an accurate clock is not needed. 
    // For more information on software trimming of DCO (which is not here) please see:
    // - examples using software trimming on TI website: https://dev.ti.com/tirex/explore/node?node=A__AJ6DhSxHmSwvIsn.HM6XYQ__msp430ware__IOGqZri__LATEST&search=MSP430FR2355 
    // - the following youtube video for a good explenation on DCO trimming: https://www.youtube.com/watch?v=9Rlr1tghqtc


    // configure port 2 to accept XIN and XOUT
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P2SEL1 |= BIT6 | BIT7;                  // P2.6~P2.7: crystal pins

    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag


    //  Description: Default DCODIV is MCLK and SMCLK source.
    //  By default, FR235x select XT1 as FLL reference.
    //  If XT1 is present, the XIN and XOUT pin needs to configure.
    //  If XT1 is absent, switch to select REFO as FLL reference automatically.

    //  f(DCOCLK) = 2^FLLD * (FLLN+1) * (fFLLREFCLK / n).
    //  FLLD = 0, FLLN =30, n=1, DIVM =1, 
    //  f(DCOCLK) = 2^0 * (30+1)*32768Hz = 1MHz,
    //  f(DCODIV) = (30+1)*32768Hz = 1MHz,
    //  ACLK = XT1 = ~32768Hz, SMCLK = MCLK = f(DCODIV) = 1MHz.
    //  Toggle LED to indicate that the program is running.
    __bis_SR_register(SCG0);                // Disable FLL before adjusting
    CSCTL3 = SELREF__XT1CLK;                // Set XT1 as FLL reference source - see Table3-7 in user guide
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_1;// DCOFTRIM=3, DCO Range = 1MHz
    CSCTL2 = FLLD_0 + 62;                   // DCODIV = 1MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                // Enable FLL after adjusting
    //software_trim();                      // Software Trim to get the best DCOFTRIM value after FLL re-enabled - this is really not needed
    CSCTL0 = 256;
    CSCTL5 &= ~ (DIVS0 | DIVS1);
    
    CSCTL4 = SELMS__DCOCLKDIV | SELA__XT1CLK;  // set ACLK = XT1CLK = 32768Hz
                                               // DCOCLK = MCLK and SMCLK source

    


    //P1DIR |= BIT1 | BIT0;                   // see table 6-63 in the *datasheet* - we want to enable P1.0 = SMCLK and P1.1 as ACLK
    //P1SEL1 |= BIT1 | BIT0;                  // see table 6.63. P1SEL0 is default 00h on startup, so no need to write to that. 

    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings


}


// public function decleration
void clock_init()
{
    xtal_init();
    enable_millis_timer();
    enable_second_timer();
}


/* -------------------------------------
//      millis timer
----------------------------------------*/
// Configure Timer B0 to trigger every 1ms. 
// for some reason cant access Timer A?
void enable_millis_timer()
{
    TB0CCTL0 |= CCIE; // TBCCR0 interrupt enabled
    //32.768 = ~1ms, 32768 = 1s, 0.5ms = 16
    TB0CCR0 = 33;
    TB0CTL = TBSSEL__ACLK | MC__UP; // ACLK, UP mode
    
    // enable debug output for 1ms timer.
    P3DIR |= BIT0;

}

void timer_1ms_flag_set()
{
    timer_1ms_flag = 1;
}

uint8_t timer_1ms_flag_get()
{
    return timer_1ms_flag;
}

void timer_1ms_flag_reset(void)
{
    timer_1ms_flag = 0;
}


/* -------------------------------------
//      seconds timer
----------------------------------------*/

void enable_second_timer()
{
    TB1CCTL0 |= CCIE; // TBCCR0 interrupt enabled
    //32.768 = ~1ms, 32768 = 1s, 0.5ms = 16
    TB1CCR0 = 32768;
    TB1CTL = TBSSEL__ACLK | MC__UP; // ACLK, UP mode
    
    // enable debug output for 1s timer. - debug only! uncomment when not in use
    P6DIR |= BIT6;
}

void timer_1s_flag_set()
{
    timer_1s_flag = 1;
}

uint8_t timer_1s_flag_get()
{
    return timer_1s_flag;
}

void timer_1s_flag_reset(void)
{
    timer_1s_flag = 0;
}

/* -------------------------------------
//      Interrupts
----------------------------------------*/


// Timer0_B0 interrupt service routine
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
{
    P3OUT ^= BIT0;
    timer_1ms_flag_set(); // flag for main loop.
    __bic_SR_register_on_exit(LPM0_bits); // wakeup main CPU
}

// Timer1_B0 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR (void)
{
    P6OUT ^= BIT6;
    timer_1s_flag_set(); // flag for main loop.
    __bic_SR_register_on_exit(LPM0_bits); // wakeup main CPU
}
