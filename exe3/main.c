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

int media_movel(int *buffer){
    int soma = 0;
    for (int i = 0; i < 5; i++) {
        soma += buffer[i];
    }
    return soma / 5;

}

void process_task(void *p) {
    int buffer[5] = {0}; // Últimos 5 valores
    int index = 0;

    while (true) {
        int data;
        if (xQueueReceive(xQueueData, &data, 100)) {
            buffer[index] = data;
            index = (index + 1) % 5;
            int media = media_movel(buffer);
            printf("%d\n", media);

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
