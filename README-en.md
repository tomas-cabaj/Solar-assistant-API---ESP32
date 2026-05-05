# 📡 ESP32-C3 OLED + Solar Assistant API

This project displays real-time data from a Solar Assistant system on a small OLED display (0.42") using an ESP32-C3 microcontroller over WiFi.

The device periodically fetches data from a local API and presents it across rotating screens (SOC, PV power, battery, load, grid).

---

## 🧰 Hardware

- ESP32-C3 (WiFi-enabled microcontroller) OLED display SSD1306 (I2C, 128x64 px)
- Solar Assistant (local API server)

---

## 📚 Libraries

### U8g2
https://github.com/olikraus/u8g2

### ArduinoJson
https://github.com/bblanchon/ArduinoJson

WiFi and HTTPClient are part of the ESP32 core.

---

## 🌐 API

http://<IP_ADDRESS>/api/v1/metrics

Uses Basic Auth.

---

## ⚙️ Features

- Fetch data every 15 seconds
- Rotate screens every 5 seconds
- Displays:
  - SOC (%)
  - PV (W)
  - Battery (W)
  - Load (W)
  - Grid (W)

---

## 🚀 Getting Started

1. Set WiFi credentials in the code
2. Set Solar Assistant IP
3. Upload to ESP32
4. Done

---

## 🔧 Notes

- API must be accessible in local network
- Uses HTTP (no HTTPS)
- Optimized for low memory


## Author
Tomáš Cabaj TMK 2026
