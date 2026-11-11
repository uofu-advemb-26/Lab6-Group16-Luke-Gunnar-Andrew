// #include <stdio.h>
// #include <pico/stdlib.h>
// #include <stdint.h>
// #include <unity.h>
// #include "unity_config.h"
// #include "busy.h"
// #include <FreeRTOS.h>
// #include "task.h"


// configRUN_TIME_COUNTER_TYPE run_time_counter_1, run_time_counter_2;
// TickType_t tick_count;



// void setUp(void) {}

// void tearDown(void) {
//     //printf("primary %lld secondary %lld elapsed %lld (us) in %d ticks\n",
//     //    first_stats, second_stats, elapsed_stats, elapsed_ticks);
// }

// void test_samePriority_busy(void)
// {
//     TaskHandle_t task_high;
//     TaskHandle_t task_high_2;

//     xTaskCreate(busy_busy, "busy_busy",
//         HIGH_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, &task_high);
//     xTaskCreate(busy_busy, "busy_busy",
//         HIGH_TASK_STACK_SIZE, NULL, HIGH_TASK_PRIORITY, &task_high_2);

// }

// void main_thread (void *args)
// {
//     while (1) {
//         UNITY_BEGIN();
//         RUN_TEST(test_samePriority_busy);
//         UNITY_END();
//         vTaskDelay(1000);
//     }
// }


// int main (void)
// {
//     stdio_init_all();
//     TaskHandle_t main_handle;
//     xTaskCreate(main_thread, "MainThread",
//                 configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+10, &main_handle);
//     vTaskStartScheduler();
    
//     return 0;
// }

#include <stdio.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include "unity_config.h"
#include <unity.h>
#include "busy.h"
#include <FreeRTOS.h>
#include "task.h"

#define TEST_DURATION_MS 1000

// Define task priorities relative to idle priority
#define LOW_PRIORITY     (tskIDLE_PRIORITY + 1)
#define HIGH_PRIORITY    (tskIDLE_PRIORITY + 2)
#define STACK_SIZE       configMINIMAL_STACK_SIZE

void setUp(void) {}

void tearDown(void) {}

// Helper function to run two tasks for a set duration and print their run-time counters
static void run_and_measure(TaskFunction_t func1, UBaseType_t prio1, const char* name1,
                              TaskFunction_t func2, UBaseType_t prio2, const char* name2)
{
    TaskHandle_t handle1 = NULL;
    TaskHandle_t handle2 = NULL;

    // Create tasks
    xTaskCreate(func1, name1, STACK_SIZE, NULL, prio1, &handle1);
    xTaskCreate(func2, name2, STACK_SIZE, NULL, prio2, &handle2);

    // Let them run for the test duration
    vTaskDelay(pdMS_TO_TICKS(TEST_DURATION_MS));

    // Capture run-time counters (requires configGENERATE_RUN_TIME_STATS = 1 in FreeRTOSConfig.h)
    configRUN_TIME_COUNTER_TYPE counter1 = ulTaskGetRunTimeCounter(handle1);
    configRUN_TIME_COUNTER_TYPE counter2 = ulTaskGetRunTimeCounter(handle2);

    // Clean up tasks
    if (handle1 != NULL) vTaskDelete(handle1);
    if (handle2 != NULL) vTaskDelete(handle2);

    // Allow idle task to clean up deleted tasks
    vTaskDelay(pdMS_TO_TICKS(10));

    printf("--- Test Results ---\n");
    printf("Task 1 (%s, Pri %lu): %llu ticks\n", name1, (unsigned long)prio1, (unsigned long long)counter1);
    printf("Task 2 (%s, Pri %lu): %llu ticks\n", name2, (unsigned long)prio2, (unsigned long long)counter2);
    printf("--------------------\n\n");
}

// --- Scenario 1: Same Priority ---
void test_samePriority_busy_busy(void)
{
    run_and_measure(busy_busy, LOW_PRIORITY, "busy_busy_1",
                    busy_busy, LOW_PRIORITY, "busy_busy_2");
}

void test_samePriority_yield_yield(void)
{
    run_and_measure(busy_yield, LOW_PRIORITY, "busy_yield_1",
                    busy_yield, LOW_PRIORITY, "busy_yield_2");
}

void test_samePriority_busy_yield(void)
{
    run_and_measure(busy_busy, LOW_PRIORITY, "busy_busy",
                    busy_yield, LOW_PRIORITY, "busy_yield");
}

// --- Scenario 2: Different Priority ---
void test_diffPriority_busy_higherFirst(void)
{
    // Higher priority created first
    run_and_measure(busy_busy, HIGH_PRIORITY, "busy_high",
                    busy_busy, LOW_PRIORITY, "busy_low");
}

void test_diffPriority_busy_lowerFirst(void)
{
    // Lower priority created first (testing if creation order changes preemption)
    run_and_measure(busy_busy, LOW_PRIORITY, "busy_low",
                    busy_busy, HIGH_PRIORITY, "busy_high");
}

void test_diffPriority_yield_yield(void)
{
    run_and_measure(busy_yield, HIGH_PRIORITY, "yield_high",
                    busy_yield, LOW_PRIORITY, "yield_low");
}

void main_thread(void *args)
{
    // Give hardware a moment to stabilize
    vTaskDelay(pdMS_TO_TICKS(500));

    UNITY_BEGIN();
    
    printf("\n=== 1. Same Priority Scenarios ===\n");
    RUN_TEST(test_samePriority_busy_busy);
    RUN_TEST(test_samePriority_yield_yield);
    RUN_TEST(test_samePriority_busy_yield);

    printf("\n=== 2. Different Priority Scenarios ===\n");
    RUN_TEST(test_diffPriority_busy_higherFirst);
    RUN_TEST(test_diffPriority_busy_lowerFirst);
    RUN_TEST(test_diffPriority_yield_yield);

    UNITY_END();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
    stdio_init_all();
    // Wait briefly for USB serial connection if debugging via UART/CDC
    sleep_ms(2000);

    TaskHandle_t main_handle;
    xTaskCreate(main_thread, "MainThread",
                configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 3, &main_handle);
    
    vTaskStartScheduler();
    
    return 0;
}