#include <stdio.h>

#include "../lib/drv8833/drv8833.h"
#include "../lib/encoder/encoder.h"
#include "../lib/uart/uart.h"
#include "../lib/timer/timer.h"
#include "../lib/rcc/rcc.h"

void motor_teste(char *buffer, drv8833_motor_t *motor)
{
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

            if (duty < motor->max_dutycycle)
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
                motor,
                0,
                duty);

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
                motor,
                0,
                duty);

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

            if (duty < motor->max_dutycycle)
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
                motor,
                duty,
                0);

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
                motor,
                duty,
                0);

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
}