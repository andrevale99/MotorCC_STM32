#ifndef SYSTEM_MOTOR_H
#define SYSTEM_MOTOR_H

#include <stm32f411xe.h>

#include "log.h"
#include "pid.h"
#include "motor.h"
#include "timer.h"
#include "drv8833.h"
#include "encoder.h"

#define ENCODER_TIMER TIM4
#define DRV_TIMER TIM2

#define DRV_TIMER_SELECT TIMER_2_32_BIT
#define ENCODER_TIMER_SELECT TIMER_4_16_BIT

#define SAMPLE_TIME_MS 50
#define SAMPLES_TIME_S ((float)(SAMPLE_TIME_MS / 1000.f))

#define SET_FALSE(var) (var = 0)
#define SET_TRUE(var) (var = 1)

#define SETPOINT_MAX 165
#define ADC_MID 2048

// ===================================================
// VARS
// ===================================================

static motor_t motor;

static pid _pid = {
    .kp = 3.96f,
    .ki = 48.41f,
    .kd = 0.f,

    .saturation = 1000.f,
};

static volatile int counterSamples = 0;

static volatile int8_t flagSample = 0;

// ===================================================
// INICIALIZACAO DOS PERIFERICOS
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

    GPIOB->MODER |= (1 << GPIO_MODER_MODER9_Pos);      // Set PB9 to output mode for SLEEP pin
    GPIOB->OSPEEDR |= (2 << GPIO_OSPEEDR_OSPEED9_Pos); // Set PB9 to fast speed
    GPIOB->BSRR |= GPIO_BSRR_BR9;                      // Set PB9 low to turn off the motor driver

    DRV_TIMER->PSC = 24;       // Prescaler
    DRV_TIMER->ARR = 1000 - 1; // Auto-reload value for 1 kHz PWM frequency

    DRV_TIMER->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos);                          // PWM mode 1 for channels 2
    DRV_TIMER->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);                          // PWM mode 1 for channels 3
    DRV_TIMER->CCER |= (1 << TIM_CCER_CC2E_Pos) | (1 << TIM_CCER_CC3E_Pos); // Enable output for channels 1 and 2

    DRV_TIMER->CCR2 = 0; // Initial duty cycle for channel 2
    DRV_TIMER->CCR3 = 0; // Initial duty cycle for channel 3

    DRV_TIMER->DIER |= TIM_DIER_UIE;

    DRV_TIMER->EGR = TIM_EGR_UG;
    DRV_TIMER->SR &= ~TIM_SR_UIF;

    DRV_TIMER->CR1 |= TIM_CR1_CEN;
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
    ENCODER_TIMER->SMCR |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;

    /* CH1 e CH2 como entrada */
    ENCODER_TIMER->CCMR1 |= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;

    /* Contador */
    ENCODER_TIMER->ARR = 0xFFFF;
    ENCODER_TIMER->CNT = 0;

    ENCODER_TIMER->CR1 |= TIM_CR1_URS | TIM_CR1_CEN;
}

static void adc_setup(void)
{
    /*
     * Habilita clock do GPIOA e ADC1.
     */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /*
     * PA4 como entrada analógica.
     */
    GPIOA->MODER &= ~(GPIO_MODER_MODE4_Msk);
    GPIOA->MODER |= (3U << GPIO_MODER_MODE4_Pos);

    /*
     * Sem pull-up/pull-down.
     */
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4_Msk);

    /*
     * ADC1:
     *
     * Canal 4 (PA4)
     * Uma conversão por sequência.
     */
    ADC1->SQR1 = 0;
    ADC1->SQR3 = 4U;

    /*
     * Tempo de amostragem do canal 4.
     *
     * 84 ciclos é uma opção mais segura para uma fonte
     * de alta impedância, como um potenciômetro.
     */
    ADC1->SMPR2 &= ~(ADC_SMPR2_SMP4_Msk);
    ADC1->SMPR2 |= (4U << ADC_SMPR2_SMP4_Pos);

    /*
     * Conversão contínua.
     */
    ADC1->CR2 |= ADC_CR2_CONT;

    /*
     * Liga ADC.
     */
    ADC1->CR2 |= ADC_CR2_ADON;

    /*
     * Inicia conversão.
     */
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

