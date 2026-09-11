#ifndef DRV8833_H
#define DRV8833_H

#include <stdint.h>
#include <stddef.h>

#include <stm32f411xe.h>

typedef enum
{
    DRV8833_OK = 0,
    DRV8833_ERROR_NO_CONFIGURE = -1,
    DRV8833_ERROR_INVALID_CHANNEL_FUNCTIONS = -2,
    DRV8833_ERROR_INVALID_DUTYCYCLE = -3,
    DRV8833_ERROR_MOTOR_NULL = -4
} drv8833_err_t;

typedef struct
{
    void (*set_ain)(uint32_t dutycycle);
    uint32_t dutycycle_ain;

    void (*set_bin)(uint32_t dutycycle);
    uint32_t dutycycle_bin;

    void (*set_sleep)(uint8_t state);

    const uint32_t max_dutycycle;
} drv8833_motor_t;

drv8833_err_t drv8833_init(void (*configure_peripherals)(void),
                           drv8833_motor_t *motor);

drv8833_err_t drv8833_set_sleep(drv8833_motor_t *motor, uint8_t sleep);

drv8833_err_t drv8833_set_motor_dutycycle(drv8833_motor_t *motor,
                                          uint32_t dutycycle_ain, 
                                          uint32_t dutycycle_bin);

#endif