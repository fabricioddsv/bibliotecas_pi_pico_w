/*
 * hc_sr04.c - Implementação da biblioteca para o sensor ultrassônico HC-SR04
 * para o Raspberry Pi Pico.
 */

#include "hc_sr04.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Fator de conversão de microssegundos para centímetros[cite: 97].
// O tempo medido (em µs) dividido por este valor resulta na distância (em cm).
const float US_TO_CM_DIVISOR = 58.0;

void hc_sr04_init(hc_sr04_t *sensor, uint trigger_pin, uint echo_pin) {
    sensor->trigger_pin = trigger_pin;
    sensor->echo_pin = echo_pin;

    // Inicializa os pinos GPIO
    gpio_init(sensor->trigger_pin);
    gpio_init(sensor->echo_pin);

    // Configura a direção dos pinos
    gpio_set_dir(sensor->trigger_pin, GPIO_OUT); // TRIG é saída [cite: 64]
    gpio_set_dir(sensor->echo_pin, GPIO_IN);    // ECHO é entrada [cite: 67]
}

float hc_sr04_get_distance_cm(hc_sr04_t *sensor) {
    // Garante que o pino de trigger esteja em nível baixo para começar
    gpio_put(sensor->trigger_pin, 0);
    sleep_us(2);

    // Envia o pulso de trigger de 10 microssegundos para iniciar a medição[cite: 55, 65].
    gpio_put(sensor->trigger_pin, 1);
    sleep_us(10);
    gpio_put(sensor->trigger_pin, 0);

    // Aguarda o pino de echo ficar em nível alto[cite: 66].
    // Adicionado um timeout para evitar loops infinitos se não houver objeto.
    // O pulso pode levar até ~24ms para um objeto a 4m. Usamos 30ms como timeout seguro.
    uint64_t timeout_start = time_us_64();
    while (!gpio_get(sensor->echo_pin)) {
        if ((time_us_64() - timeout_start) > 30000) {
            return -1.0; // Erro: Timeout esperando o início do pulso
        }
    }

    // Mede a duração do pulso de echo.
    uint64_t pulse_start_time = time_us_64();
    while (gpio_get(sensor->echo_pin)) {
        if ((time_us_64() - pulse_start_time) > 30000) {
            return -2.0; // Erro: Timeout durante o pulso
        }
    }
    uint64_t pulse_end_time = time_us_64();

    uint64_t pulse_duration = pulse_end_time - pulse_start_time;

    // A distância em cm é a duração do pulso em µs dividida por 58[cite: 97].
    float distance_cm = (float)pulse_duration / US_TO_CM_DIVISOR;

    return distance_cm;
}