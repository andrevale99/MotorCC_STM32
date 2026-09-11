#include "timer.h"

#define HOW_MUCH_TIEMR 8

// Variavel para verificar qual timer esta sendo usado
static uint8_t in_use[HOW_MUCH_TIEMR] = {0};

timer_err_t timer_use(void (*config)(void),
                      timer_select_t select)
{
    if (config == NULL)
        return TIMER_ERR_INVALID_CONFIG;

    if (select < TIMER_1_ADVANCED || select > TIMER_11_16_BIT)
        return TIMER_ERR_INVALID_TIMER;

    if (in_use[select])
        return TIMER_IN_USE;

    in_use[select] = 1;

    return TIMER_OK;
}

int8_t timer_busy(timer_select_t select)
{
    if (in_use[select])
        return TIMER_IN_USE;

    return TIMER_OK;
}

timer_err_t timer_free(timer_select_t select)
{
    if (in_use[select])
    {
        in_use[select] = 0;
        return TIMER_OK;
    }
    return TIMER_ALREADY_FREE;
}

timer_err_t timer_install_isr(timer_select_t select,
                              void (*func)(void), IRQn_Type type)
{
    if (timer_busy(select) == TIMER_IN_USE)
        return TIMER_ERR_INVALID_TIMER;

    if (NVIC_GetActive(type))
        return TIMER_ERR_ISR_IN_USE;

    //instalar a ISR
}