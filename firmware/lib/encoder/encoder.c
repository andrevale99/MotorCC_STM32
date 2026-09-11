#include "encoder.h"

static float pulses_per_revolution_calc(encoder_t *encoder)
{
    if (encoder == NULL)
        return 0.0f;

    if (encoder->gear_box.custom_gain > 0.0f)
        return encoder->gear_box.custom_gain;

    return (float)encoder->gear_box.pulses_hall *
           encoder->gear_box.gearbox_ratio;
}

encoder_err_t encoder_init(void (*configure)(void),
                           encoder_params_t *params)
{
    if (configure == NULL || params == NULL)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    configure();

    return ENCODER_OK;
}

encoder_err_t encoder_get_rpm(encoder_t *encoder, float dt_seconds)
{
    if (encoder == NULL)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    if (dt_seconds <= 0.0f)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    float ppr = pulses_per_revolution_calc(encoder);

    if (ppr <= 0.0f)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    encoder->speed.rpm =
        ((float)encoder->pulse / ppr) *
        60.0f /
        dt_seconds;

    return ENCODER_OK;
}

encoder_err_t encoder_get_rads(encoder_t *encoder, float dt_seconds)
{
    if (encoder == NULL)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    if (dt_seconds <= 0.0f)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    float ppr = pulses_per_revolution_calc(encoder);

    if (ppr <= 0.0f)
        return ENCODER_ERROR_INVALID_ARGUMENT;

    encoder->speed.rads =
        ((float)encoder->pulse / ppr) *
        2.0f * PI /
        dt_seconds;

    return ENCODER_OK;
}