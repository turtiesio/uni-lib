#include "components/button.h"
#include "mocks/gpio_mock.h"
#include "unity.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"

// Test fixtures
static button_handle_t button;
static QueueHandle_t event_queue;
static button_event_t received_event;

void setUp(void) {
    // Create event queue
    event_queue = xQueueCreate(10, sizeof(button_event_t));
    TEST_ASSERT_NOT_NULL(event_queue);

    // Create and configure button
    TEST_ASSERT_TRUE(button_driver.create(&button));
    
    button_config_t config = {
        .gpio_config = {
            .pin = 0,
            .is_output = false,
            .pull_up = true,
            .active_high = false,
        },
        .debounce_ms = 50,
        .long_press_ms = 1000,
        .pull_up = true,
        .event_queue = event_queue
    };

    TEST_ASSERT_TRUE(button.init(&button, &config));
    gpio_mock_reset();
}

void tearDown(void) {
    button.deinit(&button);
    button_driver.destroy(&button);
    vQueueDelete(event_queue);
}

// Test cases
void test_button_init(void) {
    TEST_ASSERT_FALSE(button.is_pressed);
}

void test_button_press_release(void) {
    // Simulate button press
    gpio_mock_set_pin_state(0, true);
    vTaskDelay(pdMS_TO_TICKS(60)); // Wait for debounce
    
    TEST_ASSERT_TRUE(xQueueReceive(event_queue, &received_event, pdMS_TO_TICKS(100)));
    TEST_ASSERT_EQUAL(BUTTON_EVENT_PRESSED, received_event);
    
    // Simulate button release
    gpio_mock_set_pin_state(0, false);
    vTaskDelay(pdMS_TO_TICKS(60)); // Wait for debounce
    
    TEST_ASSERT_TRUE(xQueueReceive(event_queue, &received_event, pdMS_TO_TICKS(100)));
    TEST_ASSERT_EQUAL(BUTTON_EVENT_CLICKED, received_event);
}

void test_button_long_press(void) {
    // Simulate button press
    gpio_mock_set_pin_state(0, true);
    vTaskDelay(pdMS_TO_TICKS(60)); // Wait for debounce
    
    TEST_ASSERT_TRUE(xQueueReceive(event_queue, &received_event, pdMS_TO_TICKS(100)));
    TEST_ASSERT_EQUAL(BUTTON_EVENT_PRESSED, received_event);
    
    // Wait for long press
    vTaskDelay(pdMS_TO_TICKS(1100));
    
    button_event_t event;
    TEST_ASSERT_TRUE(button.process(&button, &event));
    TEST_ASSERT_EQUAL(BUTTON_EVENT_HELD, event);
}

void test_button_debounce(void) {
    // Simulate button bounce
    for (int i = 0; i < 5; i++) {
        gpio_mock_set_pin_state(0, true);
        vTaskDelay(pdMS_TO_TICKS(5));
        gpio_mock_set_pin_state(0, false);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    
    // Should not receive any events due to debouncing
    TEST_ASSERT_FALSE(xQueueReceive(event_queue, &received_event, 0));
    
    // Simulate stable press
    gpio_mock_set_pin_state(0, true);
    vTaskDelay(pdMS_TO_TICKS(60)); // Wait for debounce
    
    TEST_ASSERT_TRUE(xQueueReceive(event_queue, &received_event, pdMS_TO_TICKS(100)));
    TEST_ASSERT_EQUAL(BUTTON_EVENT_PRESSED, received_event);
}

// Unity main
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_button_init);
    RUN_TEST(test_button_press_release);
    RUN_TEST(test_button_long_press);
    RUN_TEST(test_button_debounce);
    
    return UNITY_END();
}
