#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dht.h"
#include "ssd1306.h"

static const char *TAG = "ENV_MONITOR";

#define DHT_GPIO       GPIO_NUM_4
#define OLED_SDA_GPIO  GPIO_NUM_21
#define OLED_SCL_GPIO  GPIO_NUM_22

#define OLED_WIDTH     128
#define OLED_HEIGHT    64

static SSD1306_t oled;

static void display_message(const char *line1, const char *line2)
{
    ssd1306_clear_screen(&oled, false);
    ssd1306_display_text(&oled, 0, line1, strlen(line1), false);
    ssd1306_display_text(&oled, 2, line2, strlen(line2), false);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing OLED");

    // Initialize I2C OLED. -1 means no separate reset pin is connected.
    i2c_master_init(&oled, OLED_SDA_GPIO, OLED_SCL_GPIO, -1);
    ssd1306_init(&oled, OLED_WIDTH, OLED_HEIGHT);
    ssd1306_clear_screen(&oled, false);
    ssd1306_contrast(&oled, 0xff);

    display_message("Env Monitor", "Starting...");
    vTaskDelay(pdMS_TO_TICKS(1500));

    while (1) {
        float humidity = 0.0f;
        float temperature = 0.0f;

        esp_err_t err = dht_read_float_data(
            DHT_TYPE_DHT11,
            DHT_GPIO,
            &humidity,
            &temperature
        );

        if (err == ESP_OK) {
            char temperature_line[24];
            char humidity_line[24];

            snprintf(temperature_line, sizeof(temperature_line),
                     "Temp: %.1f C", temperature);
            snprintf(humidity_line, sizeof(humidity_line),
                     "Humidity: %.1f%%", humidity);

            ESP_LOGI(TAG, "Temperature: %.1f C, humidity: %.1f%%",
                     temperature, humidity);

            display_message(temperature_line, humidity_line);
        } else {
            ESP_LOGE(TAG, "DHT11 read failed: %s", esp_err_to_name(err));
            display_message("Sensor read", "failed");
        }

        // DHT11 should not be read more frequently than about once per second.
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
