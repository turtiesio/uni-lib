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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/freertos_hooks.c
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
set(COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/components)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Create components library
add_library(components STATIC
    ${COMPONENTS_DIR}/button.c
)

target_link_libraries(components PUBLIC freertos)

if(ENABLE_TESTS)
    # Add Unity testing framework
    add_library(unity STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Unity/src/unity.c
    )
    target_include_directories(unity PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Unity/src
    )

    # Add test mocks
    add_library(test_mocks STATIC
        ${TESTS_DIR}/mocks/gpio_mock.c
    )

    # Add button test
    add_executable(test_button
        ${TESTS_DIR}/test_button.c
    )
    target_link_libraries(test_button PRIVATE
        unity
        components
        test_mocks
        freertos
    )

    # Enable testing
    enable_testing()
    add_test(NAME test_button COMMAND test_button)
endif()

# Add subdirectories based on target platform
if(BUILD_STM32)
    add_subdirectory(src/hal/stm32)
else()
    add_subdirectory(src/hal/linux)
endif()

# Add core library
add_subdirectory(src/core)

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
    # enable_testing() is already called above
    add_subdirectory(tests)
endif()
