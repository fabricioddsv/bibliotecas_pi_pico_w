// main.c

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Define o pino do LED para facilitar a modificação
#define LED_PIN 11

// Define o tamanho máximo para o buffer de comando
#define CMD_BUFFER_SIZE 256

/**
 * @brief Lê uma linha de caracteres da entrada padrão (USB serial).
 * A função é bloqueante e aguarda até que o usuário pressione Enter.
 * @param buffer O ponteiro para o buffer onde a string será armazenada.
 * @param buffer_len O tamanho do buffer.
 */
void read_serial_line(char *buffer, size_t buffer_len) {
    int char_index = 0;
    
    // Limpa o buffer antes de usar
    memset(buffer, 0, buffer_len);

    while (char_index < buffer_len - 1) {
        // getchar() aguarda até que um caractere seja recebido via USB
        char c = getchar();

        // Se o caractere for Enter ('\n' ou '\r'), terminamos a leitura
        if (c == '\n' || c == '\r') {
            break;
        }
        
        // Adiciona o caractere ao buffer
        buffer[char_index++] = c;
    }
    // Adiciona o terminador nulo para criar uma string C válida
    buffer[char_index] = '\0';
}


int main() {
    // Inicializa a E/S padrão (necessário para printf/getchar via USB)
    stdio_init_all();

    // Inicializa o pino do LED
    gpio_init(LED_PIN);
    // Configura o pino do LED como saída
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // Garante que o LED comece desligado
    gpio_put(LED_PIN, 0);

    // Pausa para dar tempo ao usuário de conectar o monitor serial
    sleep_ms(2000); 

    // Imprime instruções no monitor serial USB
    printf("===================================\n");
    printf(" Controle de LED via Serial USB\n");
    printf("===================================\n");
    printf("Comandos disponiveis:\n");
    printf("  /ON       - Liga o LED no pino %d\n", LED_PIN);
    printf("  /OFF      - Desliga o LED no pino %d\n", LED_PIN);
    printf("  /encerrar - Finaliza o programa\n");
    printf("Aguardando comandos...\n\n");

    char command_buffer[CMD_BUFFER_SIZE];
    
    // Loop principal infinito
    while (true) {
        // Chama a função para ler um comando do usuário
        read_serial_line(command_buffer, sizeof(command_buffer));

        // Se a linha não estiver vazia, processa o comando
        if (strlen(command_buffer) > 0) {
            printf("Comando recebido: '%s'\n", command_buffer);

            if (strcmp(command_buffer, "/ON") == 0) {
                printf("=> Acao: Ligando o LED.\n");
                gpio_put(LED_PIN, 1);
            } 
            else if (strcmp(command_buffer, "/OFF") == 0) {
                printf("=> Acao: Desligando o LED.\n");
                gpio_put(LED_PIN, 0);
            } 
            else if (strcmp(command_buffer, "/encerrar") == 0) {
                printf("=> Acao: Encerrando o programa.\n");
                break; // Sai do loop while(true)
            } 
            else {
                printf("=> Erro: Comando desconhecido.\n");
            }
            printf("\nAguardando comandos...\n");
        }
    }

    // Ações de finalização
    printf("\nPrograma encerrado. O LED foi desligado.\n");
    gpio_put(LED_PIN, 0); // Desliga o LED ao sair

    return 0; // O programa termina aqui
}