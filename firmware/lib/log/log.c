#include "log.h"
#include "uart.h"

#include <stdio.h>
#include <stdarg.h>

#define LOG_BUFFER_SIZE 128

/*
 * Nível padrão:
 *
 * ERROR
 * WARNING
 * INFO
 *
 * DEBUG fica desabilitado inicialmente.
 */
static log_level_t current_level = LOG_LEVEL_INFO;


/*
 * Texto associado a cada nível.
 */
static const char *log_level_string(log_level_t level)
{
    switch (level)
    {
        case LOG_LEVEL_ERROR:
            return "ERROR";

        case LOG_LEVEL_WARNING:
            return "WARNING";

        case LOG_LEVEL_INFO:
            return "INFO";

        case LOG_LEVEL_DEBUG:
            return "DEBUG";

        default:
            return "UNKNOWN";
    }
}


void log_init(void)
{
    /*
     * A UART deve ser inicializada pelo usuário
     * antes desta função.
     *
     * Exemplo:
     *
     * usart1_init(25000000U, 115200U);
     */
    current_level = LOG_LEVEL_INFO;
}


void log_set_level(log_level_t level)
{
    if (level <= LOG_LEVEL_DEBUG)
    {
        current_level = level;
    }
}


log_level_t log_get_level(void)
{
    return current_level;
}


void log_write(log_level_t level, const char *format, ...)
{
    char buffer[LOG_BUFFER_SIZE];

    /*
     * Ignora mensagens acima do nível configurado.
     */
    if (level > current_level)
        return;

    /*
     * Monta a mensagem formatada.
     */
    va_list args;

    va_start(args, format);

    vsnprintf(
        buffer,
        sizeof(buffer),
        format,
        args
    );

    va_end(args);


    /*
     * Envia nível do log.
     */
    usart1_send_char('[');

    usart1_send_string(
        log_level_string(level)
    );

    usart1_send_string("] ");


    /*
     * Envia mensagem.
     */
    usart1_send_string(buffer);


    /*
     * Final da linha.
     */
    usart1_send_string("\r\n");
}