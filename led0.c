/**
 * Created by Jake Aboganda on 2017/05/29.
 * Copyright (c) 2017 TenTen Technologies Limited. All rights reserved.
 */

#include "led0.h"

#if (CONFIG_HARDWARE == CONFIG_HARDWARE_TI_TM4C1294)
#include "dev/clock.h"
#include "dev/hardware.h"
#include "dev/interrupt.h"
#include "dev/uart.h"
#include "dev/logger.h"

// time duration used by the beacon
#define COMMUNICATION_TIMER_DURATION                    1
#define COMMUNICATION_TIMER_DURATION_UNIT               (CLOCK_UNIT_MICROSECOND / 10)

// start bit
#define initializeTimer() \
        { \
            /* select 32-bit timer configuration */ \
            TIMER4_CFG_R = (0 << 0); \
            \
            /* set timer properties */ \
            TIMER4_TAMR_R = 0 \
                    | (0 << 4) /* countdown */ \
                    | (2 << 0) /* periodic timer */ \
                    ; \
            \
            /* enable timeout interrupt */ \
            TIMER4_IMR_R = (1 << 0); \
            interrupt_initialize(INT_TIMER4A, serviceTimerInterrupt); \
            \
            /* set target timeout */ \
            TIMER4_TAILR_R = COMMUNICATION_TIMER_DURATION * (TM4C1294_CLOCK_FREQUENCY_SYSTEM / COMMUNICATION_TIMER_DURATION_UNIT); \
        }

#define startTimer() \
        { \
            TIMER4_CTL_R = (1 << 0); \
        }

#define stopTimer() \
        { \
            TIMER4_CTL_R = (0 << 0); \
        }

#define clearTimerInterrupt() \
        { \
            TIMER4_ICR_R = (1 << 0); \
        }

#define LED0_PINS 0x400623fc

#else

#define startTimer()

#define stopTimer()

#define clearTimerInterrupt()

uint8_t led0pins_test = 0;
#define LED0_PINS (&led0pins_test)

#endif

static volatile union {
    struct {
        uint32_t        : 3;
        uint32_t blue   : 1;
        uint32_t green  : 1;
        uint32_t red    : 1;
    };
    struct {
        uint32_t        : 3;
        uint32_t all    : 3;
    };
} *led0Pins = (void *)LED0_PINS;

// service interrupt
void __attribute__ ((interrupt)) serviceTimerInterrupt(void)
{
    /*clear interrupt*/
    clearTimerInterrupt();
}

bool led0_initialize(void)
{
    initializeTimer();
    startTimer();
    return true;
}

#define MAX_COLOR 0xFF
void led0_set(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
    stopTimer();

    led0Pins->red = (red == MAX_COLOR);
    led0Pins->green = (green == MAX_COLOR);
    led0Pins->blue = (blue == MAX_COLOR);

    startTimer();
}
