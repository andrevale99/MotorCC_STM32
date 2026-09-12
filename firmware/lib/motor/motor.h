#ifndef MOTOR_H
#define MOTOR_H

#include <stm32f411xe.h>

#include "drv8833.h"
#include "encoder.h"
#include "timer.h"

typedef enum
{
    MOTOR_OK = 0,
    MOTOR_ERR_INVALID_ENCODER = -1,
    MOTOR_ERR_INVALID_DRV = -2,
}motor_err_t;

typedef struct
{
    encoder_t encoder;
    drv8833_motor_t drv;
}motor_t;

motor_err_t motor_init(motor_t *motor);


#endif