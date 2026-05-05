# 📡 ESP32-C3 OLED + Solar Assistant API
Projekt slouží k zobrazení dat ze systému Solar Assistant na malém OLED displeji (0.42") pomocí mikrokontroleru ESP32-C3 přes WiFi.
Zařízení periodicky načítá data z API a zobrazuje je ve formě rotujících obrazovek (SOC, výkon panelů, baterie, spotřeba, síť).

---
## 🧰 Použitý hardware
- ESP32-C3 (WiFi mikrokontroler) OLED displej SSD1306 (I2C, 128x64 px)
- Solar Assistant (lokální API server)
---

## 📚 Použité knihovny
### U8g2
https://github.com/olikraus/u8g2

### ArduinoJson
https://github.com/bblanchon/ArduinoJson

WiFi a HTTPClient jsou součástí ESP32 core.
---

## 🌐 API
Endpoint:
http://<IP_ADRESA>/api/v1/metrics

Používá Basic Auth.
---

## ⚙️ Funkcionalita
- načítání dat každých 15s
- přepínání obrazovek každých 5s
- zobrazení:
  - SOC (%)
  - PV (W)
  - BAT (W)
  - LOAD (W)
  - GRID (W)
---

## 🚀 Spuštění
1. Nastav WiFi údaje v kódu
2. Nastav IP Solar Assistant
3. Nahraj do ESP32
4. Hotovo
---

## 🔧 Poznámky
- API musí být dostupné v síti
- používá HTTP (bez HTTPS validace)
- optimalizováno pro nízkou paměť

## Author
Tomáš Cabaj TMK 2026

![ESP32C3OLED](https://github.com/tomas-cabaj/Solar-assistant-API---ESP32/blob/main/img/TestEsp32SolarAssistance.png)
