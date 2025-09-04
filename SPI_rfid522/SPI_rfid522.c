#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pico-mfrc522/mfrc522.h"

#define PIN_TESTE  5

#define DEBOUNCE_TIME_US 50000 // 50ms em microssegundos

int tag = 0;
volatile absolute_time_t ultimaInterrupcao;

void gpio_callback(uint gpio, uint32_t events);

void main() {
    stdio_init_all();

    // Declaração dos UIDs das tags válidas
    uint8_t valid_tags[][4] = {
        {0xA3, 0x81, 0x7B, 0x97}  // Tag 2 (exemplo de nova tag)
    };
    int num_valid_tags = sizeof(valid_tags) / sizeof(valid_tags[0]);

    MFRC522Ptr_t mfrc = MFRC522_Init();
    PCD_Init(mfrc, spi0);

    sleep_ms(5000);

    gpio_init(PIN_TESTE);
    gpio_set_dir(PIN_TESTE, GPIO_IN);
    gpio_pull_up(PIN_TESTE);
    
    gpio_set_irq_enabled_with_callback(PIN_TESTE, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while(1) {
        // Aguarda por uma nova tag
        printf("Aguardando por uma tag...\n\r");
        while(!PICC_IsNewCardPresent(mfrc));
        // Seleciona a tag
        printf("Selecionando a tag...\n\r");
        PICC_ReadCardSerial(mfrc);

        // Exibe o UID no console
        printf("UID da tag: ");
        for(int i = 0; i < mfrc->uid.size; i++) {
            printf("%02X ", mfrc->uid.uidByte[i]);
        }
        printf("\n\r");

        // Verifica se o UID lido corresponde a uma tag válida
        int is_valid = 0;
        for(int i = 0; i < num_valid_tags; i++) {
            if(memcmp(mfrc->uid.uidByte, valid_tags[i], 4) == 0) {
                is_valid = 1;
                break;
            }
        }

        if(is_valid) {
            printf("Autenticação bem-sucedida\n\r");
            
        } else {
            printf("Autenticação falhou\n\r");
        }

        if (tag==1) {
            tag = 0;
            printf("ok\n\r");
        }

        sleep_ms(500);
    }
}

void gpio_callback(uint gpio, uint32_t events) {
    //sleep_ms(50);
    absolute_time_t tempoAtual = get_absolute_time();
    if (absolute_time_diff_us(ultimaInterrupcao, tempoAtual) > DEBOUNCE_TIME_US) {
        tag = 1;
        ultimaInterrupcao = tempoAtual;
    }
    
}
