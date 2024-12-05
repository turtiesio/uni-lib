# Universal Embedded Library

A portable embedded library supporting both STM32 and Linux/Raspberry Pi platforms with FreeRTOS simulation capabilities.

## Features

- Platform-agnostic hardware abstraction layer
- Support for STM32 HAL and Linux/Raspberry Pi
- FreeRTOS simulation support for rapid development
- Modular peripheral drivers (GPIO, I2C, SPI)
- Static allocation by default, optional dynamic allocation
- Object-oriented C design following SOLID principles

## Project Structure

```
uni-lib/
├── CMakeLists.txt                 # Main CMake configuration
├── src/
│   ├── core/                      # Core interfaces and utilities
│   └── hal/                       # Hardware Abstraction Layer
│       ├── interface/            # HAL interfaces
│       ├── stm32/               # STM32 implementation
│       └── linux/               # Linux/RPi implementation
├── include/                       # Public headers
│   ├── config/                   # Platform-specific configs
│   └── hal/                     # HAL interfaces
├── examples/                      # Example applications
│   ├── stm32/
│   └── linux/
└── tests/                        # Unit tests
```

## Design Philosophy

### SOLID Principles in C

1. **Single Responsibility**
   - Each hardware driver handles one specific peripheral
   - Clear separation between HAL and platform-specific code

2. **Open/Closed**
   - New platform support can be added without modifying existing code
   - Hardware interfaces are defined through function pointers

3. **Liskov Substitution**
   - All platform implementations follow the same interface
   - Consistent behavior across different platforms

4. **Interface Segregation**
   - Minimal interfaces for each hardware feature
   - No dependency between different peripherals

5. **Dependency Inversion**
   - High-level code depends on HAL interfaces
   - Platform-specific code implements these interfaces

### Memory Management

- Static allocation by default
- Optional dynamic allocation for flexibility
- Clear memory ownership and lifecycle management

## Building

### Prerequisites

- CMake (>= 3.15)
- GCC for Linux development
- ARM GCC for STM32 development (optional)

### For Linux/FreeRTOS Simulation

```bash
mkdir build && cd build
cmake ..
make
```

### For STM32 (Coming Soon)

```bash
mkdir build_stm32 && cd build_stm32
cmake -DTARGET_PLATFORM=STM32 ..
make
```

## Examples

### GPIO Example

```c
#include "config/linux_config.h"
#include "hal/gpio.h"

// Create and configure GPIO
gpio_handle_t gpio;
gpio_config_t config = {
    .pin = 18,          // RPi GPIO18
    .is_output = true,
    .pull_up = false,
    .pull_down = false
};

// Initialize and use
linux_gpio_driver.create(&gpio);
gpio.init(&gpio, &config);
gpio.write(&gpio, true);  // Set high
```

## Testing

Run the test suite:
```bash
make test
```

## Development

### Adding New Platforms

1. Create platform-specific implementation in `src/hal/<platform>/`
2. Implement the HAL interfaces defined in `include/hal/`
3. Add platform-specific configuration in `include/config/`
4. Update CMake configuration for the new platform

### Adding New Peripherals

1. Define interface in `include/hal/`
2. Implement for each supported platform
3. Add examples and tests
4. Update documentation

## License

MIT License
