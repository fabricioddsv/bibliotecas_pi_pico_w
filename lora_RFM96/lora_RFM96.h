// lora_RFM95.h

#ifndef LORA_RFM95_H_
#define LORA_RFM95_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hardware/spi.h"

// ============================
// CONFIGURAÇÕES DE TEMPO (ms)
// ============================
#define TX_TIMEOUT_MS       5000   // tempo máximo esperando TxDone

// Struct de configuração para tornar a biblioteca mais portável
typedef struct {
    spi_inst_t *spi_instance;
    uint pin_miso;
    uint pin_cs;
    uint pin_sck;
    uint pin_mosi;
    uint pin_rst;
    uint pin_dio0;
    long frequency; // Frequência em Hz (ex: 915E6)
} lora_config_t;

/**
 * @brief Inicializa o módulo LoRa com as configurações fornecidas.
 * * @param config A struct com as configurações de pinos, SPI e frequência.
 * @return true se a inicialização foi bem-sucedida, false caso contrário.
 */
bool lora_init(lora_config_t config);

/**
 * @brief Envia uma mensagem de texto via LoRa.
 * * @param msg A mensagem a ser enviada (string terminada em nulo).
 * @return true se o envio foi iniciado com sucesso, false em caso de erro.
 */
bool lora_send(const char *msg);

/**
 * @brief Tenta receber uma mensagem LoRa. Função não bloqueante.
 * * @param buf Buffer para armazenar a mensagem recebida.
 * @param maxlen Tamanho máximo do buffer.
 * @return O número de bytes recebidos, ou 0 se nenhum pacote foi recebido.
 */
int lora_receive(char *buf, size_t maxlen);

/**
 * @brief Coloca o rádio em modo de recepção contínua.
 */
void lora_start_rx_continuous(void);


#endif // LORA_RFM95_H_