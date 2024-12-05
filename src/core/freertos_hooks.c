#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

/* Static memory for idle task */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

/* Static memory for timer task */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint16_t *puxIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint16_t *puxTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *puxTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vAssertCalled( const char * pcFile,
                    uint32_t ulLine )
{
    printf("ASSERT failed in %s, line %d\n", pcFile, ulLine);
    taskDISABLE_INTERRUPTS();
    abort();
}

void vApplicationMallocFailedHook( void )
{
    printf("Malloc failed!\n");
    taskDISABLE_INTERRUPTS();
    abort();
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,
                                    const char * pcTaskName )
{
    (void)xTask;
    printf("Stack overflow in task %s\n", pcTaskName);
    taskDISABLE_INTERRUPTS();
    abort();
}

void vApplicationTickHook( void )
{
    /* Called for each RTOS tick. */
}

void vApplicationIdleHook( void )
{
    /* Called when idle. */
}
