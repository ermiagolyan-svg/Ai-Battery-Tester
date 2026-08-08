#ifndef CHANNEL_CONTROL_H
#define CHANNEL_CONTROL_H

#include <Arduino.h>

// ─── متغیرهای شارژ تکمیلی (Top-Up) ───────────────────────
static uint8_t topUpRetries[3] = {0, 0, 0};

// ─── توابع مدیریت گراف هوشمند ────────────────────────────
void graphReset(uint8_t c) {
  ch[c].graphCount = 0;
  ch[c].graphIntervalMs = 500;
  ch[c].graphLastMs = millis();
}

void graphAddPoint(uint8_t c, float v) {
  unsigned long now = millis();
  if (ch[c].graphCount > 0 && (now - ch[c].graphLastMs) < ch[c].graphIntervalMs) return;
  ch[c].graphLastMs = now;
  
  if (ch[c].graphCount >= 240) {
    for (int i = 0; i < 120; i++) {
      ch[c].graphV[i] = (ch[c].graphV[i * 2] + ch[c].graphV[(i * 2) + 1]) / 2.0f;
    }
    ch[c].graphCount = 120;
    ch[c].graphIntervalMs *= 2; 
  }
  ch[c].graphV[ch[c].graphCount++] = v;
}

// ─── توابع کنترل سخت‌افزار و ماشین حالت ──────────────────
void setHwState(uint8_t c, ChannelState st) {
  digitalWrite(hwPins[c].pin_d500, LOW);
  digitalWrite(hwPins[c].pin_d1000, LOW);
  digitalWrite(hwPins[c].pin_ch_en, LOW);
  
  ch[c].state = st;

  if (st == CHARGING) {
    digitalWrite(hwPins[c].pin_ch_en, HIGH);
  } else if (st == DISCHARGE_500) { 
    digitalWrite(hwPins[c].pin_d500, HIGH);
  } else if (st == DISCHARGE_1000) {
    digitalWrite(hwPins[c].pin_d1000, HIGH);
  } else if (st == DISCHARGE_1500) {
    digitalWrite(hwPins[c].pin_d500, HIGH);
    digitalWrite(hwPins[c].pin_d1000, HIGH); 
  }
}

void measureIR(uint8_t c) {
  setHwState(c, IDLE);
  delay(400); 
  ch[c].voltStart = readVoltage(c);

  digitalWrite(hwPins[c].pin_d1000, HIGH); 
  delay(150); 
  ch[c].voltStartLoad = readVoltage(c);
  float iLoad = readCurrent(c); 
  digitalWrite(hwPins[c].pin_d1000, LOW);  

  if (iLoad > 10.0f) {
    float dV = ch[c].voltStart - ch[c].voltStartLoad;
    float rawIR = (dV / (iLoad / 1000.0f)) * 1000.0f; 
    ch[c].IR = rawIR - WIRE_RESISTANCE_MOHM[c]; 
    if (ch[c].IR < 0) ch[c].IR = 0.0f; 
  } else {
    ch[c].IR = 0.0f;
  }
}

void updateInaAverage() {
  bool anyTriage = false;
  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    if (ch[i].state == Triage) {
      anyTriage = true;
      break;
    }
  }
  if (anyTriage) {
    ina3221.setAverage(1); 
  } else {
    ina3221.setAverage(3); 
  }
}

// برای دسترسی به تابع توقف پیش از تعریف کامل آن
void stopChannel(uint8_t c);

