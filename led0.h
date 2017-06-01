/**
 * Created by Billy M. Millare on 2015/11/14.
 * Copyright (c) 2015 TenTen Technologies Limited. All rights reserved.
 * Updated: Joem  05/30/17
 * Updated: Joem  06/01/17
 */

#ifndef LED0_H_
#define LED0_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize the LED 0 module.
 */
bool led0_initialize(void);

#if (CONFIG_HARDWARE != CONFIG_HARDWARE_TI_TM4C1294)
// Allow main() to call the interrupt function for simulation
void serviceTimerInterrupt(void);
#endif

/**
 * Set the color and brightness of LED 0.
 */
void led0_set(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);
/**
 * Push a color onto the stack
 */
bool led0_push(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);
/**
 * Pop a color from the stack
 */
bool led0_pop();

#define LED0_WHITE               255, 255, 255, 255
#define LED0_YELLOW              255, 255,   0, 255
#define LED0_PINK                255,   0, 255, 255
#define LED0_RED                 255,   0,   0, 255
#define LED0_LIGHT_BLUE            0, 255, 255, 255
#define LED0_GREEN                 0, 255,   0, 255
#define LED0_BLUE                  0,   0, 255, 255
#define LED0_NONE                  0,   0,   0, 255
#define LED0_OFF                   0,   0,   0,   0

#define LED0_ORANGE              255, 199,   0, 255
#define LED0_SALMON              250, 128, 114, 255

#define LED_ON                   1
#define LED_OFF                  0

#define MAX_STACK                10

// Timer triggers every 100 ns, or 10,000,000 times per second
// reference:
// COMMUNICATION_TIMER_DURATION_UNIT (CLOCK_UNIT_MICROSECOND / 10)
#define TICKS_PER_SEC            10000000

// Use 50 htz for the frequency
// reference:
// http://www.waitingforfriday.com/?p=404
#define PWM_HTZ                  50
#define TICKS_PER_CYCLE          (TICKS_PER_SEC / PWM_HTZ)

// USAGE:
// DEBUG_PRINT(("%d", var));
// Note the multiple parentheses!
// ToDo:
// Use something like __VA_ARGS__ if supported
#ifdef DEBUG
#include <stdio.h>
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x) do {} while (0)
#endif

#endif /* LED0_H_ */
