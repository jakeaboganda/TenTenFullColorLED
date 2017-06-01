/**
 * Created by Joem on 2017/05/30.
 * Updated: Joem  06/01/17
 */

#include "led0.h"
#include "stdio.h"

void simulate_cycles(int numCycles)
{
    int i;
    int totalTicks = numCycles * TICKS_PER_CYCLE;
    for (i = 0; i < totalTicks; i++)
    {
        serviceTimerInterrupt();
    }
}

// TODO: Create a proper unit test framework
void unit_tests()
{
    int i;
    printf("Unit Tests Start\n");

    printf("\nSet R, G, B values\n");
    led0_set(LED0_WHITE);
    led0_set(LED0_ORANGE);
    led0_set(LED0_SALMON);

    printf("\nSet R, G, B values with A\n");
    led0_set(255, 0, 0, 128);
    led0_set(255, 255, 255, 100);
    led0_set(128, 128, 128, 200);

    printf("\nPush and Pop Settings\n");
    for (i = 0; i < (MAX_STACK/2); i++)
    {
        if (!led0_push(LED0_WHITE) ||
            !led0_push(LED0_ORANGE))
        {
            printf("Push and Pop Settings Test Failed\n");
        }
    }
    if (led0_push(LED0_WHITE))
    {
        printf("Push and Pop Settings Test Failed\n");
    }
    for (i = 0; i < (MAX_STACK/2); i++)
    {
        if (!led0_pop() ||
            !led0_pop())
        {
            printf("Push and Pop Settings Test Failed\n");
        }
    }
    if (led0_pop(LED0_WHITE))
    {
        printf("Push and Pop Settings Test Failed\n");
    }

    printf("\nSimulate 5 Cycles\n");
    led0_set(LED0_SALMON);
    simulate_cycles(5);
}

int main(int argc, char *argv[])
{
    unit_tests();    
    return 0;
}
