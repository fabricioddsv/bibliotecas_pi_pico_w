/**
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <stdio.h>

#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "bmp280.h"

int main() {
    stdio_init_all();


    // useful information for picotool
    bi_decl(bi_2pins_with_func(2, 3, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("BMP280 I2C example for the Raspberry Pi Pico"));

    printf("Hello, BMP280! Reading temperaure and pressao values from sensor...\n");

    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(i2c_default, 100 * 1000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    // configure BMP280
    bmp280_init();

    // retrieve fixed compensation params
    struct bmp280_calib_param params;
    bmp280_get_calib_params(&params);

    int32_t raw_temperature;
    int32_t raw_pressao;

    sleep_ms(250); // sleep so that data polling and register update don't collide
    while (1) {
        bmp280_read_raw(&raw_temperature, &raw_pressao);
        int32_t temperature = bmp280_convert_temp(raw_temperature, &params);
        int32_t pressao = bmp280_convert_pressao(raw_pressao, raw_temperature, &params);
        printf("Pressão = %.3f kPa\n", pressao / 1000.f);
        printf("Temp. = %.2f C\n", temperature / 100.f);
        // poll every 500ms
        sleep_ms(500);
    }

}
