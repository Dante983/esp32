#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GREEN_GPIO  GPIO_NUM_16
#define YELLOW_GPIO GPIO_NUM_17
#define RED_GPIO    GPIO_NUM_18

static void set_lights(bool green, bool yellow, bool red)
{
    ESP_ERROR_CHECK(gpio_set_level(GREEN_GPIO, green));
    ESP_ERROR_CHECK(gpio_set_level(YELLOW_GPIO, yellow));
    ESP_ERROR_CHECK(gpio_set_level(RED_GPIO, red));
}

void app_main(void)
{
    const uint64_t led_pin_mask =
        (1ULL << GREEN_GPIO) |
        (1ULL << YELLOW_GPIO) |
        (1ULL << RED_GPIO);

    gpio_config_t led_config = {
        .pin_bit_mask = led_pin_mask,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&led_config));

    while (1) {
        set_lights(true, false, false);   // Green
        vTaskDelay(pdMS_TO_TICKS(1000));

        set_lights(false, true, false);   // Yellow
        vTaskDelay(pdMS_TO_TICKS(500));

        set_lights(false, false, true);   // Red
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
