# Configuration
BUILD_DIR = build
BUILD_DIR_STM32 = build_stm32
EXAMPLES_DIR = examples
TEST_DIR = tests

# Tools
CMAKE = cmake
MAKE = make
CLANG_FORMAT = clang-format
CPPCHECK = cppcheck
GDB = gdb

# Default target platform (linux or stm32)
TARGET ?= linux

.PHONY: all clean build test format lint debug run-example help

# Default target
all: help

help:
	@echo "Universal Embedded Library Build System"
	@echo ""
	@echo "Usage:"
	@echo "  make build [TARGET=linux|stm32] - Build the project"
	@echo "  make clean - Clean build directories"
	@echo "  make test - Run tests"
	@echo "  make format - Format source code"
	@echo "  make lint - Run static analysis"
	@echo "  make debug EXAMPLE=name - Debug an example"
	@echo "  make run-example EXAMPLE=name - Run an example"
	@echo "  make setup-linux - Setup Linux development environment"
	@echo "  make setup-stm32 - Setup STM32 development environment"
	@echo ""
	@echo "Examples:"
	@echo "  make build TARGET=linux - Build for Linux"
	@echo "  make run-example EXAMPLE=gpio_example - Run GPIO example"
	@echo "  make debug EXAMPLE=gpio_example - Debug GPIO example"

# Build targets
build:
ifeq ($(TARGET),stm32)
	@mkdir -p $(BUILD_DIR_STM32)
	@cd $(BUILD_DIR_STM32) && $(CMAKE) -DCMAKE_TOOLCHAIN_FILE=../cmake/stm32_gcc.cmake ..
	@cd $(BUILD_DIR_STM32) && $(MAKE)
else
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) ..
	@cd $(BUILD_DIR) && $(MAKE)
endif

clean:
	@rm -rf $(BUILD_DIR) $(BUILD_DIR_STM32)
	@echo "Cleaned build directories"

# Development tools
format:
	@find src include $(EXAMPLES_DIR) $(TEST_DIR) -iname *.h -o -iname *.c | xargs $(CLANG_FORMAT) -i
	@echo "Formatted source files"

lint:
	$(CPPCHECK) --enable=all --suppress=missingInclude src include $(EXAMPLES_DIR)
	@echo "Completed static analysis"

# Testing
test: build
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Debugging and running
debug:
ifndef EXAMPLE
	$(error EXAMPLE is not set. Usage: make debug EXAMPLE=example_name)
endif
	$(GDB) $(BUILD_DIR)/$(EXAMPLES_DIR)/linux/$(EXAMPLE)

run-example:
ifndef EXAMPLE
	$(error EXAMPLE is not set. Usage: make run-example EXAMPLE=example_name)
endif
	@./$(BUILD_DIR)/$(EXAMPLES_DIR)/linux/$(EXAMPLE)

# Environment setup
setup-linux:
	@echo "Installing Linux development dependencies..."
	@which $(CMAKE) >/dev/null || sudo apt-get install -y cmake
	@which $(CLANG_FORMAT) >/dev/null || sudo apt-get install -y clang-format
	@which $(CPPCHECK) >/dev/null || sudo apt-get install -y cppcheck
	@which $(GDB) >/dev/null || sudo apt-get install -y gdb
	@echo "Linux development environment setup complete"

setup-stm32:
	@echo "Setting up STM32 development environment..."
	@which arm-none-eabi-gcc >/dev/null || sudo apt-get install -y gcc-arm-none-eabi
	@echo "STM32 development environment setup complete"

tree:
	@tree -I 'thirdparty|build|.git'

.PRECIOUS: $(BUILD_DIR)/% $(BUILD_DIR_STM32)/%
