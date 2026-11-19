# Environmental Monitor — ESP-IDF

This project reads temperature and relative humidity from a DHT sensor and displays the readings on a 0.96-inch I2C OLED. It uses ESP-IDF in C, the `esp-idf-lib/dht` component, and the `nopnop2002/ssd1306` driver.

> **Sensor assumption:** The example is set up for a DHT11. If your module is a DHT22, change `DHT_TYPE_DHT11` in `main.c` to `DHT_TYPE_AM2301`. The DHT driver uses `DHT_TYPE_AM2301` for DHT21/DHT22/AM2301-family sensors. <citation src="2"></citation>

## Features

- Reads temperature in degrees Celsius and relative humidity in percent.
- Displays both readings on a 128×64 OLED.
- Logs successful readings and sensor errors to the serial monitor.
- Waits two seconds between sensor reads.

## Hardware

- ESP32-WROOM-32, 30-pin development board
- 0.96-inch OLED, I2C, 128×64, SSD1306-compatible
- DHT11 or DHT22 temperature/humidity module
- Breadboard and jumper wires

## Wiring

| Component pin | ESP32 connection |
|---|---|
| OLED VCC | 3V3 |
| OLED GND | GND |
| OLED SCL | GPIO22 |
| OLED SDA | GPIO21 |
| DHT VCC | 3V3 |
| DHT GND | GND |
| DHT DATA / OUT | GPIO4 |

```text
ESP32 3V3 ───── OLED VCC
             └─ DHT VCC

ESP32 GND ───── OLED GND
             └─ DHT GND

ESP32 GPIO22 ── OLED SCL
ESP32 GPIO21 ── OLED SDA
ESP32 GPIO4  ── DHT DATA
```

Connect all grounds together. If your breadboard’s ground rail is split, bridge the split with a jumper.

The DHT data line needs a pull-up to 3V3. Many three-pin sensor modules have one built in. For a bare sensor or a module without a pull-up, add a 4.7–10 kΩ resistor between DATA and 3V3. Keep the data wire short. <citation src="2"></citation>

Check the OLED’s pin labels before connecting it; module pin order varies. This project’s `main.c` is configured for a 128×64 display. Some displays sold as SSD1306-compatible may use an SH1106 controller instead.

## Project files

```text
enviromental-monitor/
├── CMakeLists.txt
├── README.md
└── main/
    ├── CMakeLists.txt
    ├── idf_component.yml
    └── main.c
```

### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(enviromental_monitor)
```

### `main/CMakeLists.txt`

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES dht ssd1306
)
```

### `main/idf_component.yml`

The SSD1306 component is fetched from the `nopnop2002/esp-idf-ssd1306` Git repository, using its `components/ssd1306/` subdirectory. The DHT driver is obtained from the component registry. The Component Manager resolves manifest dependencies during project configuration and downloads them into `managed_components/`. <citation src="6,13"></citation>

```yaml
dependencies:
  esp-idf-lib/dht: "^1.3.0"
  nopnop2002/ssd1306:
    git: https://github.com/nopnop2002/esp-idf-ssd1306.git
    path: components/ssd1306
```

### `main/main.c`

```c
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

    // The final argument is the OLED reset pin.
    // Use -1 when no separate reset pin is connected.
    i2c_master_init(&oled, OLED_SDA_GPIO, OLED_SCL_GPIO, -1);

    ESP_ERROR_CHECK(ssd1306_init(&oled, OLED_WIDTH, OLED_HEIGHT));

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

            snprintf(
                temperature_line,
                sizeof(temperature_line),
                "Temp: %.1f C",
                temperature
            );

            snprintf(
                humidity_line,
                sizeof(humidity_line),
                "Humidity: %.1f%%",
                humidity
            );

            ESP_LOGI(
                TAG,
                "Temperature: %.1f C, humidity: %.1f%%",
                temperature,
                humidity
            );

            display_message(temperature_line, humidity_line);
        } else {
            ESP_LOGE(TAG, "DHT read failed: %s", esp_err_to_name(err));
            display_message("Sensor read", "failed");
        }

        // DHT sensors should not be polled more than once per second.
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
```

The DHT component’s floating-point API takes the sensor type, GPIO, humidity output, and temperature output in that order. It reports errors such as timeout or checksum failure through the returned `esp_err_t`. <citation src="2"></citation>

## Configure and build

Run these commands from the project directory:

```sh
idf.py set-target esp32
idf.py reconfigure
idf.py build
```

The first configuration/build may download the components declared in `main/idf_component.yml`.

## Flash and monitor

The board is connected at `/dev/ttyUSB0`; use 115200 baud:

```sh
idf.py -p /dev/ttyUSB0 -b 115200 flash
idf.py -p /dev/ttyUSB0 monitor
```
<img width="3000" height="4000" alt="PXL_20261119_221054668" src="https://github.com/user-attachments/assets/a0d252c3-e3b8-4260-8840-d05ec23dfff6" />
<img width="4000" height="3000" alt="PXL_20261119_221040218" src="https://github.com/user-attachments/assets/e4210f39-e42b-4dc4-a2da-5aeefeb7eeaa" />

Exit the serial monitor with `Ctrl+]`. If access to the serial port is denied, check that your user belongs to the `uucp` group and start a new login session after changing group membership.

The project’s Neovim commands can also be used:

- `:ESP32Build`
- `:ESP32Flash`
- `:ESP32Monitor`
- `:ESP32Menuconfig`

## Expected behavior

The OLED first displays a startup message. It then shows temperature and humidity, refreshing about every two seconds. The serial monitor logs the same values. DHT11 readings have relatively coarse resolution; that is normal for the sensor. <citation src="2"></citation>

## Troubleshooting

**The display stays blank**

- Check OLED power, GND, SDA, and SCL connections.
- Verify that the module is an I2C display and uses an SSD1306-compatible controller.
- Confirm that it is a 128×64 display; this code initializes that size.
- Check whether the OLED’s I2C address or controller differs from what the driver expects.

**The display works, but DHT reads fail**

- Verify the sensor’s pin labels; module pin order can vary.
- Confirm the `DATA` wire is on GPIO4 and the sensor shares ESP32 GND.
- Check for a DATA-to-3V3 pull-up if the module does not include one.
- Keep the data wire short and try a fresh sensor read after correcting the wiring.

**The build reports `dht.h` or `ssd1306.h` missing**

- Check that `main/idf_component.yml` contains both dependencies.
- Run `idf.py reconfigure`, then `idf.py build`.
- Confirm that the component names in `main/CMakeLists.txt` are `dht` and `ssd1306`.

**The DHT type is wrong**

For a DHT22, replace this line:

```c
DHT_TYPE_DHT11,
```

with:

```c
DHT_TYPE_AM2301,
```

## Git note

Keep source files, `README.md`, and `main/idf_component.yml` in Git. The repository’s `.gitignore` should continue ignoring generated `build/`, `managed_components/`, and `sdkconfig` files. The Component Manager also creates a `dependencies.lock` file; keep that lockfile under version control if you want builds to resolve the same dependency versions consistently.
