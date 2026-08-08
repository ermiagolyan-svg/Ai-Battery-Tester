#ifndef LOGGER_CSV_H
#define LOGGER_CSV_H

#include <Arduino.h>
#include <LittleFS.h>
#include <time.h>
#include <Preferences.h> 

// ارجاع به متغیرها و قفل‌های تعریف شده در فایل اصلی
extern Preferences prefs;
extern SemaphoreHandle_t csvMutex;

// ─── توابع تبدیل زمان ──────────────────────────────────
String timeStr(time_t t) {
  if (t == 0) return "--";
  struct tm* tm_info = localtime(&t);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
  return String(buf);
}

String durationStr(unsigned long ms) {
  unsigned long s = ms / 1000;
  unsigned long h = s / 3600;
  unsigned long m = (s % 3600) / 60;
  s = s % 60;
  char buf[16];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", h, m, s);
  return String(buf);
}

// ─── سیستم نام‌گذاری یکپارچه (Global Tagging) ───────────
String getNextGlobalAutoName() {
  uint16_t id = prefs.getUInt("global_id", 1);
  char buf[16];
  snprintf(buf, sizeof(buf), "%03u", id);
  return String(buf);
}

void incrementGlobalId() {
  uint16_t id = prefs.getUInt("global_id", 1);
  prefs.putUInt("global_id", id + 1);
}

// ─── تابع اصلی ذخیره لاگ CSV یکپارچه ───────────────────
void saveCSV(uint8_t c) {
  ch[c].name.trim();
  
  if (xSemaphoreTake(csvMutex, portMAX_DELAY) == pdTRUE) {
    
    // حفظ شناسه یکپارچه (ID) برای کلاستربندی تمام فازها
    if (ch[c].name == "" || ch[c].name.startsWith("#CH") || ch[c].name == "No-Tag-Selected") {
      
      bool isManual = (ch[c].autoMode == AUTO_OFF);
      bool isAutoTriage = (ch[c].autoMode == AUTO_TRIAGE);

      if (isManual) {
        // تست‌های دستی در لاگ نیز با MAN ثبت می‌شوند تا به دیتابیس کلاسترینگ نروند
        uint16_t mId = prefs.getUInt("man_id", 1);
        char buf[16];
        snprintf(buf, sizeof(buf), "MAN-%03u", mId);
        ch[c].name = String(buf);
        prefs.putUInt("man_id", mId + 1);
      } else if (isAutoTriage) {
        ch[c].name = getNextGlobalAutoName();
        incrementGlobalId();
      } else {
        ch[c].name = "No-Tag-Selected"; 
      }
    }

    String filename = "/global_log.csv";
    bool exists = LittleFS.exists(filename);
    File f = LittleFS.open(filename, FILE_APPEND);
    
    if (f) {
      if (!exists) {
        f.print("Channel,Battery ID,Start Time,End Time,Duration,Test Mode,Capacity (mAh),IR (mOhm),Triage Grade,DCIR (mOhm),Recovery (mV/s),V_Start,V_End");
        for (int i = 1; i <= 240; i++) {
          f.print(",Pt" + String(i));
        }
        f.println(); 
      }

      unsigned long elapsed = ch[c].endMillis - ch[c].sessionStartMillis; 
      
      // سیستم تگ‌گذاری هوشمند فازها
      String actStr = "Unknown";
      
      if (ch[c].autoMode != AUTO_OFF) {
        String baseMode = "";
        if(ch[c].autoMode == AUTO_500) baseMode = "Auto 0.5A";
        else if(ch[c].autoMode == AUTO_1000) baseMode = "Auto 1.0A";
        else if(ch[c].autoMode == AUTO_1500) baseMode = "Auto 1.5A";
        else if(ch[c].autoMode == AUTO_TRIAGE) baseMode = "Auto Triage";

        String phaseStr = "";
        if (ch[c].autoPhase == PHASE_INIT_CHG) phaseStr = "[CHG_INIT]";
        else if (ch[c].autoPhase == PHASE_REST) phaseStr = "[REST]";
        else if (ch[c].autoPhase == PHASE_DISCHG) phaseStr = "[DISCHARGE]";
        else if (ch[c].autoPhase == PHASE_REST_POST) phaseStr = "[REST_POST]";
        else if (ch[c].autoPhase == PHASE_FINAL_CHG) phaseStr = "[CHG_FINAL]";
        
        actStr = baseMode + " " + phaseStr;
      } else {
        if(ch[c].state == CHARGING) actStr = "Manual Charge";
        else if(ch[c].state == DISCHARGE_500) actStr = "Manual Dischg 0.5A";
        else if(ch[c].state == DISCHARGE_1000) actStr = "Manual Dischg 1.0A";
        else if(ch[c].state == DISCHARGE_1500) actStr = "Manual Dischg 1.5A";
        else if(ch[c].state == Triage) actStr = "Manual Triage";
        else if(ch[c].state == IDLE) actStr = "Idle Phase";
      }

      if (ch[c].isCanceled) {
          actStr = "Canceled (" + actStr + ")";
      } else if (ch[c].hasFault) {
          actStr = "Fault/Error (" + actStr + ")";
      }

      // ظرفیت ثبت شده در همین فاز اختصاصی
      float finalCapacity = ch[c].mAh;

      String grade = (ch[c].qt_grade != "" && ch[c].qt_grade != "Testing...") ? ch[c].qt_grade : "N/A";
      String dcirStr = ch[c].qt_dcir > 0 ? String(ch[c].qt_dcir, 1) : "N/A";
      String recStr = ch[c].qt_recoveryRate > 0 ? String(ch[c].qt_recoveryRate, 1) : "N/A";

      f.printf("CH%d,%s,%s,%s,%s,%s,%.1f,%.1f,%s,%s,%s,%.3f,%.3f",
        c + 1,
        ch[c].name.c_str(),
        timeStr(ch[c].sessionStartTime).c_str(),
        timeStr(ch[c].endTime).c_str(),
        durationStr(elapsed).c_str(),
        actStr.c_str(),
        finalCapacity,
        ch[c].IR,
        grade.c_str(),
        dcirStr.c_str(),
        recStr.c_str(),
        ch[c].sessionVoltStart,
        ch[c].voltNow
      );
      
      for(int i = 0; i < ch[c].graphCount; i++) {
        f.printf(",%.3f", ch[c].graphV[i]);
      }
      
      f.println();
      f.close();
    }
    
    xSemaphoreGive(csvMutex);
  }
}

#endif