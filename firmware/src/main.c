#include <stm32f411xe.h>

#include "system.h"

#include "rcc.h"
#include "uart.h"
#include "drv8833.h"

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

    drv8833_init(motor_configure_peripherals, &motor);

    drv8833_set_sleep(&motor, 1); // Wake up the motor driver

    drv8833_set_motor_dutycycle(&motor, 0, 500); // Set 50% duty cycle for both channels

    while (1)
    {
        usart1_send_string("Andre eh top!\r\n");

        /*
         * Apenas para tornar a visualização no terminal
         * mais fácil.
         *
         * Neste exemplo não estamos usando SysTick.
         */
        for (volatile uint32_t i = 0; i < 2500000; i++)
            ;
    }

    return 0;
}
