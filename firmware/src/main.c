#include <stdio.h>
#include <string.h>

#include "system_rcc.h"
#include "system_uart.h"
#include "system_motor.h"

void system_init(void)
{
    system_rcc_init();
    system_uart_init();
    system_motor_init();
}

int main(void)
{
    system_init();

    while (1)
    {
        system_motor_loop();
    }

    return 0;
}
