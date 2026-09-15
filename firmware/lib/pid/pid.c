#include "pid.h"

pid_err_t pid_clear(_pid_t *pid)
{
    if (pid == NULL)
        return PID_ERR_INVALID_ARG;

    pid->P = 0.0f;
    pid->I = 0.0f;
    pid->D = 0.0f;

    pid->erro_anterior = 0.0f;

    pid->output = 0.0f;

    pid->saturation = 0;

    return PID_OK;
}

pid_err_t pid_control(_pid_t *pid, float setpoint,
                      float input, float dt)
{
    if (pid == NULL)
        return PID_ERR_INVALID_ARG;

    float erro = setpoint - input;

    /*
     * Termo proporcional
     */
    pid->P = pid->kp * erro;

    /*
     * Termo derivativo
     */
    pid->D = pid->kd *
             (erro - pid->erro_anterior) /
             dt;

    /*
     * Atualiza o erro anterior
     */
    pid->erro_anterior = erro;

    /*
     * Calcula uma saída sem o integrador
     */
    float output_sem_I = pid->P + pid->D;

    /*
     * Integra somente se a saída não estiver
     * saturada na direção do erro.
     */
    if ((output_sem_I < pid->saturation && erro > 0.0f) ||
        (output_sem_I > -pid->saturation && erro < 0.0f))
    {
        pid->I += pid->ki * erro * dt;
    }

    /*
     * Saída do PID
     */
    pid->output = pid->P +
                  pid->I +
                  pid->D;

    /*
     * Saturação
     */
    if (pid->output > pid->saturation)
    {
        pid->output = pid->saturation;
    }
    else if (pid->output < -pid->saturation)
    {
        pid->output = -pid->saturation;
    }

    return PID_OK;
}