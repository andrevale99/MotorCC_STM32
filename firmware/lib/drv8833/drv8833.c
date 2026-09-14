#include "drv8833.h"

drv8833_err_t drv8833_init(void (*configure_peripherals)(void),
                           drv8833_t *drv)
{

    if (configure_peripherals == NULL)
    {
        return DRV8833_ERROR_NO_CONFIGURE;
    }

    if (drv->set_ain == NULL || drv->set_bin == NULL)
    {
        return DRV8833_ERROR_INVALID_CHANNEL_FUNCTIONS;
    }

    if (drv->max_dutycycle == 0)
    {
        return DRV8833_ERROR_INVALID_DUTYCYCLE;
    }

    configure_peripherals();

    return DRV8833_OK;
}

drv8833_err_t drv8833_set_sleep(drv8833_t *drv, uint8_t sleep)
{
    if (drv == NULL)
    {
        return DRV8833_ERROR_MOTOR_NULL;
    }

    if (drv->set_sleep == NULL)
    {
        return DRV8833_ERROR_INVALID_CHANNEL_FUNCTIONS;
    }

    drv->set_sleep(sleep);

    return DRV8833_OK;
}

drv8833_err_t drv8833_set_motor_dutycycle(drv8833_t *drv,
                                          uint32_t dutycycle_ain,
                                          uint32_t dutycycle_bin)
{
    if (drv == NULL)
    {
        return DRV8833_ERROR_MOTOR_NULL;
    }

    if (dutycycle_ain > drv->max_dutycycle ||
        dutycycle_bin > drv->max_dutycycle)
    {
        return DRV8833_ERROR_INVALID_DUTYCYCLE;
    }

    drv->set_ain(dutycycle_ain);
    drv->set_bin(dutycycle_bin);

    return DRV8833_OK;
}