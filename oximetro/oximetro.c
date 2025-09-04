/*
=================================================================
 SENSOR DE BATIMENTOS CARDÍACOS E OXÍMETRO COM MAX30102 (VERSÃO CORRIGIDA)
=================================================================

Este código é a versão original do projeto, corrigida para resolver
um erro na fórmula de cálculo de BPM e para tornar a detecção de picos
mais robusta.

Este código é o correto para o seu sensor, que se identificou com
o Part ID 0x15 (MAX30102).
*/

// Bibliotecas inclusas
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Definições de I2C
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

#define MAX30102_ADDR   0x57

// Lista de endereços de registradores
#define REG_INTR_STATUS_1   0x00
#define REG_INTR_ENABLE_1   0x02
#define REG_FIFO_WR_PTR     0x04
#define REG_FIFO_OVF_CNT    0x05
#define REG_FIFO_RD_PTR     0x06
#define REG_FIFO_DATA       0x07
#define REG_FIFO_CONFIG     0x08
#define REG_MODE_CONFIG     0x09
#define REG_SPO2_CONFIG     0x0A
#define REG_LED1_PA         0x0C    // Corrente do LED RED
#define REG_LED2_PA         0x0D    // Corrente do LED IR
#define REG_PART_ID         0xFF    // Deve retornar 0x15 para o MAX30102

/*
--- CONFIGURAR I2C ---
*/
void config_i2c() {
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

/*
--- ESCRITA DO SENSOR MAX30102 ---
*/
static inline void max30102_write(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = {reg, val};
    i2c_write_blocking(I2C_PORT, MAX30102_ADDR, buf, 2, false);
}

/*
--- LEITURA DO SENSOR MAX30102 ---
*/
static inline uint8_t max30102_read(uint8_t reg) {
    uint8_t val;
    i2c_write_blocking(I2C_PORT, MAX30102_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, MAX30102_ADDR, &val, 1, false);
    return val;
}

/*
--- INICIALIZAÇÃO DO SENSOR ---
*/
void max30102_init(void) {
    // Soft reset
    max30102_write(REG_MODE_CONFIG, 0x40);
    sleep_ms(100);

    // Limpar ponteiros do buffer FIFO
    max30102_write(REG_FIFO_WR_PTR, 0x00);
    max30102_write(REG_FIFO_OVF_CNT, 0x00);
    max30102_write(REG_FIFO_RD_PTR, 0x00);

    // Configuração FIFO:
    //   - Média de amostras = 4
    //   - Rollover habilitado
    //   - Interrupção quando FIFO tiver 15 amostras (17 espaços vazios)
    max30102_write(REG_FIFO_CONFIG, (0b010 << 5) | (1 << 4) | 0x0F);

    // Configuração de SpO2:
    //   - Resolução do ADC: 4096 nA
    //   - Taxa de amostragem de 100 Hz
    //   - Largura de pulso: 411 µs (18 bits de resolução)
    max30102_write(REG_SPO2_CONFIG, (0b01 << 5) | (0b011 << 2) | 0b11);

    // Amplitudes de operação dos LEDs (~7.2 mA -> 0x24)
    max30102_write(REG_LED1_PA, 0x24);  // RED
    max30102_write(REG_LED2_PA, 0x24);  // IR

    // Habilitação do modo SpO2 (RED e IR)
    max30102_write(REG_MODE_CONFIG, 0x03);
}

/*
--- LEITURA DE AMOSTRA RED/IR ---
    Lê 6 bytes do buffer (3 para RED e 3 para IR) e constrói
    inteiros de 18 bits.
*/
bool max30102_read_sample(uint32_t *red, uint32_t *ir) {
    uint8_t wr = max30102_read(REG_FIFO_WR_PTR);
    uint8_t rd = max30102_read(REG_FIFO_RD_PTR);
    if (wr == rd) return false;

    uint8_t data[6];
    uint8_t reg = REG_FIFO_DATA;
    i2c_write_blocking(I2C_PORT, MAX30102_ADDR, &reg, 1, true);
    i2c_read_blocking (I2C_PORT, MAX30102_ADDR, data, 6, false);

    // 3 bytes por amostra -> Mascarar para 18 bits
    *red = ((uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | data[2]) & 0x3FFFF;
    *ir  = ((uint32_t)data[3] << 16 | (uint32_t)data[4] << 8 | data[5]) & 0x3FFFF;
    return true;
}

#define SAMPLE_SIZE 100
uint32_t red_buffer[SAMPLE_SIZE];
uint32_t ir_buffer[SAMPLE_SIZE];

/*
--- CÁLCULO DE OXIGENAÇÃO (SPO2) ---
*/
float calculate_spo2(uint32_t *red, uint32_t *ir) {
    float red_ac = 0, ir_ac = 0;
    float red_dc = 0, ir_dc = 0;
    float ratio = 0.0f;

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        red_dc += red[i];
        ir_dc  += ir[i];
    }
    red_dc /= SAMPLE_SIZE;
    ir_dc  /= SAMPLE_SIZE;

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        red_ac += fabsf((float)red[i] - red_dc);
        ir_ac  += fabsf((float)ir[i] - ir_dc);
    }
    red_ac /= SAMPLE_SIZE;
    ir_ac  /= SAMPLE_SIZE;

    if (ir_dc != 0 && ir_ac != 0) {
        ratio = (red_ac / red_dc) / (ir_ac / ir_dc);
    }
    
    float spo2 = 110.0f - 25.0f * ratio;

    if (spo2 > 100.0f) spo2 = 100.0f;
    if (spo2 < 80.0f) spo2 = 80.0f;
    return spo2;
}

