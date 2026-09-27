# Flashing Micronucleus USB Bootloader to Blank ATtiny85 via Arduino Nano ISP

A complete step-by-step guide to reviving or configuring blank ATtiny85 ICs to work with Micro-USB Digispark development/programmer socket boards on Windows.

---

## 1. Background & Root Cause

Many inexpensive Micro-USB ATtiny85 boards feature an 8-pin DIP socket, power regulation (5V/3.3V), and passive Zener clamp diodes, but **lack an onboard USB-to-Serial bridge chip** (such as a CH340, FT232, or CP2102).

* **The Problem:** Brand-new, factory-sourced ATtiny85 ICs arrive unprogrammed (defaulting to a 1 MHz internal RC oscillator). Because they contain no bootloader code, they cannot bit-bang USB communication.
* **The Symptom:** When plugged into a PC, Windows reports:
  ```text
  Unknown USB Device (Device Descriptor Request Failed)
  ```
* **The Solution:** Flash the **Micronucleus v1.x/v2.x USB bootloader** and set the proper AVR fuse configuration once using an ISP programmer (such as an Arduino Nano). Once burned, the board programs directly over native Micro-USB using the Arduino IDE.

---

## 2. Hardware Orientation & Socket Alignment

When seating the ATtiny85 IC into the socket, verify orientation carefully:

```
                      [ Micro-USB Port ]
                      
                        === Notch ===   <-- Notch / Dot points toward USB
      (RESET / PB5)  Pin 1 [•]   [8]  VCC (+5V)
             (PB3)   Pin 2 [ ]   [7]  PB2 (SCK)
             (PB4)   Pin 3 [ ]   [6]  PB1 (MISO / Built-in LED)
             (GND)   Pin 4 [ ]   [5]  PB0 (MOSI)
                                        
                       [ 5V  GND  VIN ]  [ P0 - P5 Header ]
```

* **Pin 1:** Marked with a circular indent/dot on the chip surface. Must face toward the Micro-USB port (top-left).
* **Pin 4 (GND):** Aligns on the left side adjacent to the `5V GND VIN` header pins.
* **Pin 8 (VCC):** Sits at the top-right.

---

## 3. ISP Wiring: Arduino Nano to ATtiny85

Connect your Arduino Nano to the ATtiny85 (either inserted into the development socket or mounted on a breadboard):

| Arduino Nano Pin | ATtiny85 Physical Pin | Target Signal |
| :--- | :--- | :--- |
| **5V** | **Pin 8** | VCC |
| **GND** | **Pin 4** | GND |
| **D10** | **Pin 1** | RESET (PB5) |
| **D11** | **Pin 5** | MOSI (PB0) |
| **D12** | **Pin 6** | MISO (PB1) |
| **D13** | **Pin 7** | SCK (PB2) |

> **Crucial Tip (Nano Auto-Reset):** Connect a **10 µF electrolytic capacitor** between the Nano's **RESET** and **GND** pins (stripe/negative lead to GND) *after* uploading `ArduinoISP`. This prevents the computer from resetting the Nano during ISP programming operations.

---

## 4. Turn Arduino Nano into an ISP Programmer

1. Plug the **Arduino Nano only** into your computer.
2. In the Arduino IDE:
   * **Tools** > **Board** > **Arduino AVR Boards** > **Arduino Nano**
   * **Tools** > **Processor** > **ATmega328P** (or *ATmega328P (Old Bootloader)*)
   * **Tools** > **Port** > Select the Nano's COM port
3. Open **File** > **Examples** > **11.ArduinoISP** > **ArduinoISP**.
4. Click **Upload** and wait until it finishes (`Done uploading`).

---

## 5. Download the Micronucleus Bootloader Binary

Download the standard Digispark-compatible Micronucleus hex file:

