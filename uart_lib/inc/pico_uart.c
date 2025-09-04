// pico_uart.c

#include "pico_uart.h"
#include "hardware/gpio.h"
#include <string.h> // Para strlen

void uart_lib_init(uart_inst_t *uart_id, uint baudrate, uint tx_pin, uint rx_pin) {
    // Inicializa a UART com o baudrate fornecido
    uart_init(uart_id, baudrate);

    // Define a função dos pinos GPIO para UART
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    // Desativa os controles de fluxo (geralmente não são necessários para projetos simples)
    uart_set_hw_flow(uart_id, false, false);
}

void uart_lib_send_line(uart_inst_t *uart_id, const char *str) {
    // uart_puts envia a string, mas não a nova linha
    uart_puts(uart_id, str);
    // Enviamos a nova linha e o retorno de carro para compatibilidade
    uart_puts(uart_id, "\r\n");
}

void uart_lib_read_line(uart_inst_t *uart_id, char *buffer, size_t buffer_len) {
    char c;
    size_t i = 0;

    // Zera o buffer para começar
    memset(buffer, 0, buffer_len);

    while (i < buffer_len - 1) {
        // uart_getc é bloqueante, espera por um caractere
        c = uart_getc(uart_id);

        // Se for retorno de carro ou nova linha, terminamos
        if (c == '\r' || c == '\n') {
            break;
        }

        buffer[i++] = c;
    }

    // Adiciona o terminador nulo para formar uma string C válida
    buffer[i] = '\0';
}