// ─── شروع و توقف کانال‌ها ──────────────────────────────
void startChannel(uint8_t c, const String& name, ChannelState act) {
  topUpRetries[c] = 0; // ریست شمارنده شارژ تکمیلی

  ch[c].name        = name;
  ch[c].mAh         = 0;
  ch[c].startMillis = millis();
  ch[c].startTime   = nowEpoch();
  ch[c].endMillis   = 0;
  ch[c].endTime     = 0;
  ch[c].lastReport_mAh = 0.0f;

  ch[c].qt_grade = "";
  ch[c].qt_dcir = 0.0f;
  ch[c].qt_thermalStatus = "";
  ch[c].qt_recoveryRate = 0.0f;
  ch[c].qt_voltDrop500 = 0;
  ch[c].qt_voltRec500 = 0;
  ch[c].qt_voltDrop1000 = 0;
  ch[c].qt_voltRec1000 = 0;
  ch[c].qt_voltDrop1500 = 0;

  if (act == DISCHARGE_500 || act == DISCHARGE_1000 || act == DISCHARGE_1500) {
    measureIR(c);
  }
  else if (act == Triage) {
    ch[c].voltStart = readVoltage(c);
    ch[c].qt_voltBase = ch[c].voltStart;
    ch[c].IR = 0; 
    
    ch[c].qtPhase = QT_REST_INIT;
    ch[c].qtPhaseStartMillis = millis();
    
    ch[c].qt_grade = "Testing...";
    ch[c].qt_thermalStatus = "Testing...";
  } 
  else {
    ch[c].voltStart = readVoltage(c);
    if (ch[c].autoPhase != PHASE_FINAL_CHG) {
      ch[c].IR = 0; 
    }
  }

  // مقداردهی و ریست تایمر/گراف فاز بدون توجه به اتوماتیک یا دستی بودن
  ch[c].sessionStartTime = ch[c].startTime;
  ch[c].sessionStartMillis = ch[c].startMillis;
  ch[c].sessionVoltStart = ch[c].voltStart;
  graphReset(c); 

  // ─── منطق پالس تحریک شارژ (Kickstart) ─────────────────
  if (act == CHARGING) {
    setHwState(c, CHARGING);       
    delay(600);                    
    
    float checkI = readCurrent(c);
    if (checkI < 10.0f) {          
      setHwState(c, IDLE);         
      delay(100);                  
      setHwState(c, DISCHARGE_1000); 
      delay(1000);                 
      setHwState(c, IDLE);         
      delay(100);                  
      setHwState(c, CHARGING);     
    }
  } else {
    setHwState(c, act);            
  }
  
  String actStr = (act == CHARGING) ? "شارژ" : (act == Triage) ? "تریاژ پله‌ای" : "دشارژ";
  String modeType = (ch[c].autoMode != AUTO_OFF) ? "(اتوماتیک)" : "(دستی)";
  String msg = "🚀 عملیات " + actStr + " " + modeType + " کانال " + String(c + 1) + " آغاز شد!\n\n";
  msg += "🔋 نام: " + (name.length() > 0 ? name : "نامشخص") + "\n";
  
  if (act == DISCHARGE_500 || act == DISCHARGE_1000 || act == DISCHARGE_1500) {
      msg += "🎯 مقاومت داخلی: " + String(ch[c].IR, 1) + " mOhm\n";
  } else if(ch[c].autoPhase == PHASE_FINAL_CHG) {
      msg += "🎯 مقاومت داخلی (ثبت شده): " + String(ch[c].IR, 1) + " mOhm\n";
  }
  
  msg += "⚡ ولتاژ شروع: " + String(ch[c].voltStart, 2) + " V";
  
  sendBaleNotification(msg); 
  updateInaAverage();
}

void startAuto(uint8_t c, const String& name, AutoMode mode) {
  ch[c].recordedCapacity = 0.0f;
  
  float v = readVoltage(c);
  if (v < 4.05f) {
    ch[c].autoMode = mode;
    ch[c].autoPhase = PHASE_INIT_CHG;
    startChannel(c, name, CHARGING);
    
    String modeName = (mode == AUTO_TRIAGE) ? "تریاژ اتوماتیک" : "اتوماتیک";
    sendBaleNotification("🤖 حالت " + modeName + " کانال " + String(c+1) + " فعال شد.\nولتاژ کمتر از 4.05V است، ابتدا شارژ اولیه انجام می‌شود.");
  } else {
    ch[c].autoMode = mode;
    ch[c].autoPhase = PHASE_DISCHG;
    
    ChannelState nState;
    if (mode == AUTO_TRIAGE) nState = Triage;
    else if (mode == AUTO_500) nState = DISCHARGE_500;
    else if (mode == AUTO_1000) nState = DISCHARGE_1000;
    else nState = DISCHARGE_1500;
    
    startChannel(c, name, nState);
    
    String modeName = (mode == AUTO_TRIAGE) ? "تریاژ اتوماتیک" : "اتوماتیک";
    String actionName = (mode == AUTO_TRIAGE) ? "تریاژ هوشمند پله‌ای" : "تست دشارژ و سنجش IR";
    sendBaleNotification("🤖 حالت " + modeName + " کانال " + String(c+1) + " فعال شد.\nباتری پر است، مستقیماً وارد " + actionName + " شدیم.");
  }
}

