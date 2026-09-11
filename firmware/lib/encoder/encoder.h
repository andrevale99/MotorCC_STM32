#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stddef.h>

#include <stm32f411xe.h>

#define PI 3.14159265359

union velocity
{
    float rpm;
    float rads;
};

typedef enum
{
    ENCODER_OK = 0,
    ENCODER_ERROR_INVALID_ARGUMENT = -1,
} encoder_err_t;

typedef struct
{
    const uint32_t gearbox_ratio;
    const uint32_t pulses_hall;

    const float custom_gain;
} encoder_params_t;

typedef struct
{
    union velocity speed;

    float pulse;

    encoder_params_t gear_box;

} encoder_t;

encoder_err_t encoder_init(void (*configure)(void),
                           encoder_params_t *params);

encoder_err_t encoder_get_rpm(encoder_t *encoder, float dt_seconds);

encoder_err_t encoder_get_rads(encoder_t *encoder, float dt_seconds);
#endif