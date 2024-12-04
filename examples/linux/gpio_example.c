#include "config/linux_config.h"
#include "hal/gpio.h"
#include <stdio.h>
#include <unistd.h>

int main() {
  // Create GPIO handle
  gpio_handle_t gpio;
  if (!linux_gpio_driver.create(&gpio)) {
    printf("Failed to create GPIO handle\n");
    return -1;
  }

  // Configure GPIO
  gpio_config_t config = {.pin = 18, // RPi GPIO18
                          .is_output = true,
                          .pull_up = false,
                          .pull_down = false,
                          .platform_specific = NULL};

  // Initialize GPIO
  if (!gpio.init(&gpio, &config)) {
    printf("Failed to initialize GPIO\n");
    return -1;
  }

  // Toggle LED 5 times
  for (int i = 0; i < 5; i++) {
    gpio.write(&gpio, true);
    printf("LED ON\n");
    sleep(1);

    gpio.write(&gpio, false);
    printf("LED OFF\n");
    sleep(1);
  }

  // Cleanup
  gpio.deinit(&gpio);
  linux_gpio_driver.destroy(&gpio);

  return 0;
}
