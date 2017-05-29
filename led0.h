/**
 * Created by Billy M. Millare on 2015/11/14.
 * Copyright (c) 2015 TenTen Technologies Limited. All rights reserved.
 */

#ifndef LED0_H_
#define LED0_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * Initialize the LED 0 module.
 */
bool led0_initialize(void);

/**
 * Set the color and brightness of LED 0.
 */
void led0_set(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);

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

#endif /* LED0_H_ */
