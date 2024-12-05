#include "gpio_mock.h"
#include <string.h>

#define MAX_PINS 64

static struct {
    bool pin_states[MAX_PINS];
    bool initialized[MAX_PINS];
} mock_data;

static bool gpio_mock_init(gpio_handle_t *self, const gpio_config_t *config) {
    if (!self || !config || config->pin >= MAX_PINS) return false;
    mock_data.initialized[config->pin] = true;
    return true;
}

static bool gpio_mock_deinit(gpio_handle_t *self) {
    return true;
}

static bool gpio_mock_write(gpio_handle_t *self, bool state) {
    if (!self || !self->hw_handle) return false;
    uint32_t pin = *(uint32_t*)self->hw_handle;
    if (pin >= MAX_PINS) return false;
    mock_data.pin_states[pin] = state;
    return true;
}

static bool gpio_mock_read(gpio_handle_t *self) {
    if (!self || !self->hw_handle) return false;
    uint32_t pin = *(uint32_t*)self->hw_handle;
    if (pin >= MAX_PINS) return false;
    return mock_data.pin_states[pin];
}

static bool gpio_mock_create(gpio_handle_t *handle) {
    if (!handle) return false;
    
    static uint32_t pin_number;
    handle->hw_handle = &pin_number;
    
    handle->init = gpio_mock_init;
    handle->deinit = gpio_mock_deinit;
    handle->write = gpio_mock_write;
    handle->read = gpio_mock_read;
    
    return true;
}

static void gpio_mock_destroy(gpio_handle_t *handle) {
    // Nothing to clean up
}

const gpio_driver_t gpio_mock_driver = {
    .create = gpio_mock_create,
    .destroy = gpio_mock_destroy
};

// Mock control functions
void gpio_mock_set_pin_state(uint32_t pin, bool state) {
    if (pin < MAX_PINS) {
        mock_data.pin_states[pin] = state;
    }
}

bool gpio_mock_get_pin_state(uint32_t pin) {
    if (pin < MAX_PINS) {
        return mock_data.pin_states[pin];
    }
    return false;
}

void gpio_mock_reset(void) {
    memset(&mock_data, 0, sizeof(mock_data));
}
