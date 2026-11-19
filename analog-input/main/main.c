#include <stdio.h>

#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define POTENTIOMETER_ADC_UNIT       ADC_UNIT_1
#define POTENTIOMETER_ADC_CHANNEL    ADC_CHANNEL_6  // GPIO34 on ESP32

#define LED_GPIO                     5

#define LEDC_TIMER                   LEDC_TIMER_0
#define LEDC_MODE                    LEDC_HIGH_SPEED_MODE
#define LEDC_CHANNEL                 LEDC_CHANNEL_0
#define LEDC_DUTY_RESOLUTION         LEDC_TIMER_10_BIT
#define LEDC_FREQUENCY_HZ            5000

#define ADC_MAX_READING               4095
#define PWM_MAX_DUTY                  1023

static const char *TAG = "pot_dimmer";

static void configure_adc(adc_oneshot_unit_handle_t *adc_handle)
{
    const adc_oneshot_unit_init_cfg_t unit_config = {
        .unit_id = POTENTIOMETER_ADC_UNIT,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_config, adc_handle));

    const adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(
        *adc_handle,
        POTENTIOMETER_ADC_CHANNEL,
        &channel_config
    ));
}

static void configure_led_pwm(void)
{
    const ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RESOLUTION,
        .freq_hz = LEDC_FREQUENCY_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    const ledc_channel_config_t channel_config = {
        .gpio_num = LED_GPIO,
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER,
        .duty = 0,
        .hpoint = 0,
    };

    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));
}

void app_main(void)
{
    adc_oneshot_unit_handle_t adc_handle;

    configure_adc(&adc_handle);
    configure_led_pwm();

    ESP_LOGI(TAG, "Potentiometer dimmer started");
    ESP_LOGI(TAG, "Potentiometer: GPIO34, LED PWM: GPIO%d", LED_GPIO);

    while (1) {
        int adc_reading = 0;

        ESP_ERROR_CHECK(adc_oneshot_read(
            adc_handle,
            POTENTIOMETER_ADC_CHANNEL,
            &adc_reading
        ));

        /*
         * Scale the ADC reading to the LEDC duty range.
         * 0 duty = LED off; maximum duty = LED brightest.
         */
        int duty = (adc_reading * PWM_MAX_DUTY) / ADC_MAX_READING;

        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));

        int brightness_percent = (duty * 100) / PWM_MAX_DUTY;

        ESP_LOGI(
            TAG,
            "ADC raw: %4d | PWM duty: %4d/%d | Brightness: %3d%%",
            adc_reading,
            duty,
            PWM_MAX_DUTY,
            brightness_percent
        );

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
