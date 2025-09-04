// pico_uart.h

#ifndef PICO_UART_H
#define PICO_UART_H

#include "hardware/uart.h"

/**
 * @brief Inicializa um periférico UART com os pinos e baudrate especificados.
 * * @param uart_id A instância do UART a ser usada (ex: uart0, uart1).
 * @param baudrate A taxa de transmissão em bits por segundo (ex: 9600).
 * @param tx_pin O número do pino GPIO para a transmissão (TX).
 * @param rx_pin O número do pino GPIO para a recepção (RX).
 */
void uart_lib_init(uart_inst_t *uart_id, uint baudrate, uint tx_pin, uint rx_pin);

/**
 * @brief Envia uma string de caracteres pela UART.
 * A função adicionará automaticamente os caracteres de nova linha e retorno de carro ('\r\n').
 * * @param uart_id A instância do UART a ser usada.
 * @param str A string (terminada em nulo) a ser enviada.
 */
void uart_lib_send_line(uart_inst_t *uart_id, const char *str);

/**
 * @brief Lê uma linha completa da UART (até encontrar '\\n').
 * Esta é uma função de bloqueio; ela esperará até que uma linha completa seja recebida.
 * * @param uart_id A instância do UART a ser usada.
 * @param buffer O buffer onde a string lida será armazenada.
 * @param buffer_len O tamanho máximo do buffer (para evitar overflow).
 */
void uart_lib_read_line(uart_inst_t *uart_id, char *buffer, size_t buffer_len);

#endif // PICO_UART_H