#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <unity.h>
#include "unity_config.h"
#include "busy.h"
#include <FreeRTOS.h>

configRUN_TIME_COUNTER_TYPE run_time_counter_1, run_time_counter_2;
TickType_t tick_count;



void setUp(void) {}

void tearDown(void) {
    printf("primary %lld secondary %lld elapsed %lld (us) in %d ticks\n",
        first_stats, second_stats, elapsed_stats, elapsed_ticks);
}

void test_samePriority_busy(void)
{
    TaskHandle_t task_high;
    TaskHandle_t task_high_2;

    xTaskCreate(busy_busy, "busy_busy",
        HIGH_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, &task_high);
    xTaskCreate(busy_busy, "busy_busy",
        HIGH_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, &task_high_2);

}

void main_thread (void *args)
{
    while (1) {
        UNITY_BEGIN();
        RUN_TEST(test_samePriority_busy);
        UNITY_END();
        vTaskDelay(1000);
    }
}


int main (void)
{
    stdio_init_all();
    TaskHandle_t main_handle;
    xTaskCreate(main_thread, "MainThread",
                configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+10, &main_handle);
    vTaskStartScheduler();
    return 0;
}