// ===================================================
// FUNCOES DE ATRIBUICAO
// ===================================================

void motor_set_ain(uint32_t dutycycle)
{
    if (dutycycle > DRV_TIMER->ARR)
        dutycycle = DRV_TIMER->ARR;

    DRV_TIMER->CCR2 = dutycycle;
}

void motor_set_bin(uint32_t dutycycle)
{
    if (dutycycle > DRV_TIMER->ARR)
        dutycycle = DRV_TIMER->ARR;

    DRV_TIMER->CCR3 = dutycycle;
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
// INICIALIZACAO DO SISTEMA DO MOTOR
// ===================================================

int system_motor_init(void)
{
    adc_setup();

    drv8833_t drv = {
        .set_ain = motor_set_ain,
        .set_bin = motor_set_bin,
        .set_sleep = motor_set_sleep,
        .max_dutycycle = 1000,
    };

    timer_err_t rettimer = TIMER_OK;
    rettimer = timer_use(DRV_TIMER_SELECT);
    if (rettimer != TIMER_OK)
    {
        log_error("Problema ao verificar timer 2 (drv): %i", rettimer);
        return -1;
    }
    drv8833_init(drv_configure_peripherals, &drv);
    drv8833_set_sleep(&drv, 1); // Wake up the motor driver

    rettimer = timer_use(ENCODER_TIMER_SELECT);
    if (rettimer != TIMER_OK)
    {
        log_error("Problema ao verificar timer 4 (encoder): %i", rettimer);
        return -1;
    }
    encoder_configure_peripherals();

    encoder_t encoder = {
        .gear_box.gearbox_ratio = 36,
        .gear_box.pulses_hall = 11,
        .gear_box.custom_gain = -1.f,
    };

    motor.drv = drv;
    motor.encoder = encoder;

    motor_init(&motor);

    log_info("Motor e perifericos inicializados com sucesso");

    timer_isr_t isr_timer_motor = {
        .irq_type = TIM2_IRQn,
        .priority = 2,
    };
    rettimer = timer_install_isr(DRV_TIMER_SELECT, &isr_timer_motor);
    if (rettimer != TIMER_OK)
    {
        log_error("Problema instalar a isr no timer %i: %i",
                  DRV_TIMER_SELECT, rettimer);
        return -1;
    }

    motor_set_duty(&motor, MOTOR_CLOCKWISE, 0);

    return 0;
}

// ===================================================
// LOOP PRINCIPAL DO SISTEMA MOTOR
// ===================================================

void system_motor_loop()
{
    if (flagSample)
    {
        SET_FALSE(flagSample);
        // Logica para caso n usa a interrupcao
        while (!(ADC1->SR & ADC_SR_EOC))
            ;
        int setpoint = ((int)ADC1->DR - ADC_MID) *
                       SETPOINT_MAX /
                       ADC_MID;

        motor_get_rpm(&motor, SAMPLES_TIME_S);
        pid_control(&_pid, setpoint, motor.speed, SAMPLES_TIME_S);

        if (setpoint > 0)
            motor_set_duty(&motor, MOTOR_COUNTERCLOCKWISE, _pid.output);
        else
            motor_set_duty(&motor, MOTOR_CLOCKWISE, -_pid.output);

        log_info("%i;%i;%i", setpoint, ((int)(_pid.output)), ((int)(motor.speed)));
    }
}

// ===================================================
// INTERRUPCOES
// ===================================================

void TIM2_IRQHandler(void)
{
    if (DRV_TIMER->SR & TIM_SR_UIF)
    {
        DRV_TIMER->SR &= ~TIM_SR_UIF;

        if (!flagSample)
        {
            counterSamples++;
            if (counterSamples > SAMPLE_TIME_MS)
            {
                motor.encoder.pulse = ((int16_t)ENCODER_TIMER->CNT >> 2);
                ENCODER_TIMER->CNT = 0;
                counterSamples = 0;

                SET_TRUE(flagSample);
            }
        }
    }
}

#endif