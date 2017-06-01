/**
 * Created by Jake Aboganda on 2017/05/29.
 * Copyright (c) 2017 TenTen Technologies Limited. All rights reserved.
 * Updated: Joem  06/30/17
 * Updated: Joem  06/01/17
 */

#include "led0.h"

// ----- Utilities -----
// NOTE: If the environment allows it, move these to a separate file

// Calculate percentage without using floating point operations
// Reference: 
// https://stackoverflow.com/questions/20788793
int getPercentage(int value, int total)
{
    return (100 * value + total / 2) / total;
}

// Round off float values
// Reference:
// https://stackoverflow.com/questions/4572556
// Confirm:
// Use round() in math.h if supported
uint8_t floatRound(float x)
{
    if (x < 0.0)
        return (uint8_t)(x - 0.5);
    else
        return (uint8_t)(x + 0.5);
}

// Confirm:
// Use fmin and fmax in math.h if supported
float floatMin(float a, float b)
{
    return a < b ? a : b;
}
float floatMax(float a, float b)
{
    return a > b ? a : b;
}

// Convert between RGB and HSL
// Reference:
// http://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/
typedef struct
{
    int blue;
    int green;
    int red;
} Rgb;

// NOTE: The Hue value needs to be multiplied by 60 to convert it to degrees
typedef struct
{
    float hue;
    float saturation;
    float luminosity;
} Hsl;

Hsl Rgb2Hsl(Rgb sourceColor)
{
    // Normalize RGB values to the range 0-1
    float R = (sourceColor.red / 255.0f);
    float G = (sourceColor.green / 255.0f);
    float B = (sourceColor.blue / 255.0f);

    // Find the minimum and maximum values of R, G and B
    float Min = floatMin(floatMin(R, G), B);
    float Max = floatMax(floatMax(R, G), B);
    float Delta = Max - Min;

    // Calculate the Luminance value by adding the max and min values and divide by 2
    float H = 0;
    float S = 0; // If the min and max value are the same, there is no saturation
    float L = (float)((Max + Min) / 2.0f);

    if (Delta != 0)
    {
        if (L < 0.5f)
        {
            // If Luminance is smaller then 0.5, then Saturation = (max - min) / (max + min)
            S = (float)(Delta / (Max + Min));
        }
        else
        {
            // If Luminance is bigger then 0.5. then Saturation = ( max-min)/(2.0-max-min)
            S = (float)(Delta / (2.0f - Max - Min));
        }

        if (R == Max)
        {
            // If Red is max, then Hue = (G-B)/(max-min)
            H = (G - B) / Delta;
        }
        else if (G == Max)
        {
            // If Green is max, then Hue = 2.0 + (B-R)/(max-min)
            H = 2.0f + (B - R) / Delta;
        }
        else if (B == Max)
        {
            // If Blue is max, then Hue = 4.0 + (R-G)/(max-min)
            H = 4.0f + (R - G) / Delta;
        }
    }

    Hsl destColor;
    destColor.hue = H;
    destColor.saturation = S;
    destColor.luminosity = L;

    return destColor;
}

double GetColor(double temporary_X, double temporary_1, double temporary_2)
{
    // All values need to be between 0 and 1. In our case all the values are between 0 and 1
    // If you get a negative value you need to add 1 to it.
    // If you get a value above 1 you need to subtract 1 from it.
    if (temporary_X < 0) temporary_X += 1.0l;
    if (temporary_X > 1) temporary_X -= 1.0l;

    // If 6 x temporary_X is smaller then 1,
    // Return temporary_2 + (temporary_1 – temporary_2) x 6 x temporary_X
    if (6.0l * temporary_X < 1.0l) return temporary_2 + (temporary_1 - temporary_2) * 6.0l * temporary_X;

    // If 2 x temporary_X is smaller then 1, Return temporary_1
    if (2.0l * temporary_X < 1.0l) return temporary_1;

    // If 3 x temporary_X is smaller then 2,
    // Return temporary_2 + (temporary_1 – temporary_2) x (0.666 – temporary_X) x 6
    if (3.0l * temporary_X < 2.0l) return temporary_2 + (temporary_1 - temporary_2) * (2.0l / 3.0l - temporary_X) * 6.0l;

    // Return temporary_2
    return temporary_2;
}