void stopChannel(uint8_t c) {
  digitalWrite(hwPins[c].pin_d500, LOW);
  digitalWrite(hwPins[c].pin_d1000, LOW);
  digitalWrite(hwPins[c].pin_ch_en, LOW);

  ChannelState lastState = ch[c].state;
  setHwState(c, DONE);
  
  ch[c].endMillis  = millis();
  ch[c].endTime    = nowEpoch();
  ch[c].state = lastState; 
  
  // ذخیره بدون قید و شرط CSV در پایان هر فاز
  saveCSV(c); 
  
  ch[c].state = DONE;

  bool isFinalStop = (ch[c].autoMode == AUTO_OFF) || (ch[c].autoPhase == PHASE_FINAL_CHG) || (ch[c].hasFault) || (ch[c].isCanceled) || (lastState == Triage);
  
  String actStr = (lastState == CHARGING) ? "شارژ" : (lastState == Triage) ? "تریاژ پله‌ای" : "دشارژ";
  String modeType = (ch[c].autoMode != AUTO_OFF) ? "(اتوماتیک)" : "(دستی)";
  
  String msg;
  if (isFinalStop) {
      msg = "✅ عملیات نهایی " + modeType + " کانال " + String(c + 1) + " به پایان رسید!\n\n";
  } else {
      msg = "✅ فاز " + actStr + " " + modeType + " کانال " + String(c + 1) + " موقتاً تکمیل شد.\n\n";
  }
  
  msg += "🔋 نام: " + (ch[c].name.length() > 0 ? ch[c].name : "وارد نشده") + "\n";
  
  float dispMah = (ch[c].autoMode != AUTO_OFF && ch[c].autoPhase == PHASE_FINAL_CHG) ? ch[c].recordedCapacity : ch[c].mAh;
  msg += "⚡ ظرفیت: " + String(dispMah, 0) + " mAh\n";
  
  if(lastState != CHARGING || ch[c].autoPhase == PHASE_FINAL_CHG) {
    msg += "🎯 مقاومت داخلی: " + String(ch[c].IR, 1) + " mOhm\n";
  }
  
  if (lastState == Triage) {
    msg += "🥇 نمره تریاژ: " + ch[c].qt_grade + "\n";
    msg += "📉 افت ولتاژ پله 1.5A: " + String(ch[c].qt_voltDrop1500, 2) + " V\n";
    msg += "⚡ مقاومت داینامیک (DCIR): " + String(ch[c].qt_dcir, 1) + " mOhm\n";
    msg += "⏱️ سرعت ریکاوری: " + String(ch[c].qt_recoveryRate, 1) + " mV/s\n";
    msg += "🌡️ پیش‌بینی حرارتی: " + ch[c].qt_thermalStatus + "\n";
  }

  msg += "📉 ولتاژ: " + String(ch[c].voltNow, 2) + " V\n\n";
  
  if (isFinalStop && ch[c].graphCount > 0) {
    String chartUrl = "https://quickchart.io/chart?c=%7Btype:%27line%27,data:%7Blabels:%5B";
    int step = max(1, ch[c].graphCount / 20); 
    bool first = true;
    for(int i = 0; i < ch[c].graphCount; i += step) {
      if(!first) chartUrl += ",";
      chartUrl += String(i);
      first = false;
    }
    
    chartUrl += "%5D,datasets:%5B%7Blabel:%27V%27,fill:false,borderColor:%27green%27,data:%5B";
    
    first = true;
    for(int i = 0; i < ch[c].graphCount; i += step) {
      if(!first) chartUrl += ",";
      chartUrl += String(ch[c].graphV[i], 2);
      first = false;
    }
    chartUrl += "%5D%7D%5D%7D%7D";
    msg += "📊 تصویر نمودار فاز نهایی:\n" + chartUrl;
  } else if (!isFinalStop) {
    msg += "⏳ در حال ورود به فاز استراحت/بعدی...";
  } else {
    msg += "📊 دیتایی برای رسم نمودار ثبت نشده است.";
  }
  
  sendBaleNotification(msg);

  if (lastState == Triage) {
    ch[c].autoMode = AUTO_OFF;
    ch[c].autoPhase = PHASE_NONE;
  }
  updateInaAverage();
}

