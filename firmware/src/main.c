#include <stdio.h>
#include <string.h>

#include <stm32f411xe.h>

#include "drv8833_peripherals.h"
#include "encoder_peripherals.h"

#include "rcc.h"
#include "uart.h"
#include "drv8833.h"
#include "encoder.h"
#include "timer.h"

char buffer[64] = {0};

int main(void)
{
    rcc_confg_t config = {
        .clock_source = RCC_HSE,
        .ahb_divider = RCC_AHB_DIV_0,
        .apb1_divider = RCC_APBx_DIV_0,
        .apb2_divider = RCC_APBx_DIV_0};

    rcc_init(&config);

    rcc_clock_status_t status;
    rcc_get_clock_status(&status);

    if (status.clock_source == RCC_HSE)
    {
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // Enable GPIOC clock
        GPIOC->MODER |= GPIO_MODER_MODER13_0;
        GPIOC->BSRR |= GPIO_BSRR_BR13; // Clock source is HSE
    }

    usart1_init(25000000U, 115200U);

    drv8833_motor_t motor = {
        .set_ain = motor_set_ain,
        .set_bin = motor_set_bin,
        .set_sleep = motor_set_sleep,
        .max_dutycycle = 1000,
    };

    timer_err_t rettimer = TIMER_OK;
    rettimer = timer_use(motor_configure_peripherals, TIMER_2_32_BIT);
    if (rettimer != TIMER_OK)
    {
        sprintf(buffer, "Erro oa iniciar timer do drv8833: %i\n",
                rettimer);
        usart1_send_string(buffer);
        return -1;
    }
    sprintf(buffer, "timer do drv8833 OK: %i\n",
            rettimer);
    usart1_send_string(buffer);

    drv8833_init(motor_configure_peripherals, &motor);
    drv8833_set_sleep(&motor, 1); // Wake up the motor driver

    rettimer = timer_use(encoder_configure_peripherals, TIMER_4_16_BIT);
    if (rettimer != TIMER_OK)
    {
        sprintf(buffer, "Erro oa iniciar timer do encoder: %i\n",
                rettimer);
        usart1_send_string(buffer);
        return -1;
    }
    sprintf(buffer, "timer do encoder OK: %i\n",
            rettimer);
    usart1_send_string(buffer);
    encoder_configure_peripherals();

#ifdef MOTOR_TESTE
        #include "../test/motor_teste.h"
        motor_teste(buffer, &motor);
#endif

    return 0;
}
