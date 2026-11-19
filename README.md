```markdown
# ESP32 Learning Projects

A collection of hands-on ESP32 projects built with **ESP-IDF and C**. Each project has its own directory with source code, wiring instructions, and a project-specific README.

## Progress

- [x] Project 2 — Button-controlled LED
- [x] Project 3 — Traffic light
- [x] Project 4 — Potentiometer-controlled LED dimmer
- [x] Project 5 — Temperature and humidity monitor with OLED
- [ ] Project 6 — Light-level monitor with OLED
- [ ] Project 7 — Ultrasonic distance meter
- [ ] Project 8 — Servo position control
- [ ] Project 9 — Joystick-controlled servo
- [ ] Project 10 — Buzzer alarm with adjustable threshold
- [ ] Project 11 — IR remote-controlled device
- [ ] Project 12 — Mini weather station with data logging

## Project list

| # | Project | What it does | Status |
|---|---|---|---|
| 2 | [Button Control](https://github.com/Dante983/esp32/tree/master/esp32-input) | Uses a push button to control an LED. | Done |
| 3 | [Traffic Light](https://github.com/Dante983/esp32/tree/master/traffic_light) | Cycles green, yellow, and red LEDs in sequence. | Done |
| 4 | [Potentiometer LED Dimmer](https://github.com/Dante983/esp32/tree/master/analog-input) | Reads a potentiometer with the ADC and controls LED brightness with PWM. | Done |
| 5 | [Environmental Monitor](https://github.com/Dante983/esp32/tree/master/enviromental-monitor) | Reads temperature and humidity and displays them on an I2C OLED. | Done |
| 6 | Light-Level Monitor | Reads a light sensor with the ADC and displays the value on the OLED. | Planned |
| 7 | Ultrasonic Distance Meter | Measures distance with an ultrasonic sensor and displays it on the OLED. | Planned |
| 8 | Servo Position Control | Sweeps a servo or sets its angle using PWM. | Planned |
| 9 | Joystick-Controlled Servo | Reads joystick axes with the ADC and uses them to control a servo. | Planned |
| 10 | Buzzer Alarm | Sounds a buzzer when a sensor reading passes an adjustable threshold. | Planned |
| 11 | IR Remote Control | Reads an IR remote and maps buttons to actions, such as controlling an LED. | Planned |
| 12 | Mini Weather Station | Shows temperature and humidity on the OLED and periodically logs readings. | Planned |

Update the links if a project directory has a different name. Replace planned project names and descriptions as you choose new projects, then change `[ ]` to `[x]` when they’re done.

## Hardware and development environment

- **Board:** ESP32-WROOM-32, 30-pin development board
- **Chip revision:** v3.1
- **Framework:** ESP-IDF, using C
- **Operating system:** Artix Linux with dinit
- **Serial port:** `/dev/ttyUSB0`
- **Flash baud rate:** 115200
- **Serial-port access:** User must belong to the `uucp` group

Check each project’s wiring instructions before connecting components. GPIO availability and labels can vary between boards.

## Repository layout

```text
esp32/
├── README.md
├── .gitignore
├── project-01-blink/
├── project-02-button-control/
├── project-03-traffic-light/
├── project-04-potentiometer-dimmer/
└── enviromental-monitor/
```

Each project should be independently buildable and include its own README:

```text
project-name/
├── CMakeLists.txt
├── README.md
└── main/
    ├── CMakeLists.txt
    ├── main.c
    └── idf_component.yml   # if the project uses managed components
```

## Build, flash, and monitor

Run these commands from the project directory:

```sh
idf.py set-target esp32
idf.py build
idf.py -p /dev/ttyUSB0 -b 115200 flash
idf.py -p /dev/ttyUSB0 monitor
```

Exit the serial monitor with `Ctrl+]`.

Neovim commands are also configured:

- `:ESP32Build` — build the current ESP-IDF project
- `:ESP32Flash` — flash it to the connected ESP32
- `:ESP32Monitor` — open the serial monitor
- `:ESP32Menuconfig` — open ESP-IDF configuration

## Wiring notes

- Connect circuit grounds to ESP32 GND.
- Use a current-limiting resistor with each discrete LED; these projects use 220–1000 Ω.
- Some breadboard power rails are split. Bridge a split rail with a jumper when needed.
- Don’t assume separate rails or breadboard rows are connected. Check the board layout or test continuity.
- Check component pin labels and polarity before powering the circuit.

## Git notes

All projects are stored in this repository. Generated ESP-IDF files and build outputs should not be committed. The `.gitignore` excludes items such as:

- `build/` directories
- `sdkconfig`
- `managed_components/`
- Generated CMake and compiled files

Keep project source code, README files, wiring diagrams, and component manifests in Git.

## Adding a project

1. Create a new project directory in this repository.
2. Add the ESP-IDF project files.
3. Write a project README with its goal, parts, wiring, build steps, and troubleshooting notes.
4. Test the build, flash, and hardware behavior.
5. Add the project to the progress checklist and table above.
6. Mark it done with `[x]` when complete.
```
