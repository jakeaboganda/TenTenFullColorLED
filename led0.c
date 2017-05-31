/**
 * Created by Jake Aboganda on 2017/05/29.
 * Copyright (c) 2017 TenTen Technologies Limited. All rights reserved.
 * Updated: Joem  05/30/17
 */

#include "led0.h"
// ----- Utilities -----
// NOTE: If the environment allows it, move to separate file

// Calculate percentage without using floating point operations
// Reference: 
// https://stackoverflow.com/questions/20788793
int getPercentage(int value, int total)
{
    return (100 * value + total / 2) / total;
}

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

#define initializeTimer()

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

typedef struct
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} RgbDutyCycle;

RgbDutyCycle rgbDutyCycle;
static uint8_t timerVal = 1;

// Software Color Modulation
// reference: www.st.com/resource/en/application_note/cd00157323.pdf

// service interrupt
#if (CONFIG_HARDWARE == CONFIG_HARDWARE_TI_TM4C1294)
void __attribute__ ((interrupt)) serviceTimerInterrupt(void)
#else
void serviceTimerInterrupt(void)
#endif
{
    if (timerVal == 1)
    {
        if (rgbDutyCycle.red)
        {
            led0Pins->red = LED_ON;
        }
        if (rgbDutyCycle.green)
        {
            led0Pins->green = LED_ON;
        }
        if (rgbDutyCycle.blue)
        {
            led0Pins->blue = LED_ON;
        }
    }
    else
    {
        int progress = getPercentage(timerVal, TICKS_PER_CYCLE);
        if (led0Pins->red && (progress > rgbDutyCycle.red))
        {
            led0Pins->red = LED_OFF;
        }
        if (led0Pins->green && (progress > rgbDutyCycle.green))
        {
            led0Pins->green = LED_OFF;
        }
        if (led0Pins->blue && (progress > rgbDutyCycle.blue))
        {
            led0Pins->blue = LED_OFF;
        }
    }

    if (timerVal <= TICKS_PER_CYCLE)
    {
        timerVal++;
    }
    else
    {
        timerVal = 1;
    }

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

    timerVal = 0;
    rgbDutyCycle.red = getPercentage(red, MAX_COLOR);
    rgbDutyCycle.green = getPercentage(green, MAX_COLOR);
    rgbDutyCycle.blue = getPercentage(blue, MAX_COLOR);

    startTimer();
}

// ----- Stack Functionality ----- //
typedef struct
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t brightness;
} RgbSettings;

RgbSettings rgbSettingsStack[MAX_STACK];
static uint8_t rgbSettingsTop = 0;

bool led0_push(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
    if (rgbSettingsTop > (MAX_STACK-1))
    {
        return false;
    }
    led0_set(red, green, blue, brightness);
    rgbSettingsStack[rgbSettingsTop].red = red;
    rgbSettingsStack[rgbSettingsTop].green = green;
    rgbSettingsStack[rgbSettingsTop].blue = blue;
    rgbSettingsStack[rgbSettingsTop].brightness = brightness;
    rgbSettingsTop++;

    return true;
}

bool led0_pop()
{
    if (rgbSettingsTop == 0)
    {
        return false;
    }
    rgbSettingsTop--;
    led0_set(rgbSettingsStack[rgbSettingsTop].red,
             rgbSettingsStack[rgbSettingsTop].green,
             rgbSettingsStack[rgbSettingsTop].blue,
             rgbSettingsStack[rgbSettingsTop].brightness);

    return true;
}
