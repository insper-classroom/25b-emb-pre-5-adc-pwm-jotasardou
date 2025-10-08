#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int buf[5] = {0};      // buffer circular
    int idx = 0;           // posição atual para sobrescrever (0..4)
    int count = 0;         // quantas amostras já temos (até 5)
    int sum = 0;           // soma das amostras no buffer

    while (true) {
        // espera por dado (bloqueia até 100 ticks)
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100))) {
            if (count < 5) {
                // ainda enchendo a janela
                buf[idx] = data;
                sum += data;
                idx = (idx + 1) % 5;
                count++;
                if (count == 5) {
                    // primeira média: imprime imediatamente
                    int avg = sum / 5;
                    printf("%d\n", avg);
                }
            } else {
                // janela cheia: remove o velho, coloca o novo, atualiza soma
                sum -= buf[idx];      // subtrai elemento que sai
                buf[idx] = data;      // insere novo elemento
                sum += data;          // adiciona novo à soma
                idx = (idx + 1) % 5;  // move índice circular
                int avg = sum / 5;
                printf("%d\n", avg);
            }

            // opcional: se você precisa desse delay por causa do enunciado,
            // mantenha; caso contrário remova para processar o mais rápido possível.
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
