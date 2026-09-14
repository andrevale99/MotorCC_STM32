#ifndef SYSTEM_UART_H
#define SYSTEM_UART_H

#include "uart.h"
#include "log.h"

void system_uart_init(void)
{
    usart1_init(SystemCoreClock, 115200U);
    log_init();
}

#endif