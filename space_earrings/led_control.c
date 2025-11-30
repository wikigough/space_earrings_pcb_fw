#include "led_control.h"
#include "drivers/gpio.h"

// private variables
uint8_t led_list[9] = {LED1, LED2, LED3, LED4, LED5, LED6, LED7, LED8, LED9};
uint8_t led_port_list[9] = {LED1_PORT, LED2_PORT, LED3_PORT, LED4_PORT, LED5_PORT, LED6_PORT, LED7_PORT, LED8_PORT, LED9_PORT};

// initiate iters with an offset
LedIters led_iters = {0, 400, 1200, 0, 0, 0, 0, 0, 0};
LedActiveTracker led_active_track = {1, 0, 1, 0, 0, 0, 0, 1, 0};

// single led function
static const uint16_t max_iter = 2000;                // 2000ms per total sunsoid waveform
static const uint16_t sinusoid_size = 20;              // size of the sinusoid array. This means that each sinusoid point will be max_iter / sinusoid_size long. 
static const uint16_t max_led_blink_period_size = 20; // 10ms per single LED PWM output - this means that we have (max_iter/sinusoid_size) / max_led_blink_period_size 
                                                      // = 5 waveforms per sinusoid "point". Please ensure that the equation in this comment is a whole number. 
// reciprocals for faster calculations - remember to >> 16 for these to work. 
static const uint16_t sinusoid_size_recip = (uint16_t)(65536 / sinusoid_size); // 3276u
static const uint16_t max_led_blink_period_size_recip = (uint16_t)(65536 / max_led_blink_period_size); // 6553u


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
static const uint8_t sinusoid[sinusoid_size] = { 0, 6, 24, 53, 88, 128, 168, 203, 232, 250, 255, 250, 232, 203, 168, 128, 88, 53, 24, 6};
static uint8_t iter_to_sinusoid_index[max_iter] = {0};


// private functions
uint8_t increment_iter(uint16_t *iter);


uint8_t increment_iter(uint16_t *iter)
{
    uint8_t end_reached = 0;
    if (*iter < max_iter)
    {
        *iter += 1;        
    }
    else 
    {
        *iter = 0;
        end_reached = 1;
    }
    return end_reached;
}

void init_twinkle(void)
{
    uint16_t i;
    for (i = 0; i < max_iter; i++) 
    {
        // note the need to promote the calculation to 32bit before saving it as 8 bit
        iter_to_sinusoid_index[i] = (uint8_t)(((uint32_t)i * sinusoid_size) / max_iter);
    }
        
}

uint8_t sine_single_led(uint8_t led_num, uint16_t *iter)
{
    uint8_t sinusoid_index = iter_to_sinusoid_index[*iter]; 

    // calculate lower bound
    uint16_t lower_bound = (uint16_t)(((uint32_t)max_iter * sinusoid_index * sinusoid_size_recip) >> 16);

    // relying on getting a non-float index *getting a rounded down number*
    uint32_t temp = ((uint32_t)(*iter - lower_bound)*max_led_blink_period_size_recip);
    uint16_t index_within_bound = temp >> 16;


    // find out if you need to be high or low with the current fine iteration
    uint16_t transition_to_low = lower_bound + (uint16_t)((uint32_t)index_within_bound * max_led_blink_period_size) +((sinusoid[sinusoid_index] * max_led_blink_period_size) >> 8);

    if (*iter < transition_to_low)
    {
        set_gpio(led_list[led_num-1], led_port_list[led_num-1]);
    }
    else
    {
        clear_gpio(led_list[led_num-1], led_port_list[led_num-1]);
    }


    uint8_t end = increment_iter(iter);
    return end;

}

void twinkle(void)
{
    // GROUP 1: LEDs 1 -> 4 -> 7 twinkle with iter offset of 0
    if (led_active_track.led1_active)
    {
        uint8_t end = sine_single_led(1, &led_iters.led1_iter);
        if (end)
        {
            led_active_track.led1_active = 0;
            led_active_track.led4_active = 1;
        }
    }
    else if (led_active_track.led4_active) 
    {
        uint8_t end = sine_single_led(4, &led_iters.led4_iter);
        if (end)
        {
            led_active_track.led4_active = 0;
            led_active_track.led7_active = 1;
        }
    }
    else if (led_active_track.led7_active) 
    {
        uint8_t end = sine_single_led(7, &led_iters.led7_iter);
        if (end)
        {
            led_active_track.led7_active = 0;
            led_active_track.led1_active = 1;
        }
    }
    else {
        // invalid. error handling TBC
    }

    // GROUP 2: LEDs 2 -> 5 -> 8 twinkle with iter offset defined in led_iters
    if (led_active_track.led2_active)
    {
        uint8_t end = sine_single_led(2, &led_iters.led2_iter);
        if (end)
        {
            led_active_track.led2_active = 0;
            led_active_track.led5_active = 1;
        }
    }
    else if (led_active_track.led5_active) 
    {
        uint8_t end = sine_single_led(5, &led_iters.led5_iter);
        if (end)
        {
            led_active_track.led5_active = 0;
            led_active_track.led8_active = 1;
        }
    }
    else if (led_active_track.led8_active) 
    {
        uint8_t end = sine_single_led(8, &led_iters.led8_iter);
        if (end)
        {
            led_active_track.led8_active = 0;
            led_active_track.led2_active = 1;
        }
    }
    else {
        // invalid. error handling TBC
    }
    
    // GROUP 3: LEDs 3 -> 6 -> 9 twinkle with iter offset defined in led_iters
    if (led_active_track.led3_active)
    {
        uint8_t end = sine_single_led(3, &led_iters.led3_iter);
        if (end)
        {
            led_active_track.led3_active = 0;
            led_active_track.led6_active = 1;
        }
    }
    else if (led_active_track.led6_active) 
    {
        uint8_t end = sine_single_led(6, &led_iters.led6_iter);
        if (end)
        {
            led_active_track.led6_active = 0;
            led_active_track.led9_active = 1;
        }
    }
    else if (led_active_track.led9_active) 
    {
        uint8_t end = sine_single_led(9, &led_iters.led9_iter);
        if (end)
        {
            led_active_track.led9_active = 0;
            led_active_track.led3_active = 1;
        }
    }
    else {
        // invalid. error handling TBC
    }
}

