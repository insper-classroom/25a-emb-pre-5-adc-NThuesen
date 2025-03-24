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
    int sum = 0;
    static int buffer[5] = {0, 0, 0, 0, 0};
    int i = 0;
    int j = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
            buffer[i-j] = data;
            printf("valor de buffer[i]: %d \n", buffer[i]);
            if (j > 0){
                sum = (buffer[i-j] + buffer[i-j-1] + buffer[i-j-2] + buffer[i-j-3] + buffer[i-j-4])/5;
                printf("if j: %d \n",sum);
                j ++;
            }
            else if (i == 4){
                sum = (buffer[i] + buffer[i-1] + buffer[i-2] + buffer[i-3] + buffer[i-4])/5;
                printf("if i: %d \n",sum);
                j = 1;
            }
            i++;
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
