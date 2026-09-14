#ifndef SYSTEM_RCC_H
#define SYSTEM_RCC_H

#include "rcc.h"

int system_rcc_init(void)
{
    rcc_confg_t config = {
        .clock_source = RCC_HSE,
        .ahb_divider = RCC_AHB_DIV_0,
        .apb1_divider = RCC_APBx_DIV_0,
        .apb2_divider = RCC_APBx_DIV_0};

    rcc_init(&config);

    rcc_clock_status_t status;
    rcc_get_clock_status(&status);
    SystemCoreClockUpdate();

    if (status.clock_source == RCC_HSE)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOC clock
        GPIOC->MODER |= GPIO_MODER_MODER13_0;
        GPIOC->BSRR |= GPIO_BSRR_BR13; // Clock source is HSE
    }

    return 0;
}

#endif