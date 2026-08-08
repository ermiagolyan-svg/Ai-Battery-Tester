#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

#include <Arduino.h>

// دسترسی به آرایه اضافه شده در بک‌اند
extern uint8_t topUpRetries[3];

// ════════ پالت رنگی هماهنگ با Web UI (تبدیل شده به RGB565) ════════
#define UI_BG       0x0861  // #0d0d0f (پس‌زمینه کل)
#define UI_SURFACE  0x10A3  // #161619 (پس‌زمینه کارت‌ها)
#define UI_SURFACE2 0x18C4  // #1e1e23 (پس‌زمینه گراف)
#define UI_BORDER   0x2946  // #2a2a32 (حاشیه‌ها)
#define UI_ACCENT   0x0695  // #00d4aa (سبزآبی/سایان)
#define UI_ACCENT2  0x04BF  // #0096ff (آبی)
#define UI_WARN     0xF346  // #ff6b35 (نارنجی هشدار)
#define UI_OK       0x072E  // #00e676 (سبز تایید)
#define UI_TEXT     0xE73E  // #e8e8f0 (متن اصلی)
#define UI_MUTED    0x6B50  // #6b6b80 (متن کمرنگ)

// ─── تابع آپدیت TFT با فرمت جدید و بهینه‌سازی شده ───
void updateTFT() {
  static bool firstDraw = true;
  
  // ════════ رسم هدر (فقط بار اول) ════════
  if (firstDraw) {
    tft.fillScreen(UI_BG);
    
    // نوار هدر بالا (استایل جدید)
    tft.fillRect(0, 0, 240, 22, UI_SURFACE);
    tft.drawFastHLine(0, 22, 240, UI_BORDER); // خط زیرین هدر
    
    tft.setTextFont(2); 
    tft.setTextSize(1); 
    tft.setTextColor(UI_ACCENT, UI_SURFACE);
    tft.drawString("AI Battery Studio", 5, 2);
    
    tft.setTextColor(UI_MUTED, UI_SURFACE);
    String ipStr = "IP: ";
    if (WiFi.getMode() == WIFI_AP) ipStr += WiFi.softAPIP().toString();
    else ipStr += WiFi.localIP().toString();
    tft.drawRightString(ipStr, 235, 2, 2); 
    
    firstDraw = false;
  }

  // ════════ آپدیت دیتای کانال‌ها با استفاده از Sprite ════════
  for(int i = 0; i < 3; i++) {
    // توزیع یکنواخت کارت‌ها برای پر کردن 320 پیکسل (ارتفاع 96 + فاصله 2)
    int yPos = 25 + (i * 98); 

    // پس‌زمینه بیرونِ کارت
    spr.fillSprite(UI_BG); 
    
    uint16_t stateColor = UI_MUTED;
    String stateStr = "STBY"; 
    
    // تعیین رنگ و وضعیت مشابه وب
    if (ch[i].state == IDLE && (ch[i].autoPhase == PHASE_REST || ch[i].autoPhase == PHASE_REST_POST)) { 
        stateColor = 0x867D; // SkyBlue
        if (ch[i].autoPhase == PHASE_REST) {
            stateStr = (ch[i].autoMode == AUTO_TRIAGE) ? "REST 45s" : "REST 5M";
        } else {
            stateStr = "REST 1M";
        }
    }
    else if(ch[i].state == MEASURING_IR) { stateColor = TFT_YELLOW; stateStr = "MEASURE IR"; }
    else if(ch[i].state == CHARGING) { 
        stateColor = UI_ACCENT; 
        if(topUpRetries[i] > 0) stateStr = "TOP-UP";
        else stateStr = "CHARGING"; 
    }
    else if(ch[i].state == DISCHARGE_500) { stateColor = TFT_YELLOW; stateStr = "TEST 0.5A"; } 
    else if(ch[i].state == DISCHARGE_1000) { stateColor = UI_WARN; stateStr = "TEST 1.0A"; }
    else if(ch[i].state == DISCHARGE_1500) { stateColor = TFT_RED; stateStr = "TEST 1.5A"; }
    else if(ch[i].state == Triage) { stateColor = TFT_MAGENTA; stateStr = "SMART TRIAGE"; }
    else if(ch[i].state == DONE) { stateColor = UI_OK; stateStr = "FINISHED"; }

    char modeChar = (ch[i].autoMode != AUTO_OFF) ? 'A' : 'M';
    if((ch[i].state == IDLE && ch[i].autoPhase != PHASE_REST && ch[i].autoPhase != PHASE_REST_POST) || ch[i].state == DONE) modeChar = '-';
    char buffer[32]; 

    // رسم کادر داخلی کارت (طراحی جدید با ارتفاع 96)
    uint16_t cardBorderColor = (ch[i].state == IDLE && ch[i].autoPhase != PHASE_REST && ch[i].autoPhase != PHASE_REST_POST) ? UI_BORDER : stateColor;
    spr.fillRoundRect(0, 0, 236, 96, 8, UI_SURFACE); 
    spr.drawRoundRect(0, 0, 236, 96, 8, cardBorderColor); 

    // --- سطر اول: وضعیت و زمان ---
    spr.setTextFont(2); 
    spr.setTextSize(1); 
    spr.setTextColor(stateColor, UI_SURFACE);
    
    sprintf(buffer, "CH%d-%c: %s", i+1, modeChar, stateStr.c_str());
    spr.drawString(buffer, 8, 5); 
    
    spr.setTextColor(UI_MUTED, UI_SURFACE);
    static unsigned long savedTime[3] = {0, 0, 0}; 

    if (ch[i].state == IDLE && (ch[i].autoPhase == PHASE_REST || ch[i].autoPhase == PHASE_REST_POST)) {
      unsigned long restDuration;
      if (ch[i].autoPhase == PHASE_REST) {
          restDuration = (ch[i].autoMode == AUTO_TRIAGE) ? 45000UL : 300000UL;
      } else {
          restDuration = 60000UL;
      }
      
      unsigned long elapsedRest = millis() - ch[i].restStartMillis;
      unsigned long remainMs = (elapsedRest < restDuration) ? (restDuration - elapsedRest) : 0;
      unsigned long remainS = remainMs / 1000;
      char restTimeBuf[16];
      snprintf(restTimeBuf, sizeof(restTimeBuf), "-%02lu:%02lu", remainS / 60, remainS % 60);
      spr.setTextColor(0x867D, UI_SURFACE); // رنگ آبی برای استراحت
      spr.drawRightString(restTimeBuf, 228, 5, 2); 
    }
    else if(ch[i].state != IDLE && ch[i].state != DONE) {
      unsigned long elapsed = millis() - ch[i].startMillis;
      savedTime[i] = elapsed; 
      spr.drawRightString(durationStr(elapsed), 228, 5, 2);
    } 
    else if (ch[i].state == DONE) {
      spr.drawRightString(durationStr(savedTime[i]), 228, 5, 2); 
    } 
    else {
      spr.drawRightString("--:--", 228, 5, 2); 
    }
    
    // رسم خط جداکننده زیر هدر کارت (تنظیم فاصله جدید)
    spr.drawFastHLine(0, 24, 236, UI_BORDER);

    // بررسی تریاژ
    bool isTriageDisplay = (ch[i].state == Triage) || (ch[i].state == DONE && ch[i].qt_grade != "" && ch[i].mAh < 20.0f);

    // --- سطر دوم: ولتاژ و ظرفیت/نمره (فاصله‌ها افزایش یافت) ---
    spr.setTextFont(4); 
    spr.setTextColor(UI_TEXT, UI_SURFACE);
    sprintf(buffer, "%.3fV", ch[i].voltNow);
    spr.drawString(buffer, 8, 28);

    spr.setTextColor(UI_ACCENT, UI_SURFACE);
    if (isTriageDisplay) {
        String shortGrade = ch[i].qt_grade;
        if (shortGrade == "Testing...") shortGrade = "Test..";
        else if (shortGrade.length() > 0) shortGrade = "Gr: " + shortGrade.substring(0, 1);
        sprintf(buffer, "%s", shortGrade.c_str());
        spr.drawRightString(buffer, 228, 28, 4);
    } else {
        float displayMah = (ch[i].autoPhase == PHASE_FINAL_CHG) ? ch[i].recordedCapacity : ch[i].mAh;
        sprintf(buffer, "%4.0fmAh", displayMah);
        spr.drawRightString(buffer, 228, 28, 4);
    }

    // --- سطر سوم: مقاومت و جریان/ریکاوری (فاصله‌ها افزایش یافت) ---
    spr.setTextFont(2); 
    spr.setTextColor(TFT_MAGENTA, UI_SURFACE);
    if (isTriageDisplay) {
        if (ch[i].qt_dcir > 0) sprintf(buffer, "DCIR:%3.0fmR", ch[i].qt_dcir);
        else sprintf(buffer, "DCIR: ---");
        spr.drawString(buffer, 8, 54);
    } else {
        sprintf(buffer, "IR:%3.1f mR", ch[i].IR);
        spr.drawString(buffer, 8, 54);
    }

    spr.setTextColor(TFT_YELLOW, UI_SURFACE);
    if (isTriageDisplay && ch[i].qt_recoveryRate > 0) {
        sprintf(buffer, "Rec:%.1fmV/s", ch[i].qt_recoveryRate);
        spr.drawRightString(buffer, 228, 54, 2);
    } else {
        sprintf(buffer, "%4.0f mA", ch[i].currentNow);
        spr.drawRightString(buffer, 228, 54, 2);
    }

    // ════════ خط چهارم: گراف ════════
    int gX = 8;
    int gY = 74;
    int gW = 220;
    int gH = 17; // ارتفاع متناسب با فضای جدید

    if ((ch[i].state >= CHARGING && ch[i].state <= Triage) || (ch[i].state == IDLE && (ch[i].autoPhase == PHASE_REST || ch[i].autoPhase == PHASE_REST_POST)) || ch[i].state == DONE) {
      
      spr.fillRoundRect(gX, gY, gW, gH, 4, UI_SURFACE2); // پس‌زمینه تیره برای گراف
      
      // خط‌چین 3.7 ولت
      int y37 = (gY + gH - 1) - ((3.7 - 2.5) / (4.3 - 2.5)) * (gH - 2);
      for (int lx = gX + 2; lx < (gX + gW - 2); lx += 5) {
        spr.drawPixel(lx, y37, UI_MUTED); 
      }
      
      int numPoints = ch[i].graphCount;
      float minV = 2.5; float maxV = 4.3;
      
      if (numPoints > 0) {
        int prevX, prevY;
        for (int p = 0; p < numPoints; p++) {
          int x = gX + (p * gW / max(1, numPoints - 1));
          if (numPoints == 1) x = gX; 
          
          int yVal = (gY + gH - 2) - ((ch[i].graphV[p] - minV) / (maxV - minV)) * (gH - 4);
          yVal = constrain(yVal, gY + 1, gY + gH - 2);
          
          if (p == 0) {
             spr.drawPixel(x, yVal, stateColor); 
          } else {
             spr.drawLine(prevX, prevY, x, yVal, stateColor); 
          }
          prevX = x;
          prevY = yVal;
        }
      }
    }

    // انتقال اسپیرایت به نمایشگر اصلی (با 2 پیکسل حاشیه سمت چپ)
    spr.pushSprite(2, yPos);
  }
}

#endif