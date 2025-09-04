/*
 * hc_sr04.h - Biblioteca para o sensor ultrassônico HC-SR04.
 * Baseado no datasheet: HC-SR04 Ultrasonic Sensor by Elijah J. Morgan, Nov. 16 2014
 */

#ifndef HC_SR04_H
#define HC_SR04_H

// Inclui o SDK do Pico para tipos de dados como uint
#include "pico/stdlib.h"

// Estrutura para manter as informações de pino para um sensor HC-SR04
typedef struct {
    uint trigger_pin;
    uint echo_pin;
} hc_sr04_t;

/**
 * [cite_start]@brief Inicializa os pinos para o sensor HC-SR04[cite: 61, 62, 63, 64, 67].
 *
 * Configura o pino TRIG como saída e o pino ECHO como entrada.
 * Esta função deve ser chamada uma vez antes de usar o sensor.
 *
 * @param sensor Ponteiro para a estrutura do sensor hc_sr04_t.
 * @param trigger_pin O número do pino GPIO conectado ao pino TRIG do sensor.
 * @param echo_pin O número do pino GPIO conectado ao pino ECHO do sensor.
 */
void hc_sr04_init(hc_sr04_t *sensor, uint trigger_pin, uint echo_pin);

/**
 * @brief Realiza uma medição de distância.
 *
 * [cite_start]Envia um pulso de 10us ao pino TRIG e mede a duração do pulso de retorno no pino ECHO[cite: 55, 65, 68].
 * [cite_start]Converte a duração do pulso em uma distância em centímetros[cite: 97].
 *
 * @param sensor Ponteiro para a estrutura do sensor hc_sr04_t.
 * @return A distância medida em centímetros (cm). Retorna um valor negativo em caso de erro/timeout.
 */
float hc_sr04_get_distance_cm(hc_sr04_t *sensor);

#endif // HC_SR04_H