/*
--- CÁLCULO DE FREQUÊNCIA CARDÍACA (BPM) - VERSÃO CORRIGIDA ---
*/
float calculate_bpm(uint32_t *ir, float sample_rate_hz) {
    uint64_t ir_sum = 0;
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        ir_sum += ir[i];
    }
    uint32_t ir_mean = ir_sum / SAMPLE_SIZE;

    int peak_count = 0;
    for (int i = 1; i < SAMPLE_SIZE - 1; i++) {
        if (ir[i] > ir[i - 1] && ir[i] > ir[i + 1] && ir[i] > ir_mean) {
            peak_count++;
            i += 15;
        }
    }

    float duration_sec = (float)SAMPLE_SIZE / sample_rate_hz;
    return (peak_count / duration_sec) * 60.0f;
}

/*
--- FUNÇÃO PRINCIPAL ---
*/
int main(void) {
    stdio_init_all();
    sleep_ms(2000);

    config_i2c();
    max30102_init();

    // Verifica o Part ID para o MAX30102
    uint8_t part_id = max30102_read(REG_PART_ID);
    if (part_id == 0x15) {
        printf("MAX30102 pronto (Part ID: 0x%02X)\n", part_id);
    } else {
        printf("Erro: Part ID inesperado (0x%02X). Verifique a conexao.\n", part_id);
        while(1);
    }

    const float sample_rate = 100.0f;

    while (true) {
        int samples_collected = 0;
        while (samples_collected < SAMPLE_SIZE) {
            uint32_t red, ir;
            if (max30102_read_sample(&red, &ir)) {
                red_buffer[samples_collected] = red;
                ir_buffer[samples_collected] = ir;
                samples_collected++;
            }
            sleep_ms(10);
        }

        float bpm = calculate_bpm(ir_buffer, sample_rate);
        float spo2 = calculate_spo2(red_buffer, ir_buffer);

        if (bpm > 40 && bpm < 220) {
           printf("BPM: %.1f, SpO2: %.1f%%\n", bpm, spo2);
        } else {
           printf("Calculando... Posicione o dedo firmemente.\n");
        }
    }
}