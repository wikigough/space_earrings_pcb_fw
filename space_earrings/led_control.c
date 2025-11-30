#include "led_control.h"
#include "drivers/gpio.h"

// private variables
uint8_t led_list[9] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED9};
uint8_t led_port_list[9] = {LED1_PORT, LED2_PORT, LED3_PORT, LED4_PORT, LED5_PORT, LED6_PORT, LED7_PORT, LED8_PORT, LED9_PORT};

static uint16_t iter = 0;
static const uint16_t max_iter = 2000;                // 2000ms per total sunsoid waveform
static const uint16_t sinusoid_size = 20;              // size of the sinusoid array. This means that each sinusoid point will be max_iter / sinusoid_size long. 
static const uint16_t max_led_blink_period_size = 20; // 10ms per single LED PWM output - this means that we have (max_iter/sinusoid_size) / max_led_blink_period_size 
                                                      // = 5 waveforms per sinusoid "point". Please ensure that the equation in this comment is a whole number. 

// reciprocals for faster calculations - remember to >> 16 for these to work. 
static const uint16_t sinusoid_size_recip = (uint16_t)(65536 / sinusoid_size); // 3276u
static const uint16_t max_led_blink_period_size_recip = (uint16_t)(65536 / max_led_blink_period_size); // 6553u
//static const uint16_t max_led_blink_period_size_recip = (uint16_t)(((1u << 16) + max_led_blink_period_size - 1) / max_led_blink_period_size);



// generate sine with maybe like 20 discrete points - unfortunately, msp430 doesnt have a sine function, so either we make a lookup table or use Maclaurin series. 
// in python land:
//>>> import math
//>>> for i in range(0, 20):
//...     value = 0.5*(1 - math.cos(2*math.pi*i/20))
//...     print(value)
// originally, used this:
//static float sinusoid[20] = { 0.000, 0.024, 0.095, 0.206, 0.345, 0.500, 0.655, 0.794, 0.905, 0.976, 1.000, 0.976, 0.905, 0.794, 0.655, 0.500, 0.345, 0.206, 0.095, 0.024};
//static const uint16_t sinusoid_size = 20;
// BUT this makes use of floating point math, which is excrutiatingly slow on the MSP430, so we need to change this to integer math!
// keeping the number of points at 20, so we can make a direct comparison - note a premultiply to 255 is required (note 255 = 0b11111111, so dont use 256)
static const uint8_t sinusoid[sinusoid_size] = { 0, 20, 24, 53, 88, 128, 168, 203, 232, 250, 255, 250, 232, 203, 168, 128, 88, 53, 0, 0};
static uint8_t iter_to_sinusoid_index[max_iter] = {0};



// private functions
void increment_iter(void);

void simple_blink(void)
{     
    // NOTE: if you want to see the LEDs flash one after another, you need to change the clock to be a 1 s tick. 
    turn_off_all_leds(); // momentarily turn off all LEDs
    set_gpio(led_list[iter], led_port_list[iter]); // these will now be the values until the next interrupt.

    // increment iter 
    if (iter < 8)
    {
        iter += 1;
    }
    else {
        iter = 0;
    }
    
}

void increment_iter()
{
    if (iter < max_iter)
    {
        iter += 1;        
    }
    else 
    {
        iter = 0;
    }
}

void init_twinkle(void)
{
    uint16_t i;
    for (i = 0; i < max_iter; i++) 
    {
        // note the need to promote the calculation to 16bit before saving it as 8 bit
        iter_to_sinusoid_index[i] = (uint8_t) ((uint16_t)((i * sinusoid_size) / max_iter));
    }
        
}

void sine_single_led(uint8_t led_num)
{
    // we need to calculate waveforms on the go - the waveforms are essentially duty cycling control of LEDs. 
    // brightness at the *peak*
    //uint8_t brightness = 100; // percentage

    // we want the twinkling to be 3 seconds total per LED. This means that our array size is 3seconds / 1ms = 3000.
    // let's say that within the 3000 1ms triggers, we have 100 waveforms, so each waveform is 30 points.
    // Within the 3000 points, we want to control whether the LED is on or off. For a sine wave, we want duty cycle to be low, then high at the middle, then low again. 
    
    
    // find out where youre landing - simple filter - this will tell you where you are in the sinusoid index to get the duty cycle
    //uint8_t sinusoid_index = (uint8_t)((iter * sinusoid_size / max_iter)); // WORKS
    // note need to promote the result to 32-bit before bit shifting by 16.
    //uint8_t sinusoid_index = (uint8_t)((uint32_t)(iter * sinusoid_size * max_iter_recip) >> 16); // does not work. 
    uint8_t sinusoid_index = iter_to_sinusoid_index[iter]; // WORKS


    //find out your current window bounds - for sinusoid
    //uint16_t lower_bound = max_iter * (sinusoid_index / sinusoid_size);
    // note need to promote the result to 32-bit before bit shifting by 16.
    uint16_t lower_bound = (uint16_t)((uint32_t)(max_iter * sinusoid_index * sinusoid_size_recip) >> 16); // WORKS

    // relying on getting a non-float index *getting a rounded down number*
    //uint16_t index_within_bound = (iter - lower_bound) / max_led_blink_period_size; // WORKS
    // note need to promote the result to 32-bit before bit shifting by 16.

    //uint16_t index_within_bound = ((uint32_t)(iter - lower_bound) * (uint32_t)(max_led_blink_period_size_recip) )>> 16;
    uint32_t temp = ((uint32_t)(iter - lower_bound)*max_led_blink_period_size_recip);
    uint16_t index_within_bound = temp >> 16;


    // find out if you need to be high or low with the current fine iteration
    //uint16_t transition_to_low = (max_iter/sinusoid_size) * (sinusoid_index + sinusoid[sinusoid_index]);
    //uint16_t transition_to_low = lower_bound + index_within_bound * max_led_blink_period_size + sinusoid[sinusoid_index] * max_led_blink_period_size;
    //uint16_t transition_to_low = lower_bound + index_within_bound * max_led_blink_period_size + ( (sinusoid[sinusoid_index] * max_led_blink_period_size)>>8);
    uint16_t transition_to_low = lower_bound + index_within_bound * max_led_blink_period_size + ( (sinusoid[sinusoid_index] * max_led_blink_period_size)>>8);
    

    if (iter < transition_to_low)
    {
        set_gpio(led_list[led_num], led_port_list[led_num]);
    }
    else
    {
        clear_gpio(led_list[led_num], led_port_list[led_num]);
    }
    

}

void twinkle(void)
{
    sine_single_led(0);
    sine_single_led(1);
    //sine_single_led(2);
    //sine_single_led(3, 0);
    //sine_single_led(4, 0);
    //sine_single_led(5, 600);
    //sine_single_led(6, 700);
    //sine_single_led(7, 800);
    //sine_single_led(8, 900);
    increment_iter();
}

