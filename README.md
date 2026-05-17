# HCMUT Embedded Systems Labs (Year 4 — Semester 251)

STM32F407 firmware and ESP8266 IoT sketches written for the Embedded Systems
lab course at Ho Chi Minh City University of Technology, organized chapter by
chapter (`C1`, `C2`, `C5`, `C7`, `C8`).

## Project scope

| Chapter | Topic | Project(s) |
|---|---|---|
| C1 | GPIO with software delay | `Lab1_GPIO` |
| C2 | Timer + 7-segment display | `Lab2_TrafficLight` (traffic light FSM), `Bai2_Timer_7Seg_template` (BKIT template) |
| C5 | I²C real-time clock | `Lab5_I2C_RTC` (DS3231 + LCD + FSM clock), `Lab5_template_ZGT` (skeleton) |
| C7 | Touch screen | `Lab7_TouchScreen` (Snake game on touch LCD) |
| C8 | IoT bridge | `Lab8_STM32F4` (STM32 side), `Lab8_ESP8266_MQTT` (ESP8266 → Adafruit IO) |

## Hardware

- BKIT ARM4 training kit
- STM32F407ZGTX (most labs) / STM32F407ZETX (BKIT templates)
- ESP8266 (ESP-12E) module for the C8 IoT bridge
- Touch LCD + DS3231 RTC + 7-segment display modules

## Toolchain

| Tool | Used for |
|---|---|
| STM32CubeIDE 1.13+ | STM32F4 builds (`Cx/Lab*`) |
| STM32CubeMX | `.ioc` peripheral configuration |
| Arduino IDE 1.8+ with ESP8266 core | `Lab8_ESP8266_MQTT` |
| Adafruit MQTT + ESP8266WiFi libraries | C8 cloud bridge |

## Build & flash — STM32 labs

1. Open STM32CubeIDE → **File → Open Projects From File System…**
2. Pick the lab folder (e.g. `C5_I2C_RTC/Lab5_I2C_RTC`).
3. Build (`Ctrl+B`) — `Debug/` will be regenerated locally and is gitignored.
4. Connect the BKIT ARM4 kit via ST-Link, then **Run** to flash.

## Build & flash — ESP8266 IoT (C8)

1. Open `C8_IoT_ESP8266/Lab8_ESP8266_MQTT/Wifi_ESP8266_ESP_12E.ino` in Arduino IDE.
2. Copy `secrets.example.h` → `secrets.h` and fill in your Wi-Fi SSID/password
   and your own Adafruit IO username/key. `secrets.h` is gitignored.
3. Select board **NodeMCU 1.0 (ESP-12E)** and flash.

## Repository layout

```
.
├── C1_GPIO_Delay/        Lab1_GPIO + docs/
├── C2_Timer_7Seg/        Lab2_TrafficLight + Bai2_Timer_7Seg_template
├── C5_I2C_RTC/           Lab5_I2C_RTC + Lab5_template_ZGT + docs/
├── C7_TouchScreen/       Lab7_TouchScreen
└── C8_IoT_ESP8266/       Lab8_STM32F4 + Lab8_ESP8266_MQTT + docs/
```

Each chapter's `docs/` folder holds the lecturer-provided PDF spec; those
files are kept local-only (see `.gitignore`).

## Team

| Name | Student ID | Role |
|---|---|---|
| _TBD_ | _TBD_ | _TBD_ |

## License

Coursework — internal use only.
