#ifndef UNI_LIB_LINUX_CONFIG_H
#define UNI_LIB_LINUX_CONFIG_H

// GPIO Configuration
#define GPIO_MAX_PINS 64
#define GPIO_INTERRUPT_SUPPORT 1

// I2C Configuration
#define I2C_MAX_DEVICES 2
#define I2C_BUFFER_SIZE 256

// SPI Configuration
#define SPI_MAX_DEVICES 2
#define SPI_BUFFER_SIZE 1024

// Memory Configuration
#define STATIC_ALLOCATION 1
#define DYNAMIC_ALLOCATION 0

// Debug Configuration
#define DEBUG_ENABLE 1
#define DEBUG_UART "/dev/ttyUSB0"

#endif // UNI_LIB_LINUX_CONFIG_H
