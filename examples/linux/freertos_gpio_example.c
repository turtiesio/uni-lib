#include "FreeRTOS.h"
#include "config/linux_config.h"
#include "hal/gpio.h"
#include "task.h"
#include <stdio.h>

// Task handles
static TaskHandle_t led_task_handle = NULL;
static gpio_handle_t gpio;

// LED control task
void vLedControlTask(void *pvParameters) {
  // Configure GPIO
  gpio_config_t config = {.pin = 18, // RPi GPIO18
                          .is_output = true,
                          .pull_up = false,
                          .pull_down = false,
                          .platform_specific = NULL};

  // Initialize GPIO
  if (!gpio.init(&gpio, &config)) {
    printf("Failed to initialize GPIO\n");
    vTaskDelete(NULL);
    return;
  }

  while (1) {
    gpio.write(&gpio, true);
    printf("LED ON\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    gpio.write(&gpio, false);
    printf("LED OFF\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main(void) {
  printf("FreeRTOS GPIO Example\n");

  // Create GPIO handle
  if (!linux_gpio_driver.create(&gpio)) {
    printf("Failed to create GPIO handle\n");
    return -1;
  }

  // Create LED control task
  BaseType_t result =
      xTaskCreate(vLedControlTask, "LED_Task", configMINIMAL_STACK_SIZE * 2,
                  NULL, tskIDLE_PRIORITY + 1, &led_task_handle);

  if (result != pdPASS) {
    printf("Failed to create LED task\n");
    return -1;
  }

  // Start the scheduler
  printf("Starting FreeRTOS scheduler\n");
  vTaskStartScheduler();

  // Should never reach here
  printf("Scheduler ended unexpectedly\n");
  return 0;
}
