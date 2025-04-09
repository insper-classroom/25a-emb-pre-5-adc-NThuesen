#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"

QueueHandle_t xQueueData;

// Função que alimenta a fila com os dados do sinal
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

// Função para processar os dados e aplicar a média móvel de tamanho 5
void process_task(void *p) {
    int data = 0;
    int sum = 0;
    // Buffer circular para armazenar os últimos 5 dados
    static int buffer[5] = {0};
    // Índice de escrita para o buffer circular
    int write_idx = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, pdMS_TO_TICKS(100))) {
            // Subtrai o valor antigo que está na posição corrente
            sum -= buffer[write_idx];
            // Atualiza o buffer com o novo dado
            buffer[write_idx] = data;
            // Adiciona o novo dado à soma
            sum += data;
            // Atualiza o índice de forma circular
            write_idx = (write_idx + 1) % 5;
            
            // Calcula a média móvel
            int avg = sum / 5;
            printf("%d\n", avg);
            
            // Deixa o delay conforme solicitado
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    // Cria a fila com capacidade para 64 inteiros
    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
