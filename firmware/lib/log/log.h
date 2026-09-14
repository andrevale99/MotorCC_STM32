#ifndef LOG_H
#define LOG_H

#include <stdint.h>

typedef enum
{
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} log_level_t;


/*
 * Inicializa a biblioteca de log.
 *
 * O USART1 deve ter sido inicializado anteriormente.
 */
void log_init(void);


/*
 * Define o nível mínimo de log que será transmitido.
 *
 * Exemplo:
 *
 * LOG_LEVEL_INFO
 *
 * permite:
 * ERROR
 * WARNING
 * INFO
 *
 * mas ignora DEBUG.
 */
void log_set_level(log_level_t level);


/*
 * Retorna o nível atual de log.
 */
log_level_t log_get_level(void);


/*
 * Envia uma mensagem de log formatada.
 */
void log_write(log_level_t level, const char *format, ...);


/*
 * Macros para facilitar utilização.
 */
#define log_error(...)   log_write(LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_warning(...) log_write(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_info(...)    log_write(LOG_LEVEL_INFO, __VA_ARGS__)
#define log_debug(...)   log_write(LOG_LEVEL_DEBUG, __VA_ARGS__)

#endif