// ─── تابع مستقل مدیریت شارژ و شارژ تکمیلی (Top-Up) ────────
void handleCharging(uint8_t c) {
  if (ch[c].currentNow < 50.0f && ch[c].voltNow >= 4.05f && (millis() - ch[c].startMillis > 10000)) {
    
    // منطق شارژ تکمیلی برای عبور از افت ولتاژ مسیر
    if (ch[c].voltNow < 4.15f && topUpRetries[c] < 3) {
      topUpRetries[c]++;
      String msg = "🔄 شارژ تکمیلی کانال " + String(c+1) + " (تلاش " + String(topUpRetries[c]) + " از 3)\n";
      msg += "ولتاژ " + String(ch[c].voltNow, 2) + "V است. اعمال پالس تحریک برای شکستن ولتاژ کاذب...";
      sendBaleNotification(msg);

      setHwState(c, IDLE);
      delay(200);
      setHwState(c, DISCHARGE_1000); 
      delay(1000);
      setHwState(c, IDLE);
      delay(200);
      setHwState(c, CHARGING);       
      
      ch[c].startMillis = millis();  
      return;                        
    }
    
    topUpRetries[c] = 0;

    if (ch[c].autoMode != AUTO_OFF && ch[c].autoPhase == PHASE_INIT_CHG) {
      AutoMode am = ch[c].autoMode; 
      String name = ch[c].name;
      unsigned long chargeDuration = millis() - ch[c].startMillis;
      
      stopChannel(c); // ثبت لاگِ فاز شارژ اولیه
      
      if (chargeDuration < 60000) {
        ch[c].autoMode = am;
        ch[c].autoPhase = PHASE_DISCHG;
        ChannelState nState;
        if (am == AUTO_TRIAGE) nState = Triage;
        else if (am == AUTO_500) nState = DISCHARGE_500;
        else if (am == AUTO_1000) nState = DISCHARGE_1000;
        else nState = DISCHARGE_1500;
        
        startChannel(c, name, nState);
        sendBaleNotification("⚡ باتری کانال " + String(c+1) + " از قبل پر بود. بدون استراحت وارد فاز تست شدیم.");
      } else {
        ch[c].autoMode = am;
        ch[c].autoPhase = PHASE_REST;
        ch[c].state = IDLE; 
        ch[c].restStartMillis = millis(); 
        ch[c].name = name; 
        
        // آماده‌سازی گراف و تایمرها برای فاز استراحت
        ch[c].sessionStartMillis = millis();
        ch[c].sessionStartTime = nowEpoch();
        ch[c].sessionVoltStart = ch[c].voltNow;
        graphReset(c);

        sendBaleNotification("☕ شارژ اولیه کانال " + String(c+1) + " کامل شد.\nآغاز فاز استراحت برای تثبیت ولتاژ...");
      }
    } 
    else if (ch[c].autoMode != AUTO_OFF && ch[c].autoPhase == PHASE_FINAL_CHG) {
      float finalCapacity = ch[c].recordedCapacity; 
      stopChannel(c); // ثبت لاگِ فاز شارژ نهایی
      ch[c].recordedCapacity = finalCapacity;
      ch[c].autoMode = AUTO_OFF;
      ch[c].autoPhase = PHASE_NONE;
      sendBaleNotification("🎉 تست اتوماتیک کانال " + String(c+1) + " به پایان رسید!\nباتری فول شارژ و آماده است.\nظرفیت نهایی: " + String(finalCapacity, 0) + " mAh");
    } else {
      stopChannel(c); 
    }
  }
}