* **File:** [t85_default.hex](https://raw.githubusercontent.com/micronucleus/micronucleus/master/firmware/releases/t85_default.hex)
* Save the file directly to your `C:\` drive as:
  ```text
  C:\t85_default.hex
  ```

---

## 6. One-Click AVRDUDE Flashing Script

Because Arduino IDE 2.x often fails with `Property 'bootloader.tool.serial' is undefined` when burning bootloaders via third-party Digistump cores, flashing directly with `avrdude` bypasses IDE core configuration bugs.

1. Open Notepad and paste the following script:

```bat
@echo off
set /p USERPORT=Enter Arduino Nano COM Port (e.g. COM13 or 13): 

:: Format port string for Windows high-numbered COM ports
set PORT=%USERPORT:COM=%
set PORT=\\.\COM%PORT%

:: Automatically locate AVRDUDE bundled inside the Arduino15 directory
for /f "delims=" %%i in ('dir /b /s "%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\avrdude.exe"') do set AVRDUDE="%%i"
for /f "delims=" %%i in ('dir /b /s "%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\avrdude.conf"') do set CONF="%%i"

echo.
echo =======================================================
echo Programmer Tool : %AVRDUDE%
echo Config File     : %CONF%
echo Target Port     : %PORT%
echo Target Device   : ATtiny85
echo =======================================================
echo.

%AVRDUDE% -C %CONF% -c stk500v1 -P %PORT% -b 19200 -p t85 -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m -U flash:w:"C:\t85_default.hex":i

pause
```

2. Save the file as **`flash_bootloader.bat`** (set *Save as type* to *All Files (`*.*`)*).
3. Close the Arduino IDE and its Serial Monitor so the COM port is completely free.
4. Run **`flash_bootloader.bat`**.
5. Enter your Nano's COM port (e.g. `13` or `COM13`) and press Enter.

### Target Fuse Configuration Explained
* **Low Fuse (`0xE1`):** Configures the PLL clock for high-frequency internal operation required by software USB.
* **High Fuse (`0xDD`):** Preserves SPI programming and leaves the external RESET pin functional for future reflashing.
* **Extended Fuse (`0xFE`):** Self-programming enable for bootloader operation.

A successful run outputs:
```text
Reading 1514 bytes for flash from input file t85_default.hex
Writing 1514 bytes to flash
Writing | ################################################## | 100%
Reading | ################################################## | 100%
1514 bytes of flash verified

Avrdude done.  Thank you.
```

---

## 7. Installing Micronucleus Windows Drivers

1. Download the official driver archive: [Digistump.Drivers.zip](https://github.com/digistump/DigistumpArduino/releases/download/1.6.7/Digistump.Drivers.zip).
2. Extract the archive.
3. Right-click **`DPinst64.exe`** (or `DPinst.exe` on 32-bit machines) and choose **Run as administrator**.
4. Confirm installation of the `libusb-win32` driver.

---

## 8. Uploading Code Over Native Micro-USB

1. Disconnect all ISP wiring from the Arduino Nano and remove the Nano.
2. Ensure the ATtiny85 is firmly seated in the socket of the black USB board.
3. **DO NOT plug the board into your computer yet.**
4. In the Arduino IDE:
   * Add the Digistump board URL under **File** > **Preferences** > **Additional boards manager URLs**:
     ```text
     https://raw.githubusercontent.com/ArminJo/DigistumpArduino/master/package_digistump_index.json
     ```
   * Install **Digistump AVR Boards** via **Tools** > **Board** > **Boards Manager...**
   * Select: **Tools** > **Board** > **Digistump AVR** > **Digispark (Default - 16.5mhz)**
   * Leave **Tools** > **Port** blank/unselected.
5. Create a test Blink sketch:

```cpp
void setup() {
  // Pin 1 (PB1) controls the built-in LED on Digispark modules
  pinMode(1, OUTPUT);
}

void loop() {
  digitalWrite(1, HIGH);
  delay(500);
  digitalWrite(1, LOW);
  delay(500);
}
```

6. Click the standard **Upload** arrow button in the Arduino IDE.
7. Monitor the bottom console. When you see:
   ```text
   Running Digispark Uploader...
   Plug in device now... (will timeout in 60 seconds)
   ```
8. **Plug the board into a USB port on your PC.**
9. Micronucleus handshakes with the computer, transfers the compiled sketch, and the onboard LED will start blinking immediately.

---

## 9. Troubleshooting Tips

* **USB 3.0 / Handshake Timeouts:** V-USB uses software bit-banged USB 1.1 timing. If a modern USB 3.0/3.1 port fails to catch the bootloader window, plug the board into a **USB 2.0 port** or through an unpowered **USB 2.0 hub**.
* **Power-Only USB Cables:** Verify your Micro-USB cable contains data lines. Power-only charging cables cannot enumerate.
* **Future Sketch Uploads:** Always remember the Digispark workflow: **hit Upload first, plug in the board second**.