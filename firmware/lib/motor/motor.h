#ifndef MOTOR_H
#define MOTOR_H

#include <stm32f411xe.h>

#include "drv8833.h"
#include "encoder.h"
#include "timer.h"

typedef enum
{
    MOTOR_OK = 0,
    MOTOR_ERR_INVALID_ARG = -1,
    MOTOR_ERR_INVALID_ENCODER = -2,
    MOTOR_ERR_INVALID_DRV = -3,
} motor_err_t;

typedef struct
{
    encoder_t encoder;
    drv8833_t drv;
} motor_t;

typedef enum
{
    MOTOR_CLOCKWISE,
    MOTOR_COUNTERCLOCKWISE,
} motor_dir_t;

motor_err_t motor_init(motor_t *motor);

motor_err_t motor_on_off(motor_t *motor, int8_t on_off);

motor_err_t motor_set_duty(motor_t *motor,
                           motor_dir_t direction,
                           uint32_t dutycycle);

#endif