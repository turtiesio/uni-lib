#ifndef UNI_LIB_BUTTON_H
#define UNI_LIB_BUTTON_H

#include "hal/gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Button event types
 */
typedef enum {
    BUTTON_EVENT_PRESSED,
    BUTTON_EVENT_RELEASED,
    BUTTON_EVENT_CLICKED,
    BUTTON_EVENT_HELD,
} button_event_t;

/**
 * Button configuration structure
 */
typedef struct {
    gpio_config_t gpio_config;
    uint32_t debounce_ms;     // Debounce time in milliseconds
    uint32_t long_press_ms;   // Time threshold for long press detection
    bool pull_up;             // true: pull-up (active low), false: pull-down (active high)
    QueueHandle_t event_queue; // Queue to send button events (optional)
} button_config_t;

/**
 * Button handle structure following OOP pattern
 */
typedef struct button_handle {
    // Hardware
    gpio_handle_t gpio;
    TimerHandle_t debounce_timer;
    QueueHandle_t event_queue;

    // Configuration
    uint32_t debounce_ms;
    uint32_t long_press_ms;

    // State
    TickType_t press_start_tick;
    bool last_state;
    bool is_pressed;
    bool is_debouncing;

    // Methods
    bool (*init)(struct button_handle *self, const button_config_t *config);
    void (*deinit)(struct button_handle *self);
    bool (*process)(struct button_handle *self, button_event_t *event);
} button_handle_t;

/**
 * Button driver interface
 */
typedef struct {
    bool (*create)(button_handle_t *handle);
    void (*destroy)(button_handle_t *handle);
} button_driver_t;

// Platform-specific driver implementation declarations
extern const button_driver_t button_driver;

#endif // UNI_LIB_BUTTON_H
