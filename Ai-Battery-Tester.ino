/*
 * =====================================================
 * 18650 Battery Capacity Tester - 3 Channel (Hardware V6.8.1)
 * Modular Architecture - AI Optimized
 * =====================================================
 */

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
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
#include "display_tft.h"
#include "web_server_api.h"

// ══════════════════════════════════════════════════
// ─── Setup ────────────────────────────────────────
// ══════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);

  baleQueue = xQueueCreate(10, sizeof(String*));
  xTaskCreatePinnedToCore(baleTask, "BaleTask", 8192, NULL, 1, NULL, 0);

  for (int i = 0; i < 3; i++) {
    pinMode(hwPins[i].pin_d500, OUTPUT);
    pinMode(hwPins[i].pin_d1000, OUTPUT);
    pinMode(hwPins[i].pin_ch_en, OUTPUT);
    setHwState(i, IDLE);
  }

  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed!");
    tft.fillScreen(TFT_BLACK);
    tft.setTextFont(4); 
    tft.setTextSize(1); 
    tft.setTextColor(TFT_RED, TFT_BLACK); 
    tft.setCursor(10, 30); 
    tft.println("Storage Error!");
    delay(4000); 
  }

  Wire.begin(I2C_SDA, I2C_SCL);
  setupINA3221();

  ts.begin();
  ts.setRotation(2);

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  spr.createSprite(236, 95);

  tft.setTextFont(4); 
  tft.setTextSize(1); 
  tft.setTextColor(TFT_CYAN, TFT_BLACK); 
  tft.setCursor(5, 10); 
  tft.println("Li-ion Analyzer");
  tft.setTextFont(2); 
  tft.setTextSize(1); 
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK); 
  tft.setCursor(185, 15); 
  tft.println("V6.8.2"); // ورژن آپدیت شد
  tft.drawLine(5, 42, 235, 42, TFT_DARKGREY); 

  tft.setTextFont(4); 
  tft.setTextSize(1); 
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(5, 55); 
  tft.println("Initializing...");

  // ---> شروع خواندن اطلاعات وای‌فای از حافظه <---
  prefs.begin("triage", false);
  String current_ssid = prefs.getString("wifi_ssid", "Ermia"); 
  String current_pass = prefs.getString("wifi_pass", "8641682Ermia");
  // ---> پایان خواندن حافظه <---

  WiFi.mode(WIFI_STA);
  WiFi.begin(current_ssid.c_str(), current_pass.c_str());
  
  tft.setTextFont(4); 
  tft.setTextSize(1); 
  tft.setTextColor(TFT_YELLOW, TFT_BLACK); 
  tft.setCursor(5, 90);
  tft.println("Connecting WiFi...");

  Serial.print("Connecting to WiFi: ");
  Serial.println(current_ssid);

  int wifi_tries = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_tries < 20) {
    delay(500);
    Serial.print(".");
    wifi_tries++;
  }

  String currentIP;
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    currentIP = WiFi.localIP().toString();
    configTime(12600, 0, "ir.pool.ntp.org", "pool.ntp.org", "time.google.com");
    Serial.println("NTP Time Sync Initiated.");
  } else {
    Serial.println("\nWiFi Failed! Starting AP...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP("Portable-Tester"); 
    currentIP = WiFi.softAPIP().toString();
    delay(1000);
  }
  
  setupWebServer();
  server.begin();

  tft.fillScreen(TFT_DARKGREEN);
  tft.setTextFont(4); 
  tft.setTextSize(1); 
  tft.setTextColor(TFT_WHITE, TFT_DARKGREEN);
  tft.setCursor(10, 15);
  tft.println("System Ready!");
  tft.setTextFont(4); 
  tft.setTextSize(1); 
  tft.setTextColor(TFT_YELLOW, TFT_DARKGREEN); 
  tft.setCursor(10, 55); 
  tft.printf("IP: %s\n", currentIP.c_str());
  delay(2000); 

  tft.fillScreen(TFT_NAVY);
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