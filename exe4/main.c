/**
* Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/**
 * 0..1.0V: Desligado
 * 1..2.0V: 150 ms
 * 2..3.3V: 400 ms
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
 
const int PIN_LED_B = 4;
volatile int flag_timer_0 = 0;
volatile int flag_timer_1 = 1;

bool timer_0_callback(repeating_timer_t *rt) {
    flag_timer_0 = 1;
    return true; // keep repeating
}

bool timer_1_callback(repeating_timer_t *rt) {
    flag_timer_1 = 1;
    return true; // keep repeating
}

int main() {
    stdio_init_all();
    adc_init(); 

    adc_gpio_init(28);
    adc_select_input(1);

    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);

    repeating_timer_t timer_0;
    add_repeating_timer_ms(300, timer_0_callback, NULL, &timer_0);
    add_repeating_timer_ms(500, timer_1_callback, NULL, &timer_0);
    while (1) {
        const float conversion_factor = 3.3f / (1 << 12);
        uint16_t result = adc_read();
        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor); 

        if (result * conversion_factor >= 0 || conversion_factor < 1){
            // desligado
            gpio_put(PIN_LED_B, 0);
        }
        else if (result * conversion_factor >= 1 || result * conversion_factor < 2){
            // timer de 300ms
            if (flag_timer_0 == 1){
                
                gpio_put(PIN_LED_B, 1);
            }
        }
        else{
            // timer de 500ms
        }
    }
}
