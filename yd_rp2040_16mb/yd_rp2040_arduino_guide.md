# Getting Started with the VCC-GND YD-RP2040 (16MB) in Arduino IDE

A complete setup and troubleshooting guide for the **VCC-GND YD-RP2040** (and compatible purple 16MB / 128Mbit RP2040 clone development boards) using the Arduino IDE.

---

## Board Specifications & Pin Details

* **Microcontroller:** Raspberry Pi RP2040 Dual-Core ARM Cortex-M0+ (up to 133 MHz)
* **Flash Memory:** 16MB (128Mbit) QSPI Flash (Winbond W25Q128JV or compatible)
* **Connector:** USB Type-C
* **Onboard Buttons:**
  * `BOOT` (BOOTSEL)
  * `RESET` (or `RUN`)
* **Onboard Peripherals:**
  * User LED (Blue): Typically wired to **GPIO 25**
  * Addressable RGB LED (WS2812 / NeoPixel): Wired to **GPIO 23**
  * Power LED (Red)

---

## Step 1: Install RP2040 Board Core in Arduino IDE

The official Arduino Mbed core does not natively support third-party 16MB flash configurations. Use the **Earle Philhower** community core.

1. Open **Arduino IDE** (v2.x recommended).
2. Go to **File** > **Preferences** (`Ctrl + ,` or `Cmd + ,`).
3. Add the following URL to **Additional boards manager URLs**:
   ```text
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
4. Click **OK**.
5. Open **Tools** > **Board** > **Boards Manager...**
6. Search for **`Raspberry Pi Pico/RP2040`** by *Earle F. Philhower, III* and click **Install**.

---

## Step 2: Configure Arduino IDE Board Settings

Selecting the correct profile prevents boot-loop crashes and enables serial CDC communication.

Under the **Tools** menu, configure the following:

| Setting | Value | Notes |
| :--- | :--- | :--- |
| **Board** | `Raspberry Pi RP2040` &rarr; **`VCC-GND YD-RP2040`** | **Do not** use standard "Raspberry Pi Pico" (it limits flash to 2MB). |
| **Flash Size** | **`16MB (no FS)`** *(or your desired FS split)* | Enables the full 16MB memory space. |
| **USB Stack** | **`Pico SDK`** *(or `Adafruit TinyUSB`)* | Ensures USB CDC serial enumerates on startup. |
| **CPU Speed** | `133 MHz` (Default) | |
| **Optimize** | `Small (-Os) (standard)` | |

> **Note on "Generic RP2040":** If using the `Generic RP2040` profile instead of `VCC-GND YD-RP2040`, ensure **Boot Stage 2** is set to **`W25Q080 /2`** or **`W25Q128 /2`**. Setting it to `"Generic SPI /2"` causes immediate boot crashes.

---

## Step 3: First-Time Flashing (Entering Bootloader Mode)

On a new or unprogrammed board, Windows will not recognize a COM port until firmware with active USB Serial has been loaded.

1. **Unplug** the USB-C cable.
2. Press and hold the **`BOOT`** button.
3. Plug the USB-C cable into your computer, then release **`BOOT`**.
4. A removable drive named **`RPI-RP2`** will mount in your file explorer.
5. In Arduino IDE, go to **Tools** > **Port** and select **`UF2 Board`** (listed under *uf2conv ports*).
6. Click **Upload** ($\rightarrow$).

> **Normal Behavior:** During flashing, the drive `RPI-RP2` will automatically unmount, and the IDE console will output:
> ```text
> Flashing E: (RPI-RP2)
> Wrote xxxxxx bytes to E:/NEW.UF2
> ```
> Within 2–3 seconds, the board will reboot and register a new **COM port** under **Device Manager** / **Tools > Port**. Future uploads can target that COM port directly without holding buttons.

---

## Step 4: Verification Sketches

### 1. Serial Monitor Test

Upload the sketch below to verify USB Serial connectivity.

```cpp
void setup() {
  Serial.begin(115200);
  // Wait up to 4 seconds for USB Serial monitor to open
  while (!Serial && millis() < 4000);
  Serial.println("YD-RP2040 Initialized Successfully!");
}

void loop() {
  Serial.print("Uptime: ");
  Serial.print(millis() / 1000);
  Serial.println(" seconds");
  delay(1000);
}
```

1. Select the newly detected **COM port** under **Tools** > **Port**.
2. Open the **Serial Monitor** at **115200 baud**.

---

### 2. Built-in RGB NeoPixel Test (GPIO 23)

The onboard WS2812 addressable LED on the YD-RP2040 is connected to **GPIO 23**.

1. Go to **Sketch** > **Include Library** > **Manage Libraries...**
2. Install **`Adafruit NeoPixel`**.
3. Upload the following sketch:

```cpp
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN   23
#define NUM_PIXELS     1

Adafruit_NeoPixel rgb(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  rgb.begin();
  rgb.setBrightness(40); // Set brightness to avoid excessive glare
  rgb.show();
}

void loop() {
  // Red
  rgb.setPixelColor(0, rgb.Color(255, 0, 0));
  rgb.show();
  delay(500);

  // Green
  rgb.setPixelColor(0, rgb.Color(0, 255, 0));
  rgb.show();
  delay(500);

  // Blue
  rgb.setPixelColor(0, rgb.Color(0, 0, 255));
  rgb.show();
  delay(500);
}
```

---

## Troubleshooting

### `Scanning for RP2040 devices / No drive to deploy`
* **Cause:** The IDE could not automatically reset the board into bootloader mode.
* **Fix:** Manually enter bootloader mode:
  * If your board has a **RESET** button: Hold **`BOOT`**, press & release **`RESET`**, then release **`BOOT`**.
  * If your board lacks a reset button: Unplug the cable, hold **`BOOT`**, plug it in, and release **`BOOT`**.
  * Check **Tools** > **Port** and verify **`UF2 Board`** is selected before uploading.

### Board Disappears and No Serial Port Appears
* **Cause:** Second-stage bootloader mismatch. The RP2040 crashed immediately upon reading the flash.
* **Fix:** Ensure the board profile is set to **`VCC-GND YD-RP2040`** (or change `Boot Stage 2` to `W25Q080 /2` under `Generic RP2040`). Avoid generic SPI modes.

### Flash Memory Corruption / Board Not Responding
* To completely reset and wipe corrupted flash:
  1. Download the official Raspberry Pi [flash_nuke.uf2](https://datasheets.raspberrypi.com/soft/flash_nuke.uf2).
  2. Put the board into bootloader mode (`RPI-RP2` drive appears).
  3. Drag and drop `flash_nuke.uf2` into `RPI-RP2`.
  4. The board will wipe the entire flash chip and automatically remount in bootloader mode ready for a clean upload.

---

## License & Credits
* Core: [arduino-pico](https://github.com/earlephilhower/arduino-pico) by Earle F. Philhower, III.
* Hardware: VCC-GND Studio / Raspberry Pi Ltd.