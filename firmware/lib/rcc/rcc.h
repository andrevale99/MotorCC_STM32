#ifndef RCC_H
#define RCC_H

#include <stdint.h>

#include <stm32f411xe.h>

typedef enum
{
    RCC_OK = 0,
    RCC_ERR_INVALID_CLOCK_SOURCE = -1,
    RCC_ERR_INVALID_APB_NUM = -2,
    RCC_ERR_INVALID_APB_DIVIDER = -3,
    RCC_ERR_CLOCK_NOT_RDY = -4,
    RCC_ERR_SET_CLOCK_SOURCE_FAILED = -5,
}rcc_err_t;

typedef enum
{
    RCC_HSI = 0,
    RCC_HSE = 1,
    RCC_PLL = 2
}rcc_clock_source_t;

typedef enum
{
    RCC_APBx_DIV_0 = 0,
    RCC_APBx_DIV_2 = 4,
    RCC_APBx_DIV_4 = 5,
    RCC_APBx_DIV_8 = 6,
    RCC_APBx_DIV_16 = 7
}rcc_apb_divider_t;

typedef enum
{
    RCC_AHB_DIV_0 = 0,
    RCC_AHB_DIV_2 = 8,
    RCC_AHB_DIV_4 = 9,
    RCC_AHB_DIV_8 = 10,
    RCC_AHB_DIV_16 = 11,
    RCC_AHB_DIV_64 = 12,
    RCC_AHB_DIV_128 = 13,
    RCC_AHB_DIV_256 = 14,
    RCC_AHB_DIV_512 = 15
}rcc_ahb_divider_t;

typedef struct
{
    rcc_clock_source_t clock_source;
    rcc_ahb_divider_t ahb_divider;
    rcc_apb_divider_t apb1_divider;
    rcc_apb_divider_t apb2_divider;
}rcc_clock_status_t;

typedef struct
{
    rcc_clock_source_t clock_source;
    rcc_ahb_divider_t ahb_divider;
    rcc_apb_divider_t apb1_divider;
    rcc_apb_divider_t apb2_divider;
}rcc_confg_t;

rcc_err_t rcc_init(rcc_confg_t *config);

rcc_err_t rcc_set_clock_source(rcc_clock_source_t clock_source);

rcc_err_t rcc_set_apbx_divider(int apb_num, rcc_apb_divider_t divider);

rcc_err_t rcc_set_ahb_divider(rcc_ahb_divider_t divider);

void rcc_get_clock_status(rcc_clock_status_t *status);

#endif