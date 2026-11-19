<img width="400" height="225" alt="analog-input" src="https://github.com/user-attachments/assets/2cede992-1bac-496e-bf23-a90f754ea09d" />
# Project 4: Potentiometer-Controlled LED Dimmer

Turn a potentiometer to control an LED’s brightness. The ESP32 reads the potentiometer’s wiper voltage using its ADC, then uses LEDC PWM to dim or brighten the LED.

## Learning goals

- Read an analog input using the ESP-IDF ADC oneshot driver.
- Generate PWM using the ESP-IDF LEDC driver.
- Map an ADC reading to a PWM duty cycle.
- Understand the difference between an analog input and a PWM output.

## Hardware

- ESP32-WROOM-32, 30-pin development board
- LAFVIN kit potentiometer
- LED
- 220–1000 Ω resistor
- Breadboard and jumper wires

This project uses GPIO34 for the potentiometer and GPIO5 for the LED. GPIO34 is ADC1 channel 6 on the original ESP32. <citation src="3"></citation>

## Wiring

| Component | Connection |
|---|---|
| Potentiometer outer leg 1 | ESP32 3V3 |
| Potentiometer middle leg (wiper) | ESP32 GPIO34 |
| Potentiometer outer leg 2 | ESP32 GND |
| LED anode (long leg) | ESP32 GPIO5 |
| LED cathode (short leg) | Resistor, then ESP32 GND |

The potentiometer’s two outer legs can be swapped. Swapping them reverses which direction you turn the knob to increase brightness. Do not connect the potentiometer to 5V: its wiper must stay within the ESP32’s 0–3.3 V input range.

Connect the LED’s cathode to GND through the resistor. Make sure the ESP32, potentiometer, and LED share a ground connection. If using a breadboard ground rail, check whether it is split and bridge the gap if necessary.

## Schematic

```text
                  Potentiometer
ESP32 3V3 ─────── outer leg
                       \
ESP32 GPIO34 ─────── wiper
                       /
ESP32 GND ───────── outer leg


ESP32 GPIO5 ───── LED anode (+)
                  LED cathode (-) ─── 220–1000 Ω resistor ─── ESP32 GND
```

## Project files

Create this directory structure inside the existing Git repository:

```text
project-04-potentiometer-dimmer/
├── CMakeLists.txt
├── README.md
└── main/
    ├── CMakeLists.txt
    └── main.c
```

### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(project-04-potentiometer-dimmer)
```

### `main/CMakeLists.txt`

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        esp_driver_ledc
        esp_adc
)

```

### `main/main.c`

```c
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
```

The ADC oneshot driver returns raw conversion values. This program scales the raw value into the configured 10-bit LEDC duty range; it does not calculate an exact voltage. <citation src="2"></citation>

## Build, flash, and monitor

Open the project directory in Neovim and use the existing commands:

- `:ESP32Build`
- `:ESP32Flash`
- `:ESP32Monitor`

Or use the ESP-IDF command line:

```sh
cd ~/Projects/esp32/project-04-potentiometer-dimmer
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 -b 115200 flash
idf.py -p /dev/ttyUSB0 monitor
```
<img width="400" height="225" alt="analog-input" src="https://github.com/user-attachments/assets/c1454959-0c6f-4fd5-99b3-83afc12fa421" />

Exit the serial monitor with `Ctrl+]`. If access to `/dev/ttyUSB0` is denied, check that your user belongs to the `uucp` group. After adding yourself to the group, log out and back in.

## Expected behavior

Turning the potentiometer should change the LED brightness. The monitor should print a changing ADC raw value, PWM duty, and estimated brightness percentage. Turning the knob all the way in either direction should bring the LED close to off or full brightness.

## Troubleshooting

- **LED does not light:** Check LED polarity, GPIO5, resistor placement, and the GND connection.
- **Brightness does not change:** Confirm the potentiometer’s middle leg is connected to GPIO34, with its outer legs connected to 3V3 and GND.
- **ADC reading stays near zero or maximum:** Check the three potentiometer connections and verify that the wiper is not accidentally connected directly to a power rail.
- **Brightness increases in the opposite direction:** Swap the potentiometer’s outer-leg connections.
- **Reading or brightness flickers:** Check the jumper and breadboard connections, especially GND. A later improvement could average several ADC samples.
- **Build reports an ADC attenuation enum error:** This code uses `ADC_ATTEN_DB_11`, commonly used in ESP-IDF 5.x. If your installed ESP-IDF version expects a different attenuation enum, use the enum named in that version’s ADC headers.

## Git

From the repository root, add and commit the project:

```sh
cd ~/Projects/esp32
git add project-04-potentiometer-dimmer
git commit -m "Add ESP-IDF potentiometer LED dimmer project"
```

The repository’s existing `.gitignore` should keep generated `build/` files and `sdkconfig` out of the commit.
