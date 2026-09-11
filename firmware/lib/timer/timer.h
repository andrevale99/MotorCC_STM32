#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stddef.h>

#include <stm32f411xe.h>

typedef enum
{
    TIMER_ALREADY_FREE = 1,
    TIMER_OK = 0,
    TIMER_ERR_INVALID_CONFIG = -1,
    TIMER_ERR_INVALID_TIMER = -2,
    TIMER_IN_USE = -3,
} timer_err_t;

typedef enum
{
    TIMER_1_ADVANCED = 0,
    TIMER_2_32_BIT,
    TIMER_3_16_BIT,
    TIMER_4_16_BIT,
    TIMER_5_32_BIT,
    TIMER_9_16_BIT,
    TIMER_10_16_BIT,
    TIMER_11_16_BIT,
} timer_select_t;

timer_err_t timer_use(void (*config)(void),
                          timer_select_t select);

int8_t timer_busy(timer_select_t select);

timer_err_t timer_free(timer_select_t select);

#endif