# Button-Controlled LED — ESP-IDF

A beginner ESP32 project that reads a push button on **GPIO4** and turns an LED connected to **GPIO5** on or off. It uses ESP-IDF and C.

## 1. Components

Use these parts from the LAFVIN ESP32 starter kit:

- ESP32 development board
- Breadboard
- Push button
- LED
- 10 kΩ resistor for the button input
- 220 Ω resistor for the LED
- Jumper wires

## 2. Wiring

Turn off or unplug the ESP32 while wiring.

| From | To |
|---|---|
| ESP32 **3V3** | One side of the push button |
| Other side of the push button | **GPIO4** |
| **GPIO4** | One end of the 10 kΩ resistor |
| Other end of the 10 kΩ resistor | **GND** |
| **GPIO5** | One end of the 220 Ω resistor |
| Other end of the 220 Ω resistor | LED anode (long leg) |
| LED cathode (short leg) | **GND** |
| ESP32 **GND** | Breadboard ground rail |

Place the push button across the breadboard’s center gap. The button should connect **3V3 to GPIO4 when pressed**. The 10 kΩ resistor connects GPIO4 to GND, holding the input low when the button is released.

The LED and 220 Ω resistor must be in series. The resistor can be on either side of the LED, provided the path is **GPIO5 → resistor → LED → GND**. Do not connect the button to 5 V.

### Wiring diagram

```text
3V3 ── push button ──┬── GPIO4
                     │
                   10 kΩ
                     │
GND ─────────────────┘


GPIO5 ── 220 Ω ── LED anode (+)
                       LED cathode (−) ── GND
```

## 3. Create the ESP-IDF project

Open a terminal where the ESP-IDF environment is enabled, then run:

```sh
cd ~/Projects/esp32
idf.py create-project button
cd button
```

The project should contain files including `CMakeLists.txt` and a `main` directory. The application source file is `main/main.c`.

If you already created the `button` project, just change into its directory:

```sh
cd ~/Projects/esp32/button
```

## 4. Add the application code

Replace the contents of `main/main.c` with:

```c
#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUTTON_GPIO GPIO_NUM_4
#define LED_GPIO    GPIO_NUM_5

void app_main(void)
{
    gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&button_config));

    gpio_config_t led_config = {
        .pin_bit_mask = 1ULL << LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&led_config));

    while (1) {
        int button_pressed = gpio_get_level(BUTTON_GPIO);
        ESP_ERROR_CHECK(gpio_set_level(LED_GPIO, button_pressed));

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
```

The input uses `GPIO_PULLDOWN_ENABLE`, so GPIO4 reads low when the button is released and high when pressed. The external 10 kΩ pull-down resistor shown in the schematic provides the same default-low behavior; enabling the ESP32’s internal pull-down as well is also fine.

The loop reads the button and writes its value to the LED. The short delay prevents the task from checking the input continuously. This simple version does not implement full button debouncing.

## 5. Configure the project

From the project directory, open the ESP-IDF configuration menu:

```sh
idf.py menuconfig
```

For this project, the default configuration is usually sufficient. Save and exit the menu.

## 6. Build

Build the project:

```sh
idf.py build
```

If the build succeeds, the firmware is ready to flash.

## 7. Flash the ESP32

Connect the ESP32 and flash it using the configured serial port and known-working baud rate:

```sh
idf.py -p /dev/ttyUSB0 -b 115200 flash
```

Your account needs access to `/dev/ttyUSB0`; on this system, that requires membership in the `uucp` group. After adding yourself to the group, log out and back in for the change to take effect.

## 8. Run and test

Start the serial monitor:

```sh
idf.py -p /dev/ttyUSB0 -b 115200 monitor
```

Press the button:

- **Released:** LED is off.
- **Pressed:** LED turns on.

Exit the monitor with **Ctrl+]**.

You can also build, flash, or monitor from the Neovim commands configured for this workspace. Launch Neovim from an ESP-IDF-enabled terminal, then run `:ESP32Build`, `:ESP32Flash`, or `:ESP32Monitor`.

## Troubleshooting

- **LED does not light:** Check the LED orientation, the 220 Ω resistor, the GPIO5 wire, and the GND connection.
- **LED remains on or behaves inconsistently:** Check that the button bridges the breadboard’s center gap and connects 3V3 to GPIO4 when pressed. Confirm that the 10 kΩ resistor connects GPIO4 to GND.
- **Button input does not change:** Confirm the wire goes to **GPIO4**, not GPIO5, and that the button is connected to 3V3—not 5 V.
- **Permission denied for `/dev/ttyUSB0`:** Check `uucp` group membership, then log out and back in.
- **Flash fails:** Confirm the board appears at `/dev/ttyUSB0`, close any other program using the serial port, and try again at 115200 baud.
