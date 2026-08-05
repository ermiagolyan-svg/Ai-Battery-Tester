#ifndef WEB_SERVER_API_H
#define WEB_SERVER_API_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// این متغیر در فایل اصلی تعریف شده است، در اینجا فقط به آن اشاره می‌کنیم
extern WebServer server;
extern Preferences prefs;

// دسترسی به آرایه اضافه شده در بک‌اند
extern uint8_t topUpRetries[3];

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
      
      // اضافه کردن مقدار topUpRetries به خروجی JSON
      chObj["topup"] = topUpRetries[c]; 
    }
    
    doc["time"] = timeStr(nowEpoch());

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });

  // ۳. API مربوط به پروفایل تریاژ (منسوخ شده - توسط هوش مصنوعی جایگزین شد، صرفاً دیتای فیک می‌فرستد تا فرانت‌اند خطا ندهد)
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
      if (chIdx >= 0 && chIdx < 3) {
        name = generateAutoName(chIdx); 
      } else {
        name = "Battery";
      }
    }

    // ---> بلاک جدید برای هندل کردن توقف دستی <---
    if (chIdx >= 0 && chIdx < 3) {
      if (act == "stop") {
        ch[chIdx].isCanceled = true; // علامت‌گذاری به عنوان توقف دستی
        stopChannel(chIdx);          // فراخوانی تابع توقف
        server.send(200, "text/plain", "Stopped");
        return; // خروج از تابع
      }
    }

    // ---> بررسی وضعیت برای شروع تست‌های جدید <---
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

  // ۹. دانلود فایل‌های CSV
  server.on("/log1", HTTP_GET, []() {
    if (LittleFS.exists("/log_ch1.csv")) { File f = LittleFS.open("/log_ch1.csv", FILE_READ); server.streamFile(f, "text/csv"); f.close(); }
    else server.send(404, "text/plain", "Empty");
  });
  server.on("/log2", HTTP_GET, []() {
    if (LittleFS.exists("/log_ch2.csv")) { File f = LittleFS.open("/log_ch2.csv", FILE_READ); server.streamFile(f, "text/csv"); f.close(); }
    else server.send(404, "text/plain", "Empty");
  });
  server.on("/log3", HTTP_GET, []() {
    if (LittleFS.exists("/log_ch3.csv")) { File f = LittleFS.open("/log_ch3.csv", FILE_READ); server.streamFile(f, "text/csv"); f.close(); }
    else server.send(404, "text/plain", "Empty");
  });

  // ۱۰. پاک‌کردن لاگ‌ها
  server.on("/clearlog", HTTP_POST, []() {
    int chIdx = server.arg("plain").toInt();
    if (chIdx >= 0 && chIdx < 3) {
      String fn = "/log_ch" + String(chIdx+1) + ".csv";
      String cntFn = "/cnt_ch" + String(chIdx+1) + ".txt"; 
      
      if (LittleFS.exists(fn)) LittleFS.remove(fn);
      if (LittleFS.exists(cntFn)) LittleFS.remove(cntFn); 
    } else {
      LittleFS.remove("/log_ch1.csv"); 
      LittleFS.remove("/log_ch2.csv"); 
      LittleFS.remove("/log_ch3.csv");
      LittleFS.remove("/cnt_ch1.txt"); 
      LittleFS.remove("/cnt_ch2.txt"); 
      LittleFS.remove("/cnt_ch3.txt");
    }
    server.send(200, "text/plain", "Log Cleared");
  });

server.on("/scanwifi", HTTP_GET, []() {
    int n = WiFi.scanComplete();
    
    if (n == WIFI_SCAN_FAILED) {
      // شروع اسکن غیرمسدودکننده (Async) در بک‌گراند
      WiFi.scanNetworks(true); 
      server.send(202, "application/json", "{\"status\":\"scanning\"}");
    } 
    else if (n == WIFI_SCAN_RUNNING) {
      // اسکن هنوز در حال انجام است
      server.send(202, "application/json", "{\"status\":\"scanning\"}");
    } 
    else {
      // نتایج آماده است
      JsonDocument doc;
      JsonArray nets = doc["networks"].to<JsonArray>();
      for (int i = 0; i < n; ++i) {
        JsonObject obj = nets.add<JsonObject>();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
      }
      WiFi.scanDelete(); // پاکسازی حافظه اسکن
      String res;
      serializeJson(doc, res);
      server.send(200, "application/json", res);
    }
  });

  server.on("/setwifi", HTTP_POST, []() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    
    if (ssid.length() > 0) {
      WiFi.disconnect();
      delay(100);
      
      // تلاش برای اتصال تستی
      WiFi.begin(ssid.c_str(), pass.c_str());
      int tries = 0;
      // حداکثر ۷.۵ ثانیه منتظر می‌ماند تا وضعیت اتصال را بررسی کند
      while (WiFi.status() != WL_CONNECTED && tries < 15) {
        delay(500); 
        tries++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        // اتصال موفقیت‌آمیز بود
        prefs.putString("wifi_ssid", ssid);
        prefs.putString("wifi_pass", pass);
        server.send(200, "text/plain", "اتصال موفق! دستگاه در حال ریستارت است...");
        delay(1500);
        ESP.restart(); 
      } else {
        // عدم موفقیت در اتصال، بازگشت به شبکه هات‌اسپات دستگاه
        WiFi.disconnect();
        delay(100);
        WiFi.mode(WIFI_AP);
        WiFi.softAP("Portable-Tester");
        server.send(400, "text/plain", "خطا: رمز عبور اشتباه است یا مودم خارج از برد است!");
      }
    } else {
      server.send(400, "text/plain", "SSID نامعتبر است");
    }
  });
}

#endif