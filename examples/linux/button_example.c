#include "components/button.h"
#include <stdio.h>
#include <unistd.h>

int main(void) {
    // Create button instance
    button_handle_t button;
    button_driver.create(&button);

    // Configure button
    button_config_t config = {
        .gpio_config = {
            .pin = 18,                // GPIO18 (adjust as needed)
            .is_output = false,       // Input mode
            .pull_up = true,          // Use pull-up
            .pull_down = false,
            .active_high = false,     // Active low since we're using pull-up
        },
        .debounce_ms = 50,           // 50ms debounce
        .long_press_ms = 1000,       // 1 second for long press
        .pull_up = true              // Using pull-up resistor
    };

    // Initialize button
    if (!button.init(&button, &config)) {
        printf("Failed to initialize button\n");
        return -1;
    }

    printf("Button example started. Press Ctrl+C to exit.\n");
    printf("Short press for click, long press (>1s) for hold.\n");

    // Main loop
    while (1) {
        button_event_t event;
        if (button.process(&button, &event)) {
            switch (event) {
                case BUTTON_EVENT_PRESSED:
                    printf("Button pressed\n");
                    break;
                case BUTTON_EVENT_RELEASED:
                    printf("Button released\n");
                    break;
                case BUTTON_EVENT_CLICKED:
                    printf("Button clicked\n");
                    break;
                case BUTTON_EVENT_HELD:
                    printf("Button held\n");
                    break;
            }
        }
        usleep(10000); // 10ms sleep to prevent busy waiting
    }

    // Cleanup
    button.deinit(&button);
    button_driver.destroy(&button);

    return 0;
}
