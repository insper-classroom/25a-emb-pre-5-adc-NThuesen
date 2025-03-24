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
        int newSample = 0;
    
        // Buffer para as 5 amostras mais recentes
        static int window[5] = {0, 0, 0, 0, 0};
        static int idx = 0;      // Índice de inserção
        static int count = 0;    // Quantidade de amostras já recebidas (até 5)
        static int sum = 0;      // Soma das últimas amostras da janela
    
        while (true) {
            // Espera até 100ms por uma nova amostra
            if (xQueueReceive(xQueueData, &newSample, pdMS_TO_TICKS(100))) {
                // Remove a amostra mais antiga da soma e substitui pela nova
                sum -= window[idx];      // subtrai valor antigo
                sum += newSample;        // adiciona valor novo
                window[idx] = newSample; // salva no buffer
                idx = (idx + 1) % 5;     // avança índice de forma circular
    
                // Se ainda não chegamos a 5 leituras, incrementa count
                if (count < 5) count++;
    
                // Calcula a média móvel
                int media = sum / count;
    
                // Imprime o valor filtrado
                printf("Entrada: %d, Filtro (media movel 5): %d\n", newSample, media);
    
                // Manter esse delay
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