// ─── منطق اصلی نمونه‌برداری و لایه ایمنی ─────────────────
void sampleAll(unsigned long dt) {
  for (uint8_t c = 0; c < NUM_CHANNELS; c++) {
    float rawVoltage = readVoltage(c);
    ch[c].currentNow  = readCurrent(c);

    float wireVoltageDrop = (ch[c].currentNow / 1000.0f) * (WIRE_RESISTANCE_MOHM[c] / 1000.0f);
    
    if(ch[c].state == DISCHARGE_500 || ch[c].state == DISCHARGE_1000 || ch[c].state == DISCHARGE_1500 || ch[c].state == Triage) {
      ch[c].voltNow = rawVoltage + wireVoltageDrop;
    } 
    else if (ch[c].state == CHARGING) {
      ch[c].voltNow = rawVoltage - wireVoltageDrop;
    } 
    else {
      ch[c].voltNow = rawVoltage;
    }

    if (ch[c].state == DONE) continue;
    if (ch[c].state == IDLE && ch[c].autoPhase != PHASE_REST && ch[c].autoPhase != PHASE_REST_POST) continue;

    if (ch[c].state != IDLE) {
      ch[c].mAh += (ch[c].currentNow * dt) / 3600000.0f;
    }
    
    if ((ch[c].state >= CHARGING && ch[c].state <= Triage) || ch[c].autoPhase == PHASE_REST || ch[c].autoPhase == PHASE_REST_POST) {
      graphAddPoint(c, ch[c].voltNow);
    }

    // لایه ایمنی ناظر
    if (ch[c].state == CHARGING || ch[c].state == DISCHARGE_500 || ch[c].state == DISCHARGE_1000 || ch[c].state == DISCHARGE_1500 || ch[c].state == Triage) {
      bool triggered = false;
      String reason = "";

      if (millis() - ch[c].startMillis > 36000000UL) {
        triggered = true; reason = "⏳ تایم‌اوت امنیتی (بیش از ۱۰ ساعت کار مداوم)";
      }
      else if (ch[c].mAh > 4000.0f) {
        triggered = true; reason = "🔋 تجاوز از سقف ظرفیت (بیش از 4000mAh) - احتمال خرابی شیمیایی";
      }
      else if (ch[c].state == CHARGING && ch[c].voltNow > 4.30f) {
        triggered = true; reason = "🔥 ولتاژ بحرانی شارژ (> 4.30V) - خطر انفجار!";
      }
      else if ((ch[c].state == DISCHARGE_500 || ch[c].state == DISCHARGE_1000 || ch[c].state == DISCHARGE_1500 || ch[c].state == Triage) && ch[c].voltNow < 2.00f) {
        triggered = true; reason = "⚠️ افت شدید ولتاژ دشارژ (< 2.00V) - خطر مرگ سلول!";
      }
      else if (ch[c].state == CHARGING && (millis() - ch[c].startMillis > 10000) && ch[c].currentNow < 10.0f && ch[c].voltNow < 4.05f) {
        triggered = true; reason = "🔌 خطای اتصال: جریان شارژ برقرار نشد!";
      }

      if (triggered) {
        ch[c].hasFault = true;
        ch[c].faultMsg = reason;
        ch[c].autoMode = AUTO_OFF;
        ch[c].autoPhase = PHASE_NONE;
        
        stopChannel(c);
        
        String alertMsg = "🚨🚨 هشدار امنیتی اضطراری (Failsafe) 🚨🚨\n\n";
        alertMsg += "کانال: " + String(c + 1) + "\n";
        alertMsg += "باتری: " + (ch[c].name.length() > 0 ? ch[c].name : "نامشخص") + "\n";
        alertMsg += "علت توقف: " + reason + "\n";
        alertMsg += "ولتاژ در لحظه توقف: " + String(ch[c].voltNow, 2) + " V\n";
        alertMsg += "ظرفیت تا لحظه توقف: " + String(ch[c].mAh, 0) + " mAh\n\n";
        alertMsg += "سخت افزار بلافاصله برای جلوگیری از آسیب قطع شد!";
        sendBaleNotification(alertMsg);
        continue; 
      }
    }

    // مدیریت تایمرهای استراحت
    if (ch[c].autoPhase == PHASE_REST || ch[c].autoPhase == PHASE_REST_POST) {
      unsigned long restDuration;
      if (ch[c].autoPhase == PHASE_REST) {
          restDuration = (ch[c].autoMode == AUTO_TRIAGE) ? 45000UL : 300000UL;
      } else {
          restDuration = 60000UL; 
      }
      
      if (millis() - ch[c].restStartMillis >= restDuration) {
        AutoMode am = ch[c].autoMode;
        String name = ch[c].name;
        float rc = ch[c].recordedCapacity; 
        
        // ثبت لاگِ فاز استراحت پیش از تغییر فاز
        ch[c].endMillis = millis();
        ch[c].endTime = nowEpoch();
        saveCSV(c);

        if (ch[c].autoPhase == PHASE_REST) {
            ChannelState nState;
            if (am == AUTO_TRIAGE) nState = Triage;
            else if (am == AUTO_500) nState = DISCHARGE_500;
            else if (am == AUTO_1000) nState = DISCHARGE_1000;
            else nState = DISCHARGE_1500;
            
            ch[c].autoMode = am;
            ch[c].autoPhase = PHASE_DISCHG;
            ch[c].recordedCapacity = rc;
            startChannel(c, name, nState);
            
            if (am == AUTO_TRIAGE) {
                sendBaleNotification("⏳ استراحت ۴۵ ثانیه‌ای کانال " + String(c+1) + " به پایان رسید.\nشروع تست پله‌ای هوشمند (Triage)...");
            } else {
                sendBaleNotification("⏳ استراحت ۵ دقیقه‌ای کانال " + String(c+1) + " به پایان رسید.\nشروع تست دشارژ و محاسبه IR...");
            }
        } 
        else if (ch[c].autoPhase == PHASE_REST_POST) {
            ch[c].autoMode = am;
            ch[c].autoPhase = PHASE_FINAL_CHG;
            ch[c].recordedCapacity = rc;
            startChannel(c, name, CHARGING);
            sendBaleNotification("⏳ استراحت ۱ دقیقه‌ای کانال " + String(c+1) + " به پایان رسید.\nشروع شارژ نهایی...");
        }
      }
      continue; 
    }

    if ((ch[c].state == DISCHARGE_500 || ch[c].state == DISCHARGE_1000 || ch[c].state == DISCHARGE_1500) && (ch[c].mAh - ch[c].lastReport_mAh >= 500.0f)) {
      ch[c].lastReport_mAh = floor(ch[c].mAh / 500.0f) * 500.0f; 
      String midMsg = "⏳ گزارش میان‌دوره‌ای دشارژ کانال " + String(c + 1) + "\n\n";
      midMsg += "🔋 نام: " + (ch[c].name.length() > 0 ? ch[c].name : "وارد نشده") + "\n";
      midMsg += "⚡ ظرفیت تخلیه شده تا این لحظه: " + String(ch[c].mAh, 0) + " mAh\n";
      midMsg += "📉 ولتاژ فعلی باتری: " + String(ch[c].voltNow, 2) + " V";
      sendBaleNotification(midMsg);
    }

    if (ch[c].state == DISCHARGE_500 || ch[c].state == DISCHARGE_1000 || ch[c].state == DISCHARGE_1500) {
      if (ch[c].voltNow <= CUTOFF_DISCHARGE) {
        if (ch[c].autoMode != AUTO_OFF && ch[c].autoPhase == PHASE_DISCHG) {
          ch[c].recordedCapacity = ch[c].mAh;
          stopChannel(c); // ثبت لاگِ فاز دشارژ
          
          AutoMode am = ch[c].autoMode;
          float rc = ch[c].recordedCapacity;
          String name = ch[c].name;
          
          ch[c].autoMode = am;
          ch[c].autoPhase = PHASE_REST_POST; 
          ch[c].recordedCapacity = rc;
          ch[c].state = IDLE;
          ch[c].restStartMillis = millis();
          ch[c].name = name;
          
          // آماده‌سازی گراف و تایمرها برای فاز استراحت دوم
          ch[c].sessionStartMillis = millis();
          ch[c].sessionStartTime = nowEpoch();
          ch[c].sessionVoltStart = ch[c].voltNow;
          graphReset(c);

          sendBaleNotification("📉 دشارژ کانال " + String(c+1) + " کامل شد.\nآغاز فاز استراحت ۱ دقیقه‌ای پیش از شارژ نهایی...");
        } else {
          stopChannel(c);
        }
      }
    } 
    else if (ch[c].state == CHARGING) {
      handleCharging(c);
    }
  }
}

#endif