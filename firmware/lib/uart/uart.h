#ifndef UART_H
#define UART_H

#include <stm32f411xe.h>



void usart1_init(int clock, int baudrate);

void usart1_send_char(char c);

void usart1_send_string(const char *str);

#endif
