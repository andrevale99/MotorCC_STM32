#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <stm32f411xe.h>

void motor_configure_peripherals(void)
{
    
    // frequency = 25 MHz / (PSC + 1) / (ARR + 1)
    // For 1 kHz PWM frequency, we can set PSC = 1 and ARR = 2501 - 1
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN; // Enable GPIOA and GPIOB clocks

    GPIOA->MODER |= (2 << GPIO_MODER_MODER1_Pos) | (2 << GPIO_MODER_MODER2_Pos); // Set PA1 and PA2 to alternate function mode
    GPIOA->OSPEEDR |= (2 << GPIO_OSPEEDR_OSPEED1_Pos) | (2 << GPIO_OSPEEDR_OSPEED2_Pos); // Set PA1 and PA2 to fast speed
    GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL1_Pos) | (1 << GPIO_AFRL_AFSEL2_Pos); // Set PA1 and PA2 to AF1 (TIM2_CH2 and TIM2_CH3)

    TIM2->PSC = 24; // Prescaler
    TIM2->ARR = 1000 - 1; // Auto-reload value for 1 kHz PWM frequency
    
    TIM2->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos); // PWM mode 1 for channels 2
    TIM2->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos); // PWM mode 1 for channels 3
    TIM2->CCER |= (1 << TIM_CCER_CC2E_Pos) | (1 << TIM_CCER_CC3E_Pos); // Enable output for channels 1 and 2
    
    TIM2->CCR2 = 0; // Initial duty cycle for channel 2
    TIM2->CCR3 = 0; // Initial duty cycle for channel 3
    
    TIM2->CR1 |= (1 << TIM_CR1_CEN_Pos);  

    GPIOB->MODER |= (1 << GPIO_MODER_MODER9_Pos); // Set PB9 to output mode for SLEEP pin
    GPIOB->OSPEEDR |= (2 << GPIO_OSPEEDR_OSPEED9_Pos); // Set PB9 to fast speed
    GPIOB->BSRR |= GPIO_BSRR_BR9; // Set PB9 low to turn off the motor driver


}

void motor_set_ain(uint32_t dutycycle)
{
    if (dutycycle > TIM2->ARR)
        dutycycle = TIM2->ARR;

    TIM2->CCR2 = dutycycle;
}

void motor_set_bin(uint32_t dutycycle)
{
    if (dutycycle > TIM2->ARR)
        dutycycle = TIM2->ARR;

    TIM2->CCR3 = dutycycle;
}
void motor_set_sleep(uint8_t state)
{
    if (state)
    {
        // Set the SLEEP pin high to put the motor driver into On mode
        GPIOB->BSRR = GPIO_BSRR_BS9; // Assuming SLEEP is connected to PB9
    }
    else
    {
        // Set the SLEEP pin low to sleep the motor driver
        GPIOB->BSRR = GPIO_BSRR_BR9; // Assuming SLEEP is connected to PB9
    }
}

#endif