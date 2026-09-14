#include "motor.h"

motor_err_t motor_init(motor_t *motor)
{
    if (motor == NULL)
        return MOTOR_ERR_INVALID_ARG;

    if (&(motor->drv) == NULL)
        return MOTOR_ERR_INVALID_DRV;

    if (&(motor->encoder) == NULL)
        return MOTOR_ERR_INVALID_ENCODER;

    return MOTOR_OK;
}

motor_err_t motor_on_off(motor_t *motor, int8_t on_off)
{
    if (motor == NULL)
        return MOTOR_ERR_INVALID_ARG;

    if (on_off)
        drv8833_set_sleep(&(motor->drv), 1);
    else
        drv8833_set_sleep(&(motor->drv), 0);

    return MOTOR_OK;
}

motor_err_t motor_set_duty(motor_t *motor,
                           motor_dir_t direction,
                           uint32_t dutycycle)
{
    if (&(motor->drv) == NULL)
        return MOTOR_ERR_INVALID_DRV;

    switch (direction)
    {
    case MOTOR_CLOCKWISE:
        drv8833_set_motor_dutycycle(&(motor->drv), dutycycle, 0);
        break;

    case MOTOR_COUNTERCLOCKWISE:
        drv8833_set_motor_dutycycle(&(motor->drv), 0, dutycycle);
        break;

    default:
        return MOTOR_ERR_INVALID_ARG;
        break;
    }
    return MOTOR_OK;
}