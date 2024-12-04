cmake_minimum_required(VERSION 3.15)
project(uni-lib C)

# Options
option(BUILD_STM32 "Build for STM32 target" OFF)
option(ENABLE_TESTS "Enable testing" ON)
option(ENABLE_EXAMPLES "Build examples" ON)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add compile options
add_compile_options(-Wall -Wextra)

# FreeRTOS paths
set(FREERTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/FreeRTOS-Kernel)
set(FREERTOS_PORT_DIR ${FREERTOS_DIR}/portable/ThirdParty/GCC/Posix)
set(FREERTOS_HEAP ${FREERTOS_DIR}/portable/MemMang/heap_3.c)

# Create FreeRTOS library
add_library(freertos STATIC
    ${FREERTOS_DIR}/tasks.c
    ${FREERTOS_DIR}/list.c
    ${FREERTOS_DIR}/queue.c
    ${FREERTOS_DIR}/timers.c
    ${FREERTOS_DIR}/event_groups.c
    ${FREERTOS_DIR}/stream_buffer.c
    ${FREERTOS_HEAP}
    ${FREERTOS_PORT_DIR}/port.c
    ${FREERTOS_PORT_DIR}/utils/wait_for_event.c
)

target_include_directories(freertos PUBLIC
    ${FREERTOS_DIR}/include
    ${FREERTOS_PORT_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/config/freertos
)

target_link_libraries(freertos PUBLIC pthread)

# Define source directories
set(CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/core)
set(HAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/hal)
set(DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/drivers)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Add subdirectories based on target platform
if(BUILD_STM32)
    add_subdirectory(src/hal/stm32)
else()
    add_subdirectory(src/hal/linux)
endif()

# Add core library
add_subdirectory(src/core)
add_subdirectory(src/drivers)

# Build examples if enabled
if(ENABLE_EXAMPLES)
    if(BUILD_STM32)
        add_subdirectory(examples/stm32)
    else()
        add_subdirectory(examples/linux)
    endif()
endif()

# Build tests if enabled
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
