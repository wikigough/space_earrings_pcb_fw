/**
 * @file clock.c
 * @brief System clock setup and 1 ms / 1 s tick timers.
 * @ingroup CLOCK_DRIVER
 */

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

/**
 * @brief Configure the crystal oscillator and basic clock sources.
 * @ingroup CLOCK_DRIVER
 * @note This is a private function.
 */
void xtal_init()
{
    // configure port 2 to accept XIN and XOUT
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    P2SEL1 |= BIT6 | BIT7;                  // P2.6~P2.7: crystal pins

    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag


    //  f(DCOCLK) = 2^FLLD * (FLLN+1) * (fFLLREFCLK / n).
    //  FLLD = 0, FLLN =30, n=1, DIVM =1, 
    //  f(DCOCLK) = 2^0 * (62+1)*32768Hz = 4MHz,
    //  f(DCODIV) = (62+1)*32768Hz = 2MHz,
    //  ACLK = XT1 = ~32768Hz, SMCLK = MCLK = f(DCODIV) = 2MHz.
    // Software trimming not used here, since an accurate clock is not needed. 

    __bis_SR_register(SCG0);                                   // Disable FLL before adjusting
    CSCTL3 = SELREF__XT1CLK;                                        // Set XT1 as FLL reference source - see Table3-7 in user guide
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_1;      // DCOFTRIM=3, DCO Range = 2MHz
    CSCTL2 = FLLD_0 + 62;                                           // DCODIV = 2MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                                    // Enable FLL after adjusting
    CSCTL0 = 256;
    CSCTL5 &= ~ (DIVS0 | DIVS1);
    
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
    enable_second_timer();
}


/* -------------------------------------
//      millis timer
----------------------------------------*/
/**
 * @brief Configure and enable the hardware timer that generates a 1 ms system tick.
 * @ingroup CLOCK_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
void enable_millis_timer()
{
    TB0CCTL0 |= CCIE; // TBCCR0 interrupt enabled
    //32.768 = ~1ms, 32768 = 1s, 0.5ms = 16
    TB0CCR0 = 33;
    TB0CTL = TBSSEL__ACLK | MC__UP; // ACLK, UP mode
    
    // enable debug output for 1ms timer.
    P3DIR |= BIT0;

}

/**
 * @brief Set the 1 ms tick flag from the timer ISR.
 * @ingroup CLOCK_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
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
/**
 * @brief Configure and enable the hardware timer that generates a 1 s system tick.
 * @ingroup CLOCK_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
void enable_second_timer()
{
    TB1CCTL0 |= CCIE; // TBCCR0 interrupt enabled
    //32.768 = ~1ms, 32768 = 1s, 0.5ms = 16
    TB1CCR0 = 32768;
    TB1CTL = TBSSEL__ACLK | MC__UP; // ACLK, UP mode
    
    // enable debug output for 1s timer. - debug only! uncomment when not in use
    P6DIR |= BIT6;
}

/**
 * @brief Set the 1 s tick flag from the timer ISR.
 * @ingroup CLOCK_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
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
/**
 * @brief Timer interrupt service routine used to generate periodic ticks for the application.
 * @ingroup CLOCK_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
__interrupt void Timer0_B0_ISR (void)
{
    P3OUT ^= BIT0;
    timer_1ms_flag_set(); // flag for main loop.
    __bic_SR_register_on_exit(LPM0_bits); // wakeup main CPU
}

// Timer1_B0 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
/**
 * @brief Timer interrupt service routine used to generate periodic ticks for the application.
 * @ingroup CLOCK_DRIVER
 * @note This is an internal helper; it is not exposed in the public header.
 */
__interrupt void Timer1_B0_ISR (void)
{
    P6OUT ^= BIT6;
    timer_1s_flag_set(); // flag for main loop.
    __bic_SR_register_on_exit(LPM0_bits); // wakeup main CPU
}
