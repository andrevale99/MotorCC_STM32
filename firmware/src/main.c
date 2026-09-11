#include <stdio.h>
#include <string.h>

#include <stm32f411xe.h>

#include "drv8833_peripherals.h"
#include "encoder_peripherals.h"

#include "rcc.h"
#include "uart.h"
#include "drv8833.h"
#include "encoder.h"

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

    encoder_configure_peripherals();



    int duty = 0;

    /*
     * Estado da sequência:
     *
     * 0 -> Sentido 1: subida
     * 1 -> Sentido 1: descida
     * 2 -> Sentido 2: subida
     * 3 -> Sentido 2: descida
     */
    int estado = 0;

    int pulsos = 0;

    char buffer[64] = {0};


    while (1)
    {
        /*
         * --------------------------------------------------------
         * Leitura do encoder
         * --------------------------------------------------------
         */

        pulsos = TIM4->CNT;

        sprintf(buffer, "pls=%i duty=%i estado=%i\n",
                pulsos,
                duty,
                estado);

        usart1_send_string(buffer);


        /*
         * ========================================================
         * MÁQUINA DE ESTADOS
         * ========================================================
         */

        switch (estado)
        {
            /*
             * ====================================================
             * RAMPA 1
             *
             * Sentido 1
             * Duty: 0 -> 1000
             * ====================================================
             */

            case 0:

                if (duty < motor.max_dutycycle)
                {
                    duty++;
                }
                else
                {
                    /*
                     * Chegou ao máximo.
                     * Passa para a rampa de descida.
                     */
                    estado = 1;
                }

                /*
                 * Sentido 1
                 */
                drv8833_set_motor_dutycycle(
                    &motor,
                    0,
                    duty
                );

                break;


            /*
             * ====================================================
             * RAMPA 2
             *
             * Sentido 1
             * Duty: 1000 -> 0
             * ====================================================
             */

            case 1:

                if (duty > 0)
                {
                    duty--;
                }
                else
                {
                    /*
                     * Chegou em zero.
                     *
                     * Troca o sentido e inicia a próxima
                     * rampa.
                     */
                    estado = 2;
                }

                /*
                 * Sentido 1
                 */
                drv8833_set_motor_dutycycle(
                    &motor,
                    0,
                    duty
                );

                break;


            /*
             * ====================================================
             * RAMPA 3
             *
             * Sentido 2
             * Duty: 0 -> 1000
             * ====================================================
             */

            case 2:

                if (duty < motor.max_dutycycle)
                {
                    duty++;
                }
                else
                {
                    /*
                     * Chegou ao máximo.
                     * Passa para a descida.
                     */
                    estado = 3;
                }

                /*
                 * Sentido 2
                 */
                drv8833_set_motor_dutycycle(
                    &motor,
                    duty,
                    0
                );

                break;


            /*
             * ====================================================
             * RAMPA 4
             *
             * Sentido 2
             * Duty: 1000 -> 0
             * ====================================================
             */

            case 3:

                if (duty > 0)
                {
                    duty--;
                }
                else
                {
                    /*
                     * Terminou as quatro rampas.
                     *
                     * Volta para a primeira rampa.
                     */
                    estado = 0;
                }

                /*
                 * Sentido 2
                 */
                drv8833_set_motor_dutycycle(
                    &motor,
                    duty,
                    0
                );

                break;
        }


        /*
         * --------------------------------------------------------
         * Delay
         *
         * Controla a velocidade da rampa.
         * --------------------------------------------------------
         */

        for (volatile uint32_t i = 0; i < 2500000 / 100; i++)
            ;
    }

    return 0;
}
