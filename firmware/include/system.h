#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <stm32f411xe.h>

void motor_configure_peripherals(void)
{
    
    // frequency = 25 MHz / (PSC + 1) / (ARR + 1)
    // For 1 kHz PWM frequency, we can set PSC = 1 and ARR = 2501 - 1
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Enable TIM4 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // Enable GPIOB clock

    TIM4->PSC = 1; // Prescaler
    TIM4->ARR = 2501 - 1; // Auto-reload value for 1 kHz PWM frequency
    
    TIM4->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos) | (6 << TIM_CCMR1_OC2M_Pos); // PWM mode 1 for channels 1 and 2
    TIM4->CCER |= (1 << TIM_CCER_CC1E_Pos) | (1 << TIM_CCER_CC2E_Pos); // Enable output for channels 1 and 2
    TIM4->CR1 |= (1 << TIM_CR1_CEN_Pos);  
}

void motor_set_ain(uint32_t dutycycle)
{
    TIM4->CCR1 = dutycycle; // Set duty cycle for channel 1
}

void motor_set_bin(uint32_t dutycycle)
{
    TIM4->CCR2 = dutycycle; // Set duty cycle for channel 2
}

#endif