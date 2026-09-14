#ifndef SYSTEM_MOTOR_H
#define SYSTEM_MOTOR_H

#include <stm32f411xe.h>

#include "log.h"
#include "motor.h"
#include "timer.h"
#include "drv8833.h"
#include "encoder.h"

// ===================================================
// VARS
// ===================================================

motor_t motor;

// ===================================================
// FUNCOES STATICS
// ===================================================

static void drv_configure_peripherals(void)
{

    // frequency = 25 MHz / (PSC + 1) / (ARR + 1)
    // For 1 kHz PWM frequency, we can set PSC = 1 and ARR = 2501 - 1
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;                        // Enable TIM2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN; // Enable GPIOA and GPIOB clocks

    GPIOA->MODER |= (2 << GPIO_MODER_MODER1_Pos) |
                    (2 << GPIO_MODER_MODER2_Pos); // Set PA1 and PA2 to alternate function mode
    GPIOA->OSPEEDR |= (2 << GPIO_OSPEEDR_OSPEED1_Pos) |
                      (2 << GPIO_OSPEEDR_OSPEED2_Pos); // Set PA1 and PA2 to fast speed
    GPIOA->AFR[0] |= (1 << GPIO_AFRL_AFSEL1_Pos) |
                     (1 << GPIO_AFRL_AFSEL2_Pos); // Set PA1 and PA2 to AF1 (TIM2_CH2 and TIM2_CH3)

    TIM2->PSC = 24;       // Prescaler
    TIM2->ARR = 1000 - 1; // Auto-reload value for 1 kHz PWM frequency

    TIM2->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos);                          // PWM mode 1 for channels 2
    TIM2->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);                          // PWM mode 1 for channels 3
    TIM2->CCER |= (1 << TIM_CCER_CC2E_Pos) | (1 << TIM_CCER_CC3E_Pos); // Enable output for channels 1 and 2

    TIM2->CCR2 = 0; // Initial duty cycle for channel 2
    TIM2->CCR3 = 0; // Initial duty cycle for channel 3

    TIM2->CR1 |= (1 << TIM_CR1_CEN_Pos);

    GPIOB->MODER |= (1 << GPIO_MODER_MODER9_Pos);      // Set PB9 to output mode for SLEEP pin
    GPIOB->OSPEEDR |= (2 << GPIO_OSPEEDR_OSPEED9_Pos); // Set PB9 to fast speed
    GPIOB->BSRR |= GPIO_BSRR_BR9;                      // Set PB9 low to turn off the motor driver
}

static void encoder_configure_peripherals(void)
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

// ===================================================
// FUNCOES DE ATRIBUICAO
// ===================================================

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

// ===================================================
// PRINCIPAL
// ===================================================

int system_motor_init(void)
{
    drv8833_t drv = {
        .set_ain = motor_set_ain,
        .set_bin = motor_set_bin,
        .set_sleep = motor_set_sleep,
        .max_dutycycle = 1000,
    };

    timer_err_t rettimer = TIMER_OK;
    rettimer = timer_use(TIMER_2_32_BIT);
    if (rettimer != TIMER_OK)
    {
        log_error("Problema ao verificar timer 2 (drv): %i", rettimer);
        return -1;
    }
    drv8833_init(drv_configure_peripherals, &drv);
    drv8833_set_sleep(&drv, 1); // Wake up the motor driver

    rettimer = timer_use(TIMER_4_16_BIT);
    if (rettimer != TIMER_OK)
    {
        log_error("Problema ao verificar timer 4 (encoder): %i", rettimer);
        return -1;
    }
    encoder_configure_peripherals();

    encoder_t encoder = {
        .gear_box.gearbox_ratio = 32,
        .gear_box.pulses_hall = 11,
        .gear_box.custom_gain = -1.f,
    };

    motor.drv = drv;
    motor.encoder = encoder;
    
    motor_init(&motor);

    log_info("Motor e perifericos inicializados com sucesso");

    motor_set_duty(&motor, MOTOR_CLOCKWISE, 300);

    return 0;
}

#endif