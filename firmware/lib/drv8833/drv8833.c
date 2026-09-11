#include "drv8833.h"

static uint32_t max_duty_cycle = 0;

drv8833_err_t drv8833_init(void (*configure_peripherals)(void),
                           drv8833_motor_t *motor)
{

    if (configure_peripherals == NULL)
    {
        return DRV8833_ERROR_NO_CONFIGURE;
    }

    if (motor->set_ain == NULL || motor->set_bin == NULL)
    {
        return DRV8833_ERROR_INVALID_CHANNEL_FUNCTIONS;
    }

    if (motor->max_dutycycle == 0)
    {
        return DRV8833_ERROR_INVALID_DUTYCYCLE;
    }

    configure_peripherals();

    return DRV8833_OK;
}

drv8833_err_t drv8833_set_sleep(drv8833_motor_t *motor, uint8_t sleep)
{
    if (motor == NULL)
    {
        return DRV8833_ERROR_MOTOR_NULL;
    }

    if (motor->set_sleep == NULL)
    {
        return DRV8833_ERROR_INVALID_CHANNEL_FUNCTIONS;
    }

    motor->set_sleep(sleep);

    return DRV8833_OK;
}

drv8833_err_t drv8833_set_motor_dutycycle(drv8833_motor_t *motor,
                                          uint32_t dutycycle_ain, 
                                          uint32_t dutycycle_bin)
{
    if (motor == NULL)
    {
        return DRV8833_ERROR_MOTOR_NULL;
    }

    if (dutycycle_ain > motor->max_dutycycle || dutycycle_bin > motor->max_dutycycle)
    {
        return DRV8833_ERROR_INVALID_DUTYCYCLE;
    }

    motor->set_ain(dutycycle_ain);
    motor->set_bin(dutycycle_bin);

    return DRV8833_OK;
}