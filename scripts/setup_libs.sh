#!/bin/bash

# Script to setup third-party libraries for uni-lib
# Mainly handles FreeRTOS setup and configuration

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
THIRDPARTY_DIR="${PROJECT_ROOT}/thirdparty"
FREERTOS_DIR="${THIRDPARTY_DIR}/FreeRTOS-Kernel"
FREERTOS_TAG="V11.1.0"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Setting up third-party libraries...${NC}"

# Create thirdparty directory if it doesn't exist
mkdir -p "${THIRDPARTY_DIR}"

# Initialize git submodules if not already initialized
if [ ! -f "${PROJECT_ROOT}/.gitmodules" ]; then
    echo -e "${YELLOW}Initializing git submodules...${NC}"
    cd "${PROJECT_ROOT}"
    git init
    git submodule init
fi

# Add FreeRTOS as submodule if not already added
if [ ! -d "${FREERTOS_DIR}" ]; then
    echo -e "${YELLOW}Adding FreeRTOS-Kernel as submodule...${NC}"
    cd "${PROJECT_ROOT}"
    git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel.git "${THIRDPARTY_DIR}/FreeRTOS-Kernel"
    cd "${FREERTOS_DIR}"
    git checkout "${FREERTOS_TAG}"
    echo -e "${GREEN}FreeRTOS-Kernel submodule added and checked out to ${FREERTOS_TAG}${NC}"
else
    echo -e "${YELLOW}FreeRTOS-Kernel directory already exists, updating...${NC}"
    cd "${FREERTOS_DIR}"
    git fetch
    git checkout "${FREERTOS_TAG}"
    echo -e "${GREEN}FreeRTOS-Kernel updated to ${FREERTOS_TAG}${NC}"
fi

# Create FreeRTOS configuration for Linux simulation
FREERTOS_CONFIG_DIR="${PROJECT_ROOT}/include/config/freertos"
mkdir -p "${FREERTOS_CONFIG_DIR}"

echo -e "${YELLOW}Creating FreeRTOS configuration...${NC}"
cat > "${FREERTOS_CONFIG_DIR}/FreeRTOSConfig.h" << 'EOL'
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      60000000
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE               ((unsigned short)128)
#define configMAX_TASK_NAME_LEN                16
#define configUSE_16_BIT_TICKS                 0
#define configIDLE_SHOULD_YIELD                1
#define configUSE_TASK_NOTIFICATIONS           1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   3
#define configUSE_MUTEXES                      1
#define configUSE_RECURSIVE_MUTEXES            1
#define configUSE_COUNTING_SEMAPHORES          1
#define configQUEUE_REGISTRY_SIZE              8
#define configUSE_QUEUE_SETS                   1
#define configUSE_TIME_SLICING                 1
#define configUSE_NEWLIB_REENTRANT             0
#define configENABLE_BACKWARD_COMPATIBILITY    0
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5
#define configSTACK_DEPTH_TYPE                uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE      size_t

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION        1
#define configSUPPORT_DYNAMIC_ALLOCATION       1
#define configTOTAL_HEAP_SIZE                 ((size_t)(65536))
#define configAPPLICATION_ALLOCATED_HEAP      0

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                    0
#define configUSE_TICK_HOOK                    0
#define configCHECK_FOR_STACK_OVERFLOW         0
#define configUSE_MALLOC_FAILED_HOOK           0
#define configUSE_DAEMON_TASK_STARTUP_HOOK     0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS         0
#define configUSE_TRACE_FACILITY              1
#define configUSE_STATS_FORMATTING_FUNCTIONS  0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                 0
#define configMAX_CO_ROUTINE_PRIORITIES       2

/* Software timer related definitions. */
#define configUSE_TIMERS                      1
#define configTIMER_TASK_PRIORITY            (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH              10
#define configTIMER_TASK_STACK_DEPTH         configMINIMAL_STACK_SIZE

/* Define to trap errors during development. */
#define configASSERT(x) if((x) == 0) vAssertCalled(__FILE__, __LINE__)

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet              1
#define INCLUDE_uxTaskPriorityGet             1
#define INCLUDE_vTaskDelete                   1
#define INCLUDE_vTaskSuspend                  1
#define INCLUDE_xResumeFromISR                1
#define INCLUDE_vTaskDelayUntil               1
#define INCLUDE_vTaskDelay                    1
#define INCLUDE_xTaskGetSchedulerState        1
#define INCLUDE_xTaskGetCurrentTaskHandle     1
#define INCLUDE_uxTaskGetStackHighWaterMark   1
#define INCLUDE_xTaskGetIdleTaskHandle        1
#define INCLUDE_eTaskGetState                 1
#define INCLUDE_xEventGroupSetBitFromISR      1
#define INCLUDE_xTimerPendFunctionCall        1
#define INCLUDE_xTaskAbortDelay               1
#define INCLUDE_xTaskGetHandle                1
#define INCLUDE_xTaskResumeFromISR            1

/* Interrupt nesting behaviour configuration. */
#define configKERNEL_INTERRUPT_PRIORITY         255
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    191
#define configMAX_API_CALL_INTERRUPT_PRIORITY   191

#endif /* FREERTOS_CONFIG_H */
EOL

echo -e "${GREEN}FreeRTOS configuration created at ${FREERTOS_CONFIG_DIR}/FreeRTOSConfig.h${NC}"

# Make the script executable
chmod +x "${SCRIPT_DIR}/setup_libs.sh"

echo -e "${GREEN}Third-party library setup completed!${NC}"