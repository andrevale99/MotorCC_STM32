#include "timer.h"

#define HOW_MUCH_TIEMR 8

// Variavel para verificar qual timer esta sendo usado
static uint8_t in_use[HOW_MUCH_TIEMR] = {0};

static inline timer_err_t check_timer(timer_select_t select)
{
    if (select < TIMER_1_ADVANCED || select > TIMER_11_16_BIT)
        return TIMER_ERR_INVALID_TIMER;

    return TIMER_OK;
}

timer_err_t timer_use(void (*config)(void),
                      timer_select_t select)
{
    if (config == NULL)
        return TIMER_ERR_INVALID_CONFIG;

    if (check_timer(select) != TIMER_OK)
        return TIMER_ERR_INVALID_TIMER;

    if (in_use[select])
        return TIMER_IN_USE;

    in_use[select] = 1;

    return TIMER_OK;
}

int8_t timer_busy(timer_select_t select)
{
    if (check_timer(select) != TIMER_OK)
        return TIMER_ERR_INVALID_TIMER;

    if (in_use[select])
        return TIMER_IN_USE;

    return TIMER_OK;
}

timer_err_t timer_free(timer_select_t select)
{
    if (check_timer(select) != TIMER_OK)
        return TIMER_ERR_INVALID_TIMER;

    if (in_use[select])
    {
        in_use[select] = 0;
        return TIMER_OK;
    }
    return TIMER_ALREADY_FREE;
}

timer_err_t timer_install_isr(timer_select_t select,
                              timer_isr_t *isr)
{
    if (timer_busy(select) == TIMER_IN_USE)
        return TIMER_ERR_INVALID_TIMER;

    if (check_timer(select) != TIMER_OK)
        return TIMER_ERR_INVALID_TIMER;

    if (NVIC_GetActive(isr->irq_type))
        return TIMER_ERR_ISR_IN_USE;

    NVIC_SetPriority(isr->irq_type, isr->priority);
    NVIC_EnableIRQ(isr->irq_type);

    return TIMER_OK;
}

timer_err_t timer_unistall_isr(timer_select_t select,
                               timer_isr_t *isr)
{

    if (check_timer(select) != TIMER_OK)
        return TIMER_ERR_INVALID_TIMER;

    if (!NVIC_GetActive(isr->irq_type))
        return TIMER_ALREADY_FREE;

    NVIC_DisableIRQ(isr->irq_type);

    return TIMER_OK;
}