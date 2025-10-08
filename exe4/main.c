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
volatile int flag_timer_zona_1 = 0;
volatile int flag_timer_zona_2 = 0;

bool timer_0_callback(repeating_timer_t *rt) {
    flag_timer_zona_1 = 1;
    return true; // keep repeating
}

bool timer_1_callback(repeating_timer_t *rt) {
    flag_timer_zona_2 = 1;
    return true; // keep repeating
}

int main() {
    stdio_init_all();
    adc_init(); 

    adc_gpio_init(28);
    

    gpio_init(PIN_LED_B);
    gpio_set_dir(PIN_LED_B, GPIO_OUT);

    repeating_timer_t timer_0;
    add_repeating_timer_ms(300, timer_0_callback, NULL, &timer_0);
    add_repeating_timer_ms(500, timer_1_callback, NULL, &timer_0);
    
    int led_state = 0;
    // int zona =0;

    


    while (1) {
        adc_select_input(2);
        uint16_t result = adc_read();
        const float conversion_factor = 3.3f / (1 << 12);

        printf("Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor); 

        if (result * conversion_factor >= 0 && result * conversion_factor < 1){
            printf("Zona 0 \n");
            // desligado
            // zona = 0;
            gpio_put(PIN_LED_B, 0);
        }
        else if (result * conversion_factor >= 1 && result * conversion_factor < 2){
            printf("Zona 1 \n");
            if (flag_timer_zona_1 == 1) {
                flag_timer_zona_1 = 0;
                led_state = !led_state;
                gpio_put(PIN_LED_B, led_state);
            } 
            // timer de 300ms
            
        }
        else if(result * conversion_factor >= 2 && result * conversion_factor < 3.3){
            printf("Zona 2 \n");
            if (flag_timer_zona_2 == 1) {
                flag_timer_zona_2 = 0;
                led_state = !led_state;
                gpio_put(PIN_LED_B, led_state);
            } 
            // timer de 500ms
        }

        // if(zona == 1 && flag_timer_zona_1 == 1) {
        //     flag_timer_zona_1 = 0;
        //     // gpio put
        // } 

        // if(zona == 2 && flag_timer_zona_2 == 1) {
        //     flag_timer_zona_2 = 0;
        // }

        // if (zona == 0) {
        //     // apaga o led
        // }
    }
}
