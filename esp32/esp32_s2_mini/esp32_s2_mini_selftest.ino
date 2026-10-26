// ESP32-S2 Mini (ESP32-S2FNR2) self test
// Board: LOLIN S2 Mini | USB CDC On Boot: Enabled | Serial Monitor: 115200

#include <WiFi.h>
#include <Wire.h>

#define LED_PIN 15
#define I2C_SDA 8
#define I2C_SCL 9

void printChipInfo() {
  WiFi.mode(WIFI_STA);  // init Wi-Fi first, otherwise the MAC reads as all zeros
  Serial.println("=== Chip info ===");
  Serial.printf("Model:      %s\n", ESP.getChipModel());
  Serial.printf("Revision:   %d\n", ESP.getChipRevision());
  Serial.printf("CPU freq:   %u MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash size: %u KB\n", ESP.getFlashChipSize() / 1024);
  Serial.printf("PSRAM size: %u KB\n", ESP.getPsramSize() / 1024);
  Serial.printf("Free heap:  %u KB\n", ESP.getFreeHeap() / 1024);
  Serial.printf("MAC:        %s\n", WiFi.macAddress().c_str());
}

void scanWiFi() {
  Serial.println("\n=== Wi-Fi scan ===");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("No networks found");
    return;
  }
  for (int i = 0; i < n; i++) {
    Serial.printf("%2d: %-24s ch%-2d %4d dBm\n", i + 1,
                  WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
  }
}

void scanI2C() {
  Serial.println("\n=== I2C scan (SDA=8, SCL=9) ===");
  Wire.begin(I2C_SDA, I2C_SCL);
  int found = 0;
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Device at 0x%02X\n", addr);
      found++;
    }
  }
  if (!found) Serial.println("No I2C devices found");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  delay(2000);  // give the USB CDC port time to enumerate

  printChipInfo();
  scanWiFi();
  scanI2C();
  Serial.println("\nDone. LED blinking.");
}

void loop() {
  digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  delay(500);
}
