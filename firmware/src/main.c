#include <stdio.h>
#include <string.h>

#include "system_rcc.h"
#include "system_uart.h"
#include "system_motor.h"

int main(void)
{

    system_rcc_init();
    system_uart_init();
    system_motor_init();

    while (1)
    {
        for (int i = 0; i < 25000000U / 100; i++)
            ;
    }

    return 0;
}
