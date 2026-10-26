# ESP32-S2 Mini (ESP32-S2FNR2) – Getting Started

Self-test sketch for the ESP32-S2 Mini board (chip revision v1.0): prints chip info, scans Wi-Fi, scans I2C and blinks the onboard LED.

## Board

| Item | Value |
|---|---|
| SoC | ESP32-S2FNR2 (Xtensa LX7, single core, up to 240 MHz) |
| Flash / PSRAM | 4 MB embedded flash / 2 MB embedded PSRAM |
| Wireless | Wi-Fi 2.4 GHz (no Bluetooth) |
| USB | Native USB (USB-C) |

## Pinout

| Function | GPIO |
|---|---|
| Onboard LED | 15 |
| BOOT button | 0 |
| I2C (default) | SDA 8, SCL 9 |
| SPI (default) | MOSI 35, MISO 37, SCK 36, CS 34 |
| DAC | 17 (DAC_1), 18 (DAC_2) |
| ADC1 | 1–10 |
| Touch | 1–14 |

Notes:
- GPIO0 is a strapping pin (BOOT).
- GPIO26–32 are used by flash/PSRAM and are not broken out.
- Prefer ADC1 pins for analog reads when using Wi-Fi.

## Setup (Arduino IDE)

1. Install **esp32 by Espressif Systems** in Boards Manager.
2. Select board **LOLIN S2 Mini**.
3. Set **USB CDC On Boot: Enabled**.
4. Select the serial port and upload.

## Download mode

If the port is missing or the upload fails: hold **0**, tap **RST**, release **0**, then upload. Press **RST** afterwards to run the sketch.

## Expected output

- Model `ESP32-S2`, revision `0` (v1.0)
- Flash about 4096 KB, PSRAM about 2048 KB
- List of nearby 2.4 GHz networks
- Any connected I2C device addresses
- Onboard LED blinking on GPIO15
