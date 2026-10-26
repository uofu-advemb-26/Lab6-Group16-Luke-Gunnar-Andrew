/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"
#include "semphr.h"


#define HIGH_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MEDIUM_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )
#define LOW_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3UL )
#define HIGH_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define MEDIUM_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define LOW_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

SemaphoreHandle_t Semaphore;


void task_high(__unused void *params) {
    vTaskDelay(100);
    xSemaphoreTake(Semaphore, portMAX_DELAY);
    printf("High priority task running\n");
    xSemaphoreGive(Semaphore);

}
void task_low(__unused void *params) {
    xSemaphoreTake(Semaphore, portMAX_DELAY);
    printf("Low priority task running\n");
    xSemaphoreGive(Semaphore);
}

void task_medium(__unused void *params) {
    printf("Medium priority task running\n");
    while(1) {

    }

}

void task_supervisor(__unused void *params) {
    while(1) {
        printf("Supervisor task running\n");
        vTaskDelay(400);
    }

}

int main( void )
{
    stdio_init_all();
    const char *rtos_name;
    rtos_name = "FreeRTOS";
    TaskHandle_t task_high;
    TaskHandle_t task_low;
    TaskHandle_t task_medium;
    TaskHandle_t task_supervisor;

    Semaphore = xSemaphoreCreateCounting(1, 1);
    xTaskCreate(task_high, "TaskHigh",
        HIGH_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, &task_high);
    xTaskCreate(task_low, "TaskLow",
        LOW_TASK_STACK_SIZE, NULL, LOW_TASK_PRIORITY, &task_low);
    xTaskCreate(task_medium, "TaskMedium",
        MEDIUM_TASK_STACK_SIZE, NULL, MEDIUM_TASK_PRIORITY, &task_medium);
    xTaskCreate(task_supervisor, "TaskSupervisor",
        HIGH_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY, &task_supervisor);
    vTaskStartScheduler();
    return 0;
}