Rgb Hsl2Rgb(Hsl sourceColor)
{
    uint8_t R, G, B;
    if (sourceColor.saturation == 0)
    {
        // If there is no Saturation it’s a shade of grey
        R = floatRound(sourceColor.luminosity * 255.0f);
        G = floatRound(sourceColor.luminosity * 255.0f);
        B = floatRound(sourceColor.luminosity * 255.0f);
    }
    else
    {
        double temporary_1, temporary_2;

        if (sourceColor.luminosity < 0.5f)
        {
            // If Luminance is smaller then 0.5 (50%) then temporary_1 = Luminance x (1.0+Saturation)
            temporary_1 = sourceColor.luminosity * (1.0f + sourceColor.saturation);
        }
        else
        {
            // If Luminance is equal or larger then 0.5 (50%) then temporary_1 = Luminance + Saturation – Luminance x Saturation
            temporary_1 = (sourceColor.luminosity + sourceColor.saturation) - (sourceColor.luminosity * sourceColor.saturation);
        }
        // temporary_2 = 2 x Luminance – temporary_1
        temporary_2 = 2.0f * sourceColor.luminosity - temporary_1;

        // NOTE: The Hue value needs to be divided by 6 to normalize it (60/360)
        double Hue = sourceColor.hue / 6.0f;

        double temporary_R, temporary_G, temporary_B;
        // temporary_R = Hue + 0.333 = 0.536 + 0.333
        temporary_R = Hue + (1.0f / 3.0f);
        // temporary_G = Hue
        temporary_G = Hue;
        // temporary_B = Hue – 0.333
        temporary_B = Hue - (1.0f / 3.0f);

        // perform final color calculations
        temporary_R = GetColor(temporary_R, temporary_1, temporary_2);
        temporary_G = GetColor(temporary_G, temporary_1, temporary_2);
        temporary_B = GetColor(temporary_B, temporary_1, temporary_2);
        R = floatRound(temporary_R * 255.0f);
        G = floatRound(temporary_G * 255.0f);
        B = floatRound(temporary_B * 255.0f);
    }

    Rgb destColor;
    destColor.red = R;
    destColor.green = G;
    destColor.blue = B;

    return destColor;
}

// ----- End Utilities -----

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
// Reference: www.st.com/resource/en/application_note/cd00157323.pdf

// service interrupt
#if (CONFIG_HARDWARE == CONFIG_HARDWARE_TI_TM4C1294)
void __attribute__ ((interrupt)) serviceTimerInterrupt(void)
#else
void serviceTimerInterrupt(void)
#endif
{
    if (timerVal == 1)
    {
        DEBUG_PRINT(("--- Cycle Start ---\n"));
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
    DEBUG_PRINT(("Tick=%d,R=%d,G=%d,B=%d\n", timerVal, led0Pins->red, led0Pins->green, led0Pins->blue));

    if (timerVal < TICKS_PER_CYCLE)
    {
        timerVal++;
    }
    else
    {
        DEBUG_PRINT(("--- Cycle End ---\n"));
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

    // Apply brightness if necessary
    // Convert to HSL, modify L, then revert to RGB
    if (brightness != 255)
    {
        DEBUG_PRINT(("Applying Brightness\n"));
        Rgb rgbColor;
        rgbColor.red = red;
        rgbColor.green = green;
        rgbColor.blue = blue;
        DEBUG_PRINT(("Initial Color Info\n"));
        DEBUG_PRINT(("R=%d,G=%d,B=%d\n", rgbColor.red, rgbColor.green, rgbColor.blue));
        Hsl hslColor = Rgb2Hsl(rgbColor);
        DEBUG_PRINT(("H=%f,S=%f,L=%f\n", hslColor.hue, hslColor.saturation, hslColor.luminosity));

        DEBUG_PRINT(("Final Color Info\n"));
        hslColor.luminosity = hslColor.luminosity * (brightness / 255.0f);
        DEBUG_PRINT(("H=%f,S=%f,L=%f\n", hslColor.hue, hslColor.saturation, hslColor.luminosity));
        rgbColor = Hsl2Rgb(hslColor);
        DEBUG_PRINT(("R=%d,G=%d,B=%d\n", rgbColor.red, rgbColor.green, rgbColor.blue));
    }
    else
    {
        DEBUG_PRINT(("RGB Values\n"));
        DEBUG_PRINT(("R=%d,G=%d,B=%d\n", red, green, blue));
    }

    timerVal = 1;
    rgbDutyCycle.red = getPercentage(red, MAX_COLOR);
    rgbDutyCycle.green = getPercentage(green, MAX_COLOR);
    rgbDutyCycle.blue = getPercentage(blue, MAX_COLOR);
    DEBUG_PRINT(("Duty Cycles\n"));
    DEBUG_PRINT(("R=%d,G=%d,B=%d\n", rgbDutyCycle.red, rgbDutyCycle.green, rgbDutyCycle.blue));

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
    // Handling for invalid inputs and error cases
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
    DEBUG_PRINT(("Stacked Settings=%d\n", rgbSettingsTop));

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
    DEBUG_PRINT(("Stacked Settings=%d\n", rgbSettingsTop));

    return true;
}
