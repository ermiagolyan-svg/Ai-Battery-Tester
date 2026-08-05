#ifndef LOGGER_CSV_H
#define LOGGER_CSV_H

#include <Arduino.h>
#include <LittleFS.h>
#include <time.h>

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

// ─── توابع نام‌گذاری خودکار و شمارنده تست‌ها ───────────
uint16_t getNextTestId(uint8_t ch) {
  String counterFile = "/cnt_ch" + String(ch + 1) + ".txt";
  uint16_t id = 1;

  if (LittleFS.exists(counterFile)) {
    File f = LittleFS.open(counterFile, FILE_READ);
    if (f) {
      id = f.readStringUntil('\n').toInt();
      f.close();
      if (id < 1) id = 1;
    }
  }
  return id;
}

void saveNextTestId(uint8_t ch, uint16_t nextId) {
  String counterFile = "/cnt_ch" + String(ch + 1) + ".txt";
  File f = LittleFS.open(counterFile, FILE_WRITE);
  if (f) {
    f.println(nextId);
    f.close();
  }
}

String generateAutoName(uint8_t ch) {
  uint16_t id = getNextTestId(ch);
  char buf[16];
  snprintf(buf, sizeof(buf), "#CH%d-%03u", ch + 1, id);
  return String(buf);
}

// ─── تابع اصلی ذخیره لاگ CSV ──────────────────────────
void saveCSV(uint8_t c) {
  // --- [فیکس باگ CH-004]: اعتبارسنجی اجباری فرمت ID ---
  ch[c].name.trim();
  // فرمت استاندارد باید با #CH شروع شود و کاراکتر پنجم آن خط تیره باشد (مثل #CH1-001)
  if (ch[c].name.length() < 7 || !ch[c].name.startsWith("#CH") || ch[c].name.charAt(4) != '-') {
      Serial.printf("|- WARNING: Invalid Battery ID '%s' on CH%d! Forcing Auto-Name to protect ML Database.\n", ch[c].name.c_str(), c + 1);
      ch[c].name = generateAutoName(c);
  }
  // ----------------------------------------------------------------

  String filename = "/log_ch" + String(c+1) + ".csv";

  bool exists = LittleFS.exists(filename);
  File f = LittleFS.open(filename, FILE_APPEND);
  if (!f) return;

  // بازگرداندن منطق ۲۴۰ ستون گراف برای استفاده در داشبورد Triage Manager
  if (!exists) {
    f.print("Battery ID,Start Time,End Time,Duration,Test Mode,Capacity (mAh),IR (mOhm),Triage Grade,DCIR (mOhm),Recovery (mV/s),V_Start,V_End");
    for (int i = 1; i <= 240; i++) {
      f.print(",Pt" + String(i));
    }
    f.println(); 
  }

  // محاسبه زمان کل بر اساس sessionStartMillis
  unsigned long elapsed = ch[c].endMillis - ch[c].sessionStartMillis; 
  
  // دسته‌بندی اکشن‌ها
  String actStr = "Unknown";
  if(ch[c].state == CHARGING) actStr = "Manual Charge";
  else if(ch[c].state == DISCHARGE_500) actStr = "Manual Dischg 0.5A";
  else if(ch[c].state == DISCHARGE_1000) actStr = "Manual Dischg 1.0A";
  else if(ch[c].state == DISCHARGE_1500) actStr = "Manual Dischg 1.5A";
  else if(ch[c].state == Triage) actStr = "Manual Triage";
  
  if(ch[c].autoMode == AUTO_500) actStr = "Auto Cycle 0.5A";
  else if(ch[c].autoMode == AUTO_1000) actStr = "Auto Cycle 1.0A";
  else if(ch[c].autoMode == AUTO_1500) actStr = "Auto Cycle 1.5A";
  else if(ch[c].autoMode == AUTO_TRIAGE) actStr = "Auto Smart Triage";

   if (ch[c].isCanceled) {
      actStr = "Canceled (" + actStr + ")";
  } else if (ch[c].hasFault) {
      actStr = "Fault/Error (" + actStr + ")";
  }

  float finalCapacity = (ch[c].autoMode != AUTO_OFF) ? ch[c].recordedCapacity : ch[c].mAh;

  // مدیریت خالی بودن متغیرهای تریاژ در تست‌های معمولی
  String grade = (ch[c].qt_grade != "" && ch[c].qt_grade != "Testing...") ? ch[c].qt_grade : "N/A";
  String dcirStr = ch[c].qt_dcir > 0 ? String(ch[c].qt_dcir, 1) : "N/A";
  String recStr = ch[c].qt_recoveryRate > 0 ? String(ch[c].qt_recoveryRate, 1) : "N/A";

  // ذخیره اطلاعات متنی و عددی پایه
  f.printf("%s,%s,%s,%s,%s,%.1f,%.1f,%s,%s,%s,%.3f,%.3f",
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
  
  // ذخیره دیتای گراف در ادامه‌ی همان ردیف
  for(int i = 0; i < ch[c].graphCount; i++) {
    f.printf(",%.3f", ch[c].graphV[i]);
  }
  
  f.println();
  f.close();

  // آپدیت شمارنده
  uint16_t currentId = getNextTestId(c);
  saveNextTestId(c, currentId + 1);
}

#endif