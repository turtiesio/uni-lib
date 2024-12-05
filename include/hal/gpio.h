#ifndef UNI_LIB_GPIO_H
#define UNI_LIB_GPIO_H

#include <stdbool.h>
#include <stdint.h>

/**
 * GPIO Pin configuration structure
 */
typedef struct {
  uint32_t pin;
  bool is_output;
  bool pull_up;
  bool pull_down;
  bool active_high;        // Added to specify active high/low logic
  void *platform_specific; // Platform-specific configuration
} gpio_config_t;

/**
 * GPIO Handle structure following OOP pattern
 */
typedef struct gpio_handle {
  void *hw_handle;  // Platform-specific hardware handle
  bool active_high; // Store the active logic configuration

  // Methods (function pointers for OOP)
  bool (*init)(struct gpio_handle *self, const gpio_config_t *config);
  bool (*deinit)(struct gpio_handle *self);

  bool (*write)(struct gpio_handle *self, bool state);
  bool (*read)(struct gpio_handle *self);

  bool (*activate)(struct gpio_handle *self);
  bool (*deactivate)(struct gpio_handle *self);
  bool (*is_active)(struct gpio_handle *self);

  bool (*toggle)(struct gpio_handle *self);

  // Optional interrupt support
  bool (*set_interrupt)(struct gpio_handle *self, void (*callback)(void *),
                        void *arg);
} gpio_handle_t;

/**
 * Platform-specific GPIO driver interface
 */
typedef struct {
  bool (*create)(gpio_handle_t *handle);
  bool (*destroy)(gpio_handle_t *handle);
} gpio_driver_t;

// Platform-specific driver implementation declarations
#ifdef STM32
extern const gpio_driver_t stm32_gpio_driver;
#else
extern const gpio_driver_t linux_gpio_driver;
#endif

#endif // UNI_LIB_GPIO_H
