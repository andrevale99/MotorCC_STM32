#ifndef ENCODER_STARTUP_H
#define ENCODER_STARTUP_H

#include <stm32f411xe.h>

void encoder_configure_peripherals(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    GPIOB->MODER |= (2 << GPIO_MODER_MODER6_Pos) |
                    (2 << GPIO_MODER_MODER7_Pos);

    GPIOB->AFR[0] |= (2 << GPIO_AFRL_AFSEL6_Pos) |
                     ((2 << GPIO_AFRL_AFSEL7_Pos));

    /* Encoder mode */
    TIM4->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;

    /* CH1 e CH2 como entrada */
    TIM4->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

    /* Contador */
    TIM4->ARR = 0xFFFF;
    TIM4->CNT = 0;

    TIM4->CR1 |= TIM_CR1_CEN;
}


#endif