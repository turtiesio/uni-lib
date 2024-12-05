#ifndef UNI_LIB_GPIO_MOCK_H
#define UNI_LIB_GPIO_MOCK_H

#include "hal/gpio.h"

// Mock GPIO driver for testing
extern const gpio_driver_t gpio_mock_driver;

// Mock control functions
void gpio_mock_set_pin_state(uint32_t pin, bool state);
bool gpio_mock_get_pin_state(uint32_t pin);
void gpio_mock_reset(void);

#endif // UNI_LIB_GPIO_MOCK_H
