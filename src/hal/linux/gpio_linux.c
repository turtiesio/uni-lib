#include "hal/gpio.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GPIO_PATH "/sys/class/gpio"

typedef struct {
  int fd;
  int pin_number;
  void (*interrupt_callback)(void *);
  void *callback_arg;
} linux_gpio_data_t;

static bool linux_gpio_init(gpio_handle_t *self, const gpio_config_t *config) {
  if (!self || !self->hw_handle || !config)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  hw->pin_number = config->pin;
  self->active_high = config->active_high; // Store active logic configuration

  // Export GPIO
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/export",
           hw->pin_number);
  system(cmd);

  // Set direction
  snprintf(cmd, sizeof(cmd), "echo %s > /sys/class/gpio/gpio%d/direction",
           config->is_output ? "out" : "in", hw->pin_number);
  system(cmd);

  return true;
}

static bool linux_gpio_deinit(gpio_handle_t *self) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/unexport",
           hw->pin_number);
  system(cmd);

  return true;
}

static bool linux_gpio_activate(gpio_handle_t *self) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value",
           self->active_high ? 1 : 0, hw->pin_number);
  system(cmd);

  return true;
}

static bool linux_gpio_deactivate(gpio_handle_t *self) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value",
           self->active_high ? 0 : 1, hw->pin_number);
  system(cmd);

  return true;
}

static bool linux_gpio_is_active(gpio_handle_t *self) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char path[64], value;
  snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", hw->pin_number);

  FILE *fp = fopen(path, "r");
  if (!fp)
    return false;

  value = fgetc(fp);
  fclose(fp);

  bool pin_high = (value == '1');
  return self->active_high ? pin_high : !pin_high;
}

static bool linux_gpio_write(gpio_handle_t *self, bool state) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/gpio%d/value",
           state ? 1 : 0, hw->pin_number);
  system(cmd);

  return true;
}

static bool linux_gpio_read(gpio_handle_t *self) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char path[64];
  char value;

  snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", hw->pin_number);
  int fd = open(path, O_RDONLY);
  if (fd < 0)
    return false;

  read(fd, &value, 1);
  close(fd);

  return value == '1';
}

static bool linux_gpio_toggle(gpio_handle_t *self) {
  if (!self || !self->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)self->hw_handle;
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo toggle > /sys/class/gpio/gpio%d/edge",
           hw->pin_number);
  system(cmd);

  return true;
}

static bool linux_gpio_create(gpio_handle_t *handle) {
  if (!handle)
    return false;

  linux_gpio_data_t *hw = malloc(sizeof(linux_gpio_data_t));
  if (!hw)
    return false;

  handle->hw_handle = hw;
  handle->init = linux_gpio_init;
  handle->deinit = linux_gpio_deinit;
  handle->activate = linux_gpio_activate;
  handle->deactivate = linux_gpio_deactivate;
  handle->is_active = linux_gpio_is_active;
  handle->toggle = linux_gpio_toggle;
  handle->write = linux_gpio_write;
  handle->read = linux_gpio_read;

  return true;
}

static bool linux_gpio_destroy(gpio_handle_t *handle) {
  if (!handle || !handle->hw_handle)
    return false;

  linux_gpio_data_t *hw = (linux_gpio_data_t *)handle->hw_handle;

  // Unexport GPIO
  char cmd[64];
  snprintf(cmd, sizeof(cmd), "echo %d > /sys/class/gpio/unexport",
           hw->pin_number);
  system(cmd);

  free(hw);
  handle->hw_handle = NULL;

  return true;
}

const gpio_driver_t linux_gpio_driver = {.create = linux_gpio_create,
                                         .destroy = linux_gpio_destroy};
