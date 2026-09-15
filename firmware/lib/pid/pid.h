#ifndef PID_H
#define PID_H

#include <stddef.h>
#include <stdint.h>

#include <stm32f411xe.h>

typedef enum
{
    PID_OK = 0,
    PID_ERR_INVALID_ARG = -1,
} pid_err_t;

typedef struct
{
    float kp;
    float ki;
    float kd;

    float P;
    float I;
    float D;

    float erro_anterior;

    float saturation;

    float output;
} _pid_t;

pid_err_t pid_clear(_pid_t *pid);

pid_err_t pid_control(_pid_t *pid, float setpoint,
                      float input, float dt);

#endif