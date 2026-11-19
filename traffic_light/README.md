# Traffic Light — ESP-IDF

A three-LED traffic light project for the original ESP32, written in C with ESP-IDF. The LEDs run in a repeating sequence: green for 5 seconds, yellow for 2 seconds, and red for 5 seconds.

## What you’ll learn

- Configure multiple ESP32 GPIO pins as outputs.
- Control several LEDs independently.
- Use FreeRTOS delays to set timing.
- Build and flash an ESP-IDF project.

## Components

- ESP32 development board
- Breadboard
- One red LED
- One yellow LED
- One green LED
- Three 220–1,000 Ω resistors
- Jumper wires

Use a separate resistor for each LED.

## GPIO assignments

| LED | ESP32 GPIO |
|---|---:|
| Green | GPIO16 |
| Yellow | GPIO17 |
| Red | GPIO18 |

## Wiring diagram

Connect each GPIO to its LED **through a resistor**. Connect each LED’s other side to GND.

```text
ESP32 GPIO16 ──[220–1,000 Ω]──|>|── GND
                               Green

ESP32 GPIO17 ──[220–1,000 Ω]──|>|── GND
                               Yellow

ESP32 GPIO18 ──[220–1,000 Ω]──|>|── GND

ESP32 GND ──────────────────────── Breadboard GND rail
```

`|>|` represents the LED. Its anode (long leg) faces the resistor and GPIO; its cathode (short leg) connects to GND.

### Breadboard wiring

1. Connect an ESP32 **GND** pin to the breadboard’s ground rail.
2. Connect **GPIO16** through a resistor to the green LED’s anode.
3. Connect the green LED’s cathode to the ground rail.
4. Repeat for the yellow LED on **GPIO17** and the red LED on **GPIO18**.
5. Check that no LED is connected directly to a GPIO without a series resistor.

```text
GPIO16 ── resistor ── green LED anode
                       green LED cathode ── GND

GPIO17 ── resistor ── yellow LED anode
                       yellow LED cathode ── GND

GPIO18 ── resistor ── red LED anode
                       red LED cathode ── GND
```

## Project files

The project directory should have this structure:

```text
traffic_light/
├── CMakeLists.txt
└── main/
    ├── CMakeLists.txt
    └── main.c
```

## 1. Create or check the project files

Open an ESP-IDF-enabled terminal and go to the project directory:

```sh
cd ~/Projects/esp32/traffic_light
```

If the directory is empty, create the `main` subdirectory:

```sh
mkdir -p main
```

Create or replace the project’s top-level `CMakeLists.txt` with:

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(traffic_light)
```

Create or replace `main/CMakeLists.txt` with:

```cmake
idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS ".")
```

## 2. Add the application code

Create or replace `main/main.c` with:

```c
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
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&led_config));

    while (1) {
        set_lights(true, false, false);   // Green
        vTaskDelay(pdMS_TO_TICKS(5000));

        set_lights(false, true, false);   // Yellow
        vTaskDelay(pdMS_TO_TICKS(2000));

        set_lights(false, false, true);   // Red
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

## 3. Build the project

From the project directory, run:

```sh
idf.py build
```

If the build completes successfully, the firmware is ready to flash.

## 4. Flash the ESP32

Connect the ESP32 and run:

```sh
idf.py -p /dev/ttyUSB0 -b 115200 flash
```

Your account must have permission to access `/dev/ttyUSB0`. On this system, access requires membership in the `uucp` group. Log out and back in after changing group membership.

## 5. Run and test

Start the serial monitor:

```sh
idf.py -p /dev/ttyUSB0 -b 115200 monitor
```

Watch the LEDs cycle:

1. Green turns on for 5 seconds.
2. Yellow turns on for 2 seconds.
3. Red turns on for 5 seconds.
4. The cycle repeats.

<img width="400" height="225" alt="PXL_20261119_115928054" src="https://github.com/user-attachments/assets/04a832ce-eb5c-44e4-aea7-aab6aa7135db" />

Exit the monitor with **Ctrl+]**.

## Timing changes

The durations are set by the arguments to `pdMS_TO_TICKS()`:

```c
vTaskDelay(pdMS_TO_TICKS(5000)); // 5 seconds
vTaskDelay(pdMS_TO_TICKS(2000)); // 2 seconds
```

Change `5000` or `2000` to another duration in milliseconds, then build and flash again.

## Troubleshooting

- **An LED does not light:** Check its direction, resistor, GPIO connection, and GND connection. The LED’s long leg is usually the anode.
- **LEDs appear in the wrong sequence:** Check that green is wired to GPIO16, yellow to GPIO17, and red to GPIO18.
- **More than one LED stays on:** Check for wiring shorts and verify the `set_lights()` calls set only the intended LED to `true`.
- **Permission denied for `/dev/ttyUSB0`:** Check that your user is in the `uucp` group and log out and back in.
- **Flash fails:** Confirm the board is connected at `/dev/ttyUSB0`, close any other program using the serial port, and retry at 115200 baud.

## Safety notes

- Use one current-limiting resistor per LED.
- Connect the LED circuits to ESP32 **GND**, not to 5 V.
- Do not connect an LED directly between a GPIO pin and GND without a resistor.
