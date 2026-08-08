#ifndef WEB_SERVER_API_H
#define WEB_SERVER_API_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// این متغیرها در فایل اصلی تعریف شده‌اند
extern WebServer server;
extern Preferences prefs;

// دسترسی به آرایه اضافه شده در بک‌اند
extern uint8_t topUpRetries[3];

// ─── توابع سیستم لاگ یکپارچه (تعریف شده در logger_csv.h) ───
extern String getNextGlobalAutoName();
extern void incrementGlobalId();

// ─── Forward Declarations برای توابعی که در فایل اصلی هستند ───
void startChannel(uint8_t ch_idx, const String& name, ChannelState act);
void startAuto(uint8_t c, const String& name, AutoMode mode);
void stopChannel(uint8_t ch_idx);

// ─── راه‌اندازی و تعریف مسیرهای وب‌سرور (Routes) ───────────────
void setupWebServer() {
  
// ۱. صفحات اصلی (HTML)
  server.on("/", HTTP_GET, []() { server.send(200, "text/html; charset=utf-8", buildIndexPage()); });
  server.on("/settings", HTTP_GET, []() { server.send(200, "text/html; charset=utf-8", buildSettingsPage()); });
  server.on("/logs", HTTP_GET, []() { server.send(200, "text/html; charset=utf-8", buildLogPage()); });
  server.on("/triage", HTTP_GET, []() { server.send(200, "text/html; charset=utf-8", buildTriagePage()); });
  server.on("/pack_builder", HTTP_GET, []() { server.send(200, "text/html; charset=utf-8", buildPackBuilderPage()); });

  // ۲. دریافت اطلاعات آنلاین وضعیت سیستم (JSON)
  server.on("/data", HTTP_GET, []() {
    JsonDocument doc; 
    JsonArray channels = doc["channels"].to<JsonArray>();

    for (uint8_t c = 0; c < 3; c++) {
      JsonObject chObj = channels.add<JsonObject>();
      
      unsigned long elapsed = 0;
      if (ch[c].state != IDLE && ch[c].state != DONE) elapsed = millis() - ch[c].startMillis;
      else if (ch[c].state == DONE) elapsed = ch[c].endMillis - ch[c].startMillis;
      
      long restRemain = 0;
      if (ch[c].state == IDLE && (ch[c].autoPhase == PHASE_REST || ch[c].autoPhase == PHASE_REST_POST)) { 
         unsigned long restDuration;
         if (ch[c].autoPhase == PHASE_REST) {
           restDuration = (ch[c].autoMode == AUTO_TRIAGE) ? 45000UL : 300000UL; 
         } else {
           restDuration = 60000UL;
         }
         unsigned long elapsedRest = millis() - ch[c].restStartMillis;
         restRemain = (elapsedRest < restDuration) ? (restDuration - elapsedRest) / 1000 : 0;
      }

      float health = min(100.0f, (ch[c].mAh / 2500.0f) * 100.0f);
      float displayMah = (ch[c].autoPhase == PHASE_FINAL_CHG) ? ch[c].recordedCapacity : ch[c].mAh;

      chObj["qt_grade"] = ch[c].qt_grade;
      chObj["qt_dcir"] = String(ch[c].qt_dcir, 1);
      chObj["qt_rec"] = String(ch[c].qt_recoveryRate, 1);
      chObj["state"] = (int)ch[c].state;
      chObj["auto"] = (int)ch[c].autoMode;
      chObj["autophase"] = (int)ch[c].autoPhase;
      chObj["rest_rem"] = restRemain;
      chObj["name"] = ch[c].name;
      
      chObj["v"] = String(ch[c].voltNow, 3);
      chObj["i"] = String(ch[c].currentNow, 0);
      chObj["mah"] = String(displayMah, 1);
      chObj["ir"] = String(ch[c].IR, 1);
      chObj["elapsed"] = durationStr(elapsed);
      chObj["vstart"] = String(ch[c].voltStart, 3);
      chObj["health"] = String(health, 0);
      
      chObj["topup"] = topUpRetries[c]; 
    }
    
    doc["time"] = timeStr(nowEpoch());

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // ۳. API مربوط به پروفایل تریاژ
  server.on("/getprofile", HTTP_GET, []() {
    String json = "{\"p\":2,\"a\":80.0,\"b\":150.0,\"c\":300.0}";
    server.send(200, "application/json", json);
  });

  // ۴. API مربوط به گراف هوشمند
  server.on("/api/graph", HTTP_GET, []() {
    if (!server.hasArg("ch")) { server.send(400, "text/plain", "missing ch"); return; }
    int c = server.arg("ch").toInt();
    if (c < 0 || c >= 3) { server.send(400, "text/plain", "bad ch"); return; }
    
    JsonDocument doc;
    JsonArray dataArray = doc.to<JsonArray>();
    
    for (int i = 0; i < ch[c].graphCount; i++) {
      dataArray.add(serialized(String(ch[c].graphV[i], 3))); 
    }
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // ۵. دریافت دستورات کنترلی از کلاینت (Action)
  server.on("/action", HTTP_POST, []() {
    String body = server.arg("plain");
    int chIdx = -1; String name = ""; String act = "";
    
    int p = body.indexOf("ch=");
    if (p >= 0) chIdx = body.substring(p+3, body.indexOf('&', p)).toInt();
    
    p = body.indexOf("act=");
    if (p >= 0) act = body.substring(p+4, body.indexOf('&', p));
    
    p = body.indexOf("name=");
    if (p >= 0) {
      name = server.urlDecode(body.substring(p+5)); 
    }
    
    name.trim(); 
    
    if (name.length() == 0) {
        name = getNextGlobalAutoName();
        incrementGlobalId();
    }

    if (chIdx >= 0 && chIdx < 3) {
      if (act == "stop") {
        ch[chIdx].isCanceled = true; 
        stopChannel(chIdx);          
        server.send(200, "text/plain", "Stopped");
        return; 
      }
    } 

    if (chIdx >= 0 && chIdx < 3 && ch[chIdx].state == IDLE && ch[chIdx].autoPhase == PHASE_NONE) {
      if (act == "charge") startChannel(chIdx, name, CHARGING);
      else if (act == "d500") startChannel(chIdx, name, DISCHARGE_500);
      else if (act == "d1000") startChannel(chIdx, name, DISCHARGE_1000);
      else if (act == "d1500") startChannel(chIdx, name, DISCHARGE_1500);
      else if (act == "auto500") startAuto(chIdx, name, AUTO_500);
      else if (act == "auto1000") startAuto(chIdx, name, AUTO_1000);
      else if (act == "auto1500") startAuto(chIdx, name, AUTO_1500);
      else if (act == "triage") startChannel(chIdx, name, Triage);
      else if (act == "autotriage") startAuto(chIdx, name, AUTO_TRIAGE);
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Error: Channel is busy or in Auto-Rest phase");
    }
  });

  // ۶. دستور توقف دستی
  server.on("/stop", HTTP_POST, []() {
    int chIdx = server.arg("plain").toInt();
    if (chIdx >= 0 && chIdx < 3 && ch[chIdx].state != IDLE && ch[chIdx].state != DONE) {
      ch[chIdx].autoMode = AUTO_OFF;
      ch[chIdx].autoPhase = PHASE_NONE;
      stopChannel(chIdx); 
      server.send(200, "text/plain", "OK");
    } else server.send(400, "text/plain", "Error");
  });

  // ۷. ریست کانال
  server.on("/reset", HTTP_POST, []() {
    int chIdx = server.arg("plain").toInt();
    if (chIdx >= 0 && chIdx < 3 && (ch[chIdx].state == IDLE || ch[chIdx].state == DONE)) {
      ch[chIdx] = Channel(); server.send(200, "text/plain", "OK");
    } else server.send(400, "text/plain", "Error");
  });

  // ۸. تنظیم زمان (NTP Sync)
  server.on("/settime", HTTP_POST, []() {
    String body = server.arg("plain");
    int p = body.indexOf("epoch=");
    if (p >= 0) {
      time_t epoch = (time_t)body.substring(p+6).toInt();
      struct timeval tv;
      tv.tv_sec = epoch;
      tv.tv_usec = 0;
      settimeofday(&tv, NULL); 
      server.send(200, "text/plain", "Time Sync Success");
    } else server.send(400, "text/plain", "Error");
  });

  // ۹. دانلود فایل لاگ یکپارچه
  server.on("/global_log", HTTP_GET, []() {
    if (LittleFS.exists("/global_log.csv")) { 
        File f = LittleFS.open("/global_log.csv", FILE_READ); 
        server.streamFile(f, "text/csv"); 
        f.close(); 
    }
    else server.send(404, "text/plain", "Empty");
  });

  // ۱۰. پاک‌کردن لاگ‌ها (یکپارچه شده)
  server.on("/clearlog", HTTP_POST, []() {
    if (LittleFS.exists("/global_log.csv")) {
        LittleFS.remove("/global_log.csv");
    }
    prefs.putUInt("global_id", 1); 
    server.send(200, "text/plain", "Log Cleared");
  });

  // ۱۱. مدیریت شبکه‌های WiFi
  server.on("/scanwifi", HTTP_GET, []() {
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_FAILED) {
      WiFi.scanNetworks(true); 
      server.send(202, "application/json", "{\"status\":\"scanning\"}");
    } 
    else if (n == WIFI_SCAN_RUNNING) {
      server.send(202, "application/json", "{\"status\":\"scanning\"}");
    } 
    else {
      JsonDocument doc;
      JsonArray nets = doc["networks"].to<JsonArray>();
      for (int i = 0; i < n; ++i) {
        JsonObject obj = nets.add<JsonObject>();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
      }
      WiFi.scanDelete(); 
      String res;
      serializeJson(doc, res);
      server.send(200, "application/json", res);
    }
  });

  // واکشی شبکه‌های ذخیره شده روی دستگاه
  server.on("/getwifi", HTTP_GET, []() {
    String wifiJson = prefs.getString("wifi_list", "[]");
    server.send(200, "application/json", wifiJson);
  });

  // حذف شبکه از حافظه دستگاه
  server.on("/delwifi", HTTP_POST, []() {
    String ssid = server.arg("plain");
    String wifiJson = prefs.getString("wifi_list", "[]");
    JsonDocument doc;
    deserializeJson(doc, wifiJson);
    if (doc.is<JsonArray>()) {
      JsonArray arr = doc.as<JsonArray>();
      for (int i = 0; i < arr.size(); i++) {
        if (arr[i]["s"] == ssid) {
          arr.remove(i);
          break;
        }
      }
      String out;
      serializeJson(doc, out);
      prefs.putString("wifi_list", out);
    }
    server.send(200, "text/plain", "OK");
  });

  // افزودن یا اتصال به شبکه
  server.on("/setwifi", HTTP_POST, []() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    
    // حذف فاصله‌های پنهان احتمالی
    ssid.trim();
    pass.trim();
    
    if (ssid.length() > 0) {
      // قطع اتصال بدون خاموش کردن کامل ماژول
      WiFi.disconnect();
      delay(100);
      
      // قرار دادن در حالت AP_STA برای پایداری کانکشن فعلی کاربر
      WiFi.mode(WIFI_AP_STA);
      delay(100); 
      
      if (pass.length() > 0) {
          WiFi.begin(ssid.c_str(), pass.c_str());
      } else {
          WiFi.begin(ssid.c_str());
      }
      
      int tries = 0;
      // افزایش زمان مهلت به ۲۰ ثانیه (۴۰ تست ۵۰۰ میلی‌ثانیه‌ای)
      while (WiFi.status() != WL_CONNECTED && tries < 40) {
        delay(500); 
        tries++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        String wifiJson = prefs.getString("wifi_list", "[]");
        JsonDocument doc;
        deserializeJson(doc, wifiJson);
        JsonArray arr = doc.is<JsonArray>() ? doc.as<JsonArray>() : doc.to<JsonArray>();
        
        bool found = false;
        for (JsonObject nw : arr) {
          if (nw["s"] == ssid) {
            nw["p"] = pass; 
            found = true;
            break;
          }
        }
        if (!found) {
          JsonObject newNw = arr.add<JsonObject>();
          newNw["s"] = ssid;
          newNw["p"] = pass;
        }
        
        String out;
        serializeJson(doc, out);
        prefs.putString("wifi_list", out);
        
        server.send(200, "text/plain", "OK");
        delay(1000);
        ESP.restart(); 
      } else {
        // بازگشت مطمئن به حالت هات‌اسپات در صورت خطا
        WiFi.disconnect();
        delay(100);
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Portable-Tester");
        server.send(400, "text/plain", "FAIL");
      }
    } else {
      server.send(400, "text/plain", "INVALID");
    }
  });
}

#endif