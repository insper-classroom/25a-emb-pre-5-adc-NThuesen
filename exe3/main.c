#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"

#define WINDOW_SIZE 5

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
    int buffer[WINDOW_SIZE] = {0};
    int index = 0;
    int count = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            buffer[index] = data;
            index = (index + 1) % WINDOW_SIZE;

            if (count < WINDOW_SIZE) count++;

            if (count == WINDOW_SIZE) {
                int sum = 0;
                for (int i = 0; i < WINDOW_SIZE; i++) {
                    sum += buffer[i];
                }
                int avg = sum / WINDOW_SIZE;
                printf("%d\n", avg);
            }

            // deixar esse delay!
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
