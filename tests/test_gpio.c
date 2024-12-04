#include "hal/gpio.h"
#include <assert.h>
#include <stdio.h>

void test_gpio_create() {
  gpio_handle_t gpio;
  assert(linux_gpio_driver.create(&gpio) == true);
  assert(gpio.hw_handle != NULL);
  assert(gpio.init != NULL);
  assert(gpio.write != NULL);
  assert(gpio.read != NULL);

  linux_gpio_driver.destroy(&gpio);
  printf("GPIO creation test passed\n");
}

void test_gpio_init() {
  gpio_handle_t gpio;
  linux_gpio_driver.create(&gpio);

  gpio_config_t config = {.pin = 18,
                          .is_output = true,
                          .pull_up = false,
                          .pull_down = false,
                          .platform_specific = NULL};

  assert(gpio.init(&gpio, &config) == true);
  linux_gpio_driver.destroy(&gpio);
  printf("GPIO initialization test passed\n");
}

int main() {
  printf("Running GPIO tests...\n");

  test_gpio_create();
  test_gpio_init();

  printf("All GPIO tests passed!\n");
  return 0;
}
