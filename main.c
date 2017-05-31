/**
 * Created by Joem on 2017/05/30.
 */

#include "led0.h"

int main(int argc, char *argv[])
{
    // test calls
	led0_initialize();
    led0_set(LED0_ORANGE);
	led0_push(LED0_ORANGE);
	led0_pop();
    return 0;
}
