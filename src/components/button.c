#include "components/button.h"
#include <stdlib.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"

// Forward declarations
static void button_debounce_callback(TimerHandle_t timer);

static bool button_init(button_handle_t *self, const button_config_t *config) {
    if (!self || !config) return false;

    // Initialize GPIO
    if (!self->gpio.init(&self->gpio, &config->gpio_config)) {
        return false;
    }
    
    // Store configuration
    self->debounce_ms = config->debounce_ms;
    self->long_press_ms = config->long_press_ms;
    self->press_start_tick = 0;
    self->last_state = false;
    self->is_pressed = false;
    self->is_debouncing = false;
    self->event_queue = config->event_queue;

    // Create debounce timer
    self->debounce_timer = xTimerCreate(
        "btn_debounce",
        pdMS_TO_TICKS(config->debounce_ms),
        pdFALSE,  // One-shot timer
        (void*)self,
        button_debounce_callback
    );

    if (self->debounce_timer == NULL) {
        self->gpio.deinit(&self->gpio);
        return false;
    }

    return true;
}

static void button_deinit(button_handle_t *self) {
    if (!self) return;
    
    if (self->debounce_timer) {
        xTimerDelete(self->debounce_timer, portMAX_DELAY);
    }
    
    self->gpio.deinit(&self->gpio);
}

static bool button_is_pressed(button_handle_t *self) {
    if (!self) return false;
    return self->gpio.is_active(&self->gpio);
}

static void button_debounce_callback(TimerHandle_t timer) {
    button_handle_t *self = (button_handle_t *)pvTimerGetTimerID(timer);
    if (!self) return;

    bool current_state = button_is_pressed(self);
    button_event_t event;

    if (current_state != self->last_state) {
        if (current_state) {
            // Button pressed
            self->is_pressed = true;
            self->press_start_tick = xTaskGetTickCount();
            event = BUTTON_EVENT_PRESSED;
        } else {
            // Button released
            self->is_pressed = false;
            TickType_t press_duration = xTaskGetTickCount() - self->press_start_tick;
            
            if (press_duration >= pdMS_TO_TICKS(self->long_press_ms)) {
                event = BUTTON_EVENT_HELD;
            } else {
                event = BUTTON_EVENT_CLICKED;
            }
        }

        self->last_state = current_state;

        // Send event if queue is configured
        if (self->event_queue) {
            xQueueSend(self->event_queue, &event, 0);
        }
    }

    self->is_debouncing = false;
}

static bool button_process(button_handle_t *self, button_event_t *event) {
    if (!self || !event) return false;

    bool current_state = button_is_pressed(self);

    // Check if state changed and not currently debouncing
    if (current_state != self->last_state && !self->is_debouncing) {
        self->is_debouncing = true;
        xTimerStart(self->debounce_timer, 0);
    }

    // Check for long press while button is held
    if (self->is_pressed && !self->is_debouncing) {
        TickType_t press_duration = xTaskGetTickCount() - self->press_start_tick;
        if (press_duration >= pdMS_TO_TICKS(self->long_press_ms)) {
            *event = BUTTON_EVENT_HELD;
            return true;
        }
    }

    return false;
}

bool button_create(button_handle_t *handle) {
    if (!handle) return false;

    // Initialize function pointers
    handle->init = button_init;
    handle->deinit = button_deinit;
    handle->is_pressed = button_is_pressed;
    handle->process = button_process;

    return true;
}

void button_destroy(button_handle_t *handle) {
    if (!handle) return;
    if (handle->deinit) {
        handle->deinit(handle);
    }
}

const button_driver_t button_driver = {
    .create = button_create,
    .destroy = button_destroy
};
