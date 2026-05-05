#include <U8g2lib.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ================= LICENSE =================
// CZ: Tento kód je licencován pod Apache License 2.0.
// CZ: Při použití nebo úpravě musí být zachována informace o autorovi (copyright)
// CZ: a přiložena tato licence. Změny v kódu musí být označeny.
//
// EN: This code is licensed under the Apache License 2.0.
// EN: When using or modifying this code, the original author attribution (copyright)
// EN: must be preserved and this license must be included.
// EN: Any modifications must be clearly marked.

//-------------------------------------------------------------------------
// ESP32 JSON BOARD
// https://espressif.github.io/arduino-esp32/package_esp32_dev_index.json
//-------------------------------------------------------------------------

// ================= OLED =================
// CZ: Inicializace OLED displeje přes HW I2C (SCL=6, SDA=5)
// EN: OLED display initialization via HW I2C (SCL=6, SDA=5)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, 6, 5);

// ================= VIEWPORT =================
// CZ: Posunutí vykreslovací oblasti (pro menší displej)
// EN: Viewport offset (for smaller display usage)
int8_t screenWidth = 72;
int8_t screenHeight = 40;
int8_t xOffset = 28;
int8_t yOffset = 24;

// ================= WIFI =================
// CZ: WiFi přihlašovací údaje
// EN: WiFi credentials
const char* ssid = "YouWIFIname";
const char* password = "WIFIPASS";

// ================= API =================
// CZ: Solar Assistant API endpoint + přihlášení
// EN: Solar Assistant API endpoint + authentication
const char* url  = "http:// YOU LOCAL IP /api/v1/metrics"; 
const char* user = "admin";
const char* pass = "YOUlocalPASS";

// ================= DATA =================
// CZ: Proměnné pro ukládání hodnot
// EN: Variables storing fetched values
float pv = 0, bat = 0, load = 0, soc = 0, grid = 0;

// ================= TIMING =================
// CZ: Řízení času (fetch + přepínání obrazovek)
// EN: Timing control (fetch + screen rotation)
int screen = 0;
unsigned long lastSwitch = 0;
unsigned long lastFetch = 0;
bool redrawNeeded = true;

// ================= ICONS =================
// CZ: Jednoduché kreslení ikon (low memory)
// EN: Simple vector icons (low memory usage)

void icon_sun(int x, int y){
  u8g2.drawCircle(x+6,y+6,3);
  u8g2.drawLine(x+6,y,x+6,y+2); 
  u8g2.drawLine(x+6,y+10,x+6,y+12);
  u8g2.drawLine(x,y+6,x+2,y+6); 
  u8g2.drawLine(x+10,y+6,x+12,y+6);
}

void icon_battery(int x, int y){
  u8g2.drawFrame(x,y+2,12,6); 
  u8g2.drawBox(x+12,y+4,2,2);
}

void icon_house(int x, int y){
  u8g2.drawTriangle(x,y+6,x+6,y,x+12,y+6); 
  u8g2.drawFrame(x+3,y+6,6,6);
}

void icon_grid(int x, int y){
  u8g2.drawLine(x,y,x+12,y+12); 
  u8g2.drawLine(x+12,y,x,y+12);
}

void icon_percent(int x, int y){
  u8g2.drawCircle(x+3,y+3,2); 
  u8g2.drawCircle(x+9,y+9,2);
  u8g2.drawLine(x+2,y+11,x+11,y+2);
}

// ================= JSON PARSE =================
// CZ: Parsování JSON odpovědi z API
// EN: Parse JSON response from API
void parseJson(const String& json) {
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, json);

  if (error) return;

  JsonArray array = doc.as<JsonArray>();

  for (JsonObject item : array) {
    const char* topic = item["topic"];
    float value = item["value"];

    if (strcmp(topic, "total/pv_power") == 0) pv = value;
    else if (strcmp(topic, "total/battery_power") == 0) bat = value;
    else if (strcmp(topic, "total/load_power") == 0) load = value;
    else if (strcmp(topic, "total/battery_state_of_charge") == 0) soc = value;
    else if (strcmp(topic, "total/grid_power") == 0) grid = value;
  }
}

// ================= HTTP FETCH =================
// CZ: Načtení dat z API
// EN: Fetch data from API
void fetchData() {

  // CZ: Kontrola WiFi + reconnect
  // EN: WiFi check + reconnect
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    return;
  }

  HTTPClient http;
  http.begin(url);
  http.setAuthorization(user, pass);
  http.setTimeout(5000);

  int code = http.GET();

  if (code == 200) {
    String payload = http.getString();
    parseJson(payload);
  }

  http.end();
}

// ================= DRAW =================
// CZ: Univerzální vykreslení obrazovky
// EN: Universal screen renderer
void drawScreen(const char* label, int value, const char* unit, void (*icon)(int,int)) {

  u8g2.clearBuffer();
  u8g2.drawFrame(xOffset, yOffset, screenWidth, screenHeight);

  if(icon) icon(xOffset + 4, yOffset + 4);

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(xOffset + 20, yOffset + 14, label);

  u8g2.setFont(u8g2_font_7x14B_tf);
  char buf[20];
  snprintf(buf, sizeof(buf), "%d %s", value, unit);
  u8g2.drawStr(xOffset + 20, yOffset + 32, buf);

  u8g2.sendBuffer();
}

// ================= SETUP =================
// CZ: Inicializace zařízení
// EN: Device initialization
void setup() {

  u8g2.begin();
  u8g2.setBusClock(400000);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(xOffset, yOffset + 15, "Connecting...");
  u8g2.sendBuffer();

  WiFi.begin(ssid, password);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    u8g2.clearBuffer();
    u8g2.drawStr(xOffset, yOffset + 15, "WiFi OK!");
    u8g2.drawStr(xOffset, yOffset + 30, "Fetching...");
    u8g2.sendBuffer();
    fetchData();
  } else {
    u8g2.clearBuffer();
    u8g2.drawStr(xOffset, yOffset + 15, "WiFi Error!");
    u8g2.sendBuffer();
    delay(2000);
  }
}

// ================= LOOP =================
// CZ: Hlavní smyčka programu
// EN: Main program loop
void loop() {

  // CZ: Načtení dat každých 15s
  // EN: Fetch data every 15 seconds
  if (millis() - lastFetch > 15000) {
    fetchData();
    lastFetch = millis();
    redrawNeeded = true;
  }

  // CZ: Přepínání obrazovek každých 5s
  // EN: Rotate screens every 5 seconds
  if (millis() - lastSwitch > 5000) {
    screen = (screen + 1) % 5;
    lastSwitch = millis();
    redrawNeeded = true;
  }

  // CZ: Překreslení pouze pokud je potřeba
  // EN: Redraw only when needed
  if (redrawNeeded) {
    switch(screen) {
      case 0: drawScreen("SOC",  (int)soc,  "%", icon_percent); break;
      case 1: drawScreen("PV",   (int)pv,   "W", icon_sun);     break;
      case 2: drawScreen("BAT",  (int)bat,  "W", icon_battery); break;
      case 3: drawScreen("LOAD", (int)load, "W", icon_house);   break;
      case 4: drawScreen("GRID", (int)grid, "W", icon_grid);    break;
    }
    redrawNeeded = false;
  }

  delay(50);
}
