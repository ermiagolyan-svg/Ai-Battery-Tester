/*
 * =====================================================
 * 18650 Battery Capacity Tester - 3 Channel (Hardware V6.8.2)
 * Modular Architecture - AI Optimized
 * =====================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiMulti.h> 
#include <WebServer.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <time.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Preferences.h>

// ─── Pin Definitions ───────────────────────────────
#define I2C_SDA     4
#define I2C_SCL     16

#define CH1_D500    33
#define CH1_D1000   25
#define CH1_CH_EN   2

#define CH2_D500    26
#define CH2_D1000   27
#define CH2_CH_EN   17

#define CH3_D500    14
#define CH3_D1000   12
#define CH3_CH_EN   21

#define TOUCH_CS    22
#define TOUCH_IRQ   5

// ─── Global Objects ────────────────────────────────
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

WebServer server(80);

IPAddress local_IP(192, 168, 1, 66);
IPAddress gateway(192, 168, 1, 1);    
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);     

QueueHandle_t baleQueue;
SemaphoreHandle_t csvMutex;

// ─── Constants ─────────────────────────────────────
const float CUTOFF_DISCHARGE = 2.80f;
const float CUTOFF_CHARGE    = 4.20f;
const uint8_t NUM_CHANNELS   = 3;
const unsigned long SAMPLE_MS = 500;

// ─── State Definitions ─────────────────────────────
enum ChannelState { IDLE, CHARGING, DISCHARGE_500, DISCHARGE_1000, DISCHARGE_1500, MEASURING_IR, Triage, DONE };
enum AutoMode { AUTO_OFF=0, AUTO_500=1, AUTO_1000=2, AUTO_1500=3, AUTO_TRIAGE=4 };
enum AutoPhase { PHASE_NONE=0, PHASE_INIT_CHG=1, PHASE_REST=2, PHASE_DISCHG=3, PHASE_REST_POST=4, PHASE_FINAL_CHG=5 }; 
enum TriagePhase { QT_NONE=0, QT_REST_INIT=1, QT_PULSE_500=2, QT_REST_1=3, QT_PULSE_1000=4, QT_REST_2=5, QT_PULSE_1500=6, QT_REST_FINAL=7 };

struct ChannelPins {
  uint8_t pin_d500, pin_d1000, pin_ch_en;
};
ChannelPins hwPins[3] = {
  {CH1_D500, CH1_D1000, CH1_CH_EN},
  {CH2_D500, CH2_D1000, CH2_CH_EN},
  {CH3_D500, CH3_D1000, CH3_CH_EN}
};

struct Channel {
  ChannelState state       = IDLE;
  String       name        = "";
  float        voltStart   = 0;
  float        voltStartLoad = 0;
  float        voltNow     = 0;
  float        currentNow  = 0;
  float        mAh         = 0;
  float        IR          = 0;         
  unsigned long startMillis = 0;
  unsigned long endMillis   = 0;
  time_t       startTime   = 0;
  time_t       endTime     = 0;
  float        lastReport_mAh = 0.0f;
  
  AutoMode     autoMode    = AUTO_OFF;
  AutoPhase    autoPhase   = PHASE_NONE;
  float        recordedCapacity = 0.0f;
  unsigned long restStartMillis = 0; 
  
  time_t       sessionStartTime   = 0;
  unsigned long sessionStartMillis = 0;
  float        sessionVoltStart   = 0;

  TriagePhase qtPhase            = QT_NONE;
  unsigned long  qtPhaseStartMillis = 0;
  float          qt_voltBase        = 0;
  float          qt_voltDrop500     = 0;
  float          qt_voltRec500      = 0;
  float          qt_voltDrop1000    = 0;
  float          qt_voltRec1000     = 0;
  float          qt_voltDrop1500    = 0;
  String         qt_grade           = "";
  float          qt_dcir            = 0.0f;  
  float          qt_recoveryRate    = 0.0f;  
  String         qt_thermalStatus   = "";    

  float        graphV[240] = {0};
  int          graphCount = 0;
  unsigned long graphIntervalMs = 1000;
  unsigned long graphLastMs = 0;

  bool         hasFault    = false;
  String       faultMsg    = "";
  bool         isCanceled  = false;
};

Channel ch[NUM_CHANNELS];

// ─── Core Helper Functions ─────────────────────────
time_t nowEpoch() { 
  return time(NULL); 
}

Preferences prefs;

// ─── Custom Modules (Dependencies Order) ───────────
#include "bale_bot.h"
#include "sensor_ina3221.h"
#include "logger_csv.h"
#include "triage_logic.h"
#include "channel_control.h"
#include "page_index.h"
#include "page_settings.h"
#include "page_logs.h"
#include "page_triage.h"
#include "page_pack_builder.h"
#include "page_worker_manager.h"
#include "display_tft.h"
#include "web_server_api.h"

// ══════════════════════════════════════════════════
// ─── Setup ────────────────────────────────────────
// ══════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);

  csvMutex = xSemaphoreCreateMutex();
  
  baleQueue = xQueueCreate(10, sizeof(String*));
  xTaskCreatePinnedToCore(baleTask, "BaleTask", 8192, NULL, 1, NULL, 0);

  for (int i = 0; i < 3; i++) {
    pinMode(hwPins[i].pin_d500, OUTPUT);
    pinMode(hwPins[i].pin_d1000, OUTPUT);
    pinMode(hwPins[i].pin_ch_en, OUTPUT);
    setHwState(i, IDLE);
  }

  // --- راه‌اندازی نمایشگر و صفحه لودینگ مدرن ---
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(UI_BG); // استفاده از پالت رنگی جدید
  spr.createSprite(236, 96); // سایز متناسب با UI جدید

  // کادر لودینگ
  tft.drawRoundRect(10, 10, 220, 300, 12, UI_BORDER);
  
  tft.setTextFont(4); 
  tft.setTextColor(UI_ACCENT, UI_BG); 
  tft.drawCentreString("AI Battery Studio", 120, 30, 4);
  
  tft.setTextFont(2); 
  tft.setTextColor(UI_MUTED, UI_BG); 
  tft.drawCentreString("Hardware V6.8.2", 120, 55, 2); 
  tft.drawFastHLine(30, 80, 180, UI_BORDER); 

  tft.setTextColor(UI_TEXT, UI_BG);
  tft.drawString("Storage: Mounting...", 20, 100);

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    tft.setTextColor(UI_WARN, UI_BG); 
    tft.drawString("Storage Error!", 20, 100);
    delay(4000); 
  } else {
    tft.setTextColor(UI_OK, UI_BG); 
    tft.drawString("Storage: Ready       ", 20, 100);
  }

  tft.setTextColor(UI_TEXT, UI_BG);
  tft.drawString("Sensors: Init INA3221...", 20, 130);
  
  Wire.begin(I2C_SDA, I2C_SCL);
  setupINA3221();
  
  tft.setTextColor(UI_OK, UI_BG); 
  tft.drawString("Sensors: Ready          ", 20, 130);

  ts.begin();
  ts.setRotation(2);

  // ---> بارگذاری لیست شبکه‌ها از NVS در WiFiMulti <---
  WiFiMulti wifiMulti;
  prefs.begin("triage", false);
  String wifiJson = prefs.getString("wifi_list", "[]");
  JsonDocument doc;
  deserializeJson(doc, wifiJson);
  bool hasNetworks = false;
  
  if (doc.is<JsonArray>()) {
    JsonArray arr = doc.as<JsonArray>();
    for (JsonObject nw : arr) {
      const char* s = nw["s"];
      const char* p = nw["p"];
      if (s && strlen(s) > 0) {
        wifiMulti.addAP(s, p);
        hasNetworks = true;
      }
    }
  }
  
  if (!hasNetworks) {
     wifiMulti.addAP("Ermia", "8641682Ermia");
  }

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  
  tft.setTextColor(UI_ACCENT2, UI_BG); 
  tft.drawString("Network: Scanning...", 20, 160);

  Serial.print("Scanning and connecting to best saved WiFi");

  int wifi_tries = 0;
  bool wifiConnected = false;
  
  while (wifi_tries < 7) {
    if (wifiMulti.run() == WL_CONNECTED) {
      wifiConnected = true;
      break;
    }
    delay(100);
    Serial.print(".");
    wifi_tries++;
  }

  String currentIP;
  if (wifiConnected) {
    Serial.println("\nWiFi Connected to: " + WiFi.SSID());
    currentIP = WiFi.localIP().toString();
    configTime(12600, 0, "ir.pool.ntp.org", "pool.ntp.org", "time.google.com");
    Serial.println("NTP Time Sync Initiated.");
    
    tft.setTextColor(UI_OK, UI_BG);
    tft.drawString("Network: Connected  ", 20, 160);
  } else {
    Serial.println("\nWiFi Failed! Starting AP...");
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Portable-Tester"); 
    currentIP = WiFi.softAPIP().toString();
    delay(1000);
    
    tft.setTextColor(UI_WARN, UI_BG);
    tft.drawString("Network: AP Mode    ", 20, 160);
  }
  
  setupWebServer();
  server.begin();

  // صفحه آماده به کار (مینیمال و شیک)
  tft.fillRoundRect(20, 200, 200, 60, 8, UI_SURFACE);
  tft.drawRoundRect(20, 200, 200, 60, 8, UI_ACCENT);
  
  tft.setTextFont(4); 
  tft.setTextColor(UI_ACCENT, UI_SURFACE);
  tft.drawCentreString("System Ready!", 120, 210, 4);
  
  tft.setTextFont(2); 
  tft.setTextColor(UI_TEXT, UI_SURFACE); 
  char ipBuf[32];
  sprintf(ipBuf, "IP: %s", currentIP.c_str());
  tft.drawCentreString(ipBuf, 120, 240, 2);
  
  delay(2500); 

  // پاک کردن صفحه آماده برای نمایش کارت‌ها
  tft.fillScreen(UI_BG);
}

// ══════════════════════════════════════════════════
// ─── Loop ─────────────────────────────────────────
// ══════════════════════════════════════════════════
void loop() {
  server.handleClient();

  for (uint8_t c = 0; c < NUM_CHANNELS; c++) {
    if (ch[c].state == Triage) {
      processTriage(c);
    }
  }

  static unsigned long lastSample = 0;
  unsigned long now = millis();
  unsigned long dt = now - lastSample; 
  
  if (dt >= SAMPLE_MS) {
    lastSample = now;
    sampleAll(dt); 
    updateTFT(); 
  }
  delay(10);
}