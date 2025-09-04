#include <stdio.h>
#include "pico/stdlib.h"
#include "hc_sr04.h"

// Define os pinos GPIO para o sensor
#define TRIGGER_PIN 8
#define ECHO_PIN    9

int main() {
    // Inicializa o stdio para que possamos usar printf via USB Serial
    stdio_init_all();

    // Aguarda um pouco para a porta serial ser estabelecida
    sleep_ms(2000);
    
    printf("Iniciando exemplo do sensor HC-SR04 para Raspberry Pi Pico...\n");

    // Cria uma instância do nosso sensor
    hc_sr04_t sensor;

    // Inicializa o sensor com os pinos definidos.
    hc_sr04_init(&sensor, TRIGGER_PIN, ECHO_PIN);

    while (1) {
        // Realiza a medição
        float distance = hc_sr04_get_distance_cm(&sensor);

        if (distance >= 0) {
            // A faixa de medição útil do sensor é de 2cm a 400cm[cite: 52].
            if (distance > 400) {
                printf("Distancia: Fora de alcance (> 400 cm)\n");
            } else {
                printf("Distancia: %.2f cm\n", distance);
            }
        } else {
            // Imprime o erro
            printf("Erro na leitura do sensor (codigo: %.0f)\n", distance);
        }

        // Espera um pouco antes da próxima leitura para evitar ecos sobrepostos.
        sleep_ms(1000);
    }

    return 0;
}