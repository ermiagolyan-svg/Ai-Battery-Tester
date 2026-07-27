#ifndef DISPLAY_TFT_H
#define DISPLAY_TFT_H

#include <Arduino.h>

// دسترسی به آرایه اضافه شده در بک‌اند
extern int topUpRetries[3];

// ─── تابع آپدیت TFT با فرمت و پدینگ‌های درخواستی شما ───
void updateTFT() {
  static bool firstDraw = true;
  uint16_t COLOR_BG = TFT_BLACK; 
  
  // ════════ رسم هدر (فقط بار اول) ════════
  if (firstDraw) {
    tft.fillScreen(COLOR_BG);
    tft.fillRect(0, 0, 240, 20, TFT_NAVY);
    
    tft.setTextFont(2); 
    tft.setTextSize(1); 
    tft.setTextColor(TFT_CYAN, TFT_NAVY);
    tft.drawString("Li-ion Analyzer", 5, 2);
    
    tft.setTextColor(TFT_WHITE, TFT_NAVY);
    String ipStr = "IP: ";
    if (WiFi.getMode() == WIFI_AP) ipStr += WiFi.softAPIP().toString();
    else ipStr += WiFi.localIP().toString();
    tft.drawRightString(ipStr, 235, 2, 2); 
    
    firstDraw = false;
  }

  // ════════ آپدیت دیتای کانال‌ها با استفاده از Sprite ════════
  for(int i = 0; i < 3; i++) {
    int yPos = 22 + (i * 98); // موقعیت Y کارت روی نمایشگر اصلی

    spr.fillSprite(COLOR_BG); // رنگ پس‌زمینه کارت مجازی
    
    uint16_t stateColor = TFT_LIGHTGREY;
    String stateStr = "STBY"; 
    
    // --- اضافه شدن حالت استراحت داینامیک برای نمایش متنی ---
    if (ch[i].state == IDLE && (ch[i].autoPhase == PHASE_REST || ch[i].autoPhase == PHASE_REST_POST)) { 
        stateColor = TFT_SKYBLUE; 
        if (ch[i].autoPhase == PHASE_REST) {
            stateStr = (ch[i].autoMode == AUTO_TRIAGE) ? "REST45s" : "REST5";
        } else {
            stateStr = "REST1";
        }
    }
    else if(ch[i].state == MEASURING_IR) { stateColor = TFT_YELLOW; stateStr = "IR.."; }
    else if(ch[i].state == CHARGING) { 
        stateColor = TFT_CYAN; 
        // تغییرات منطق شارژ تکمیلی
        if(topUpRetries[i] > 0) {
            stateStr = "TOP-UP";
        } else {
            stateStr = "CHRG"; 
        }
    }
    else if(ch[i].state == DISCHARGE_500) { stateColor = TFT_ORANGE; stateStr = "D.5A"; } 
    else if(ch[i].state == DISCHARGE_1000) { stateColor = TFT_RED; stateStr = "D1A"; }
    else if(ch[i].state == DISCHARGE_1500) { stateColor = TFT_MAROON; stateStr = "D1.5A"; }
    else if(ch[i].state == Triage) { stateColor = TFT_MAGENTA; stateStr = "QUICK"; }
    else if(ch[i].state == DONE) { stateColor = TFT_GREEN; stateStr = "DONE"; }

    char modeChar = (ch[i].autoMode != AUTO_OFF) ? 'A' : 'M';
    if((ch[i].state == IDLE && ch[i].autoPhase != PHASE_REST && ch[i].autoPhase != PHASE_REST_POST) || ch[i].state == DONE) modeChar = '-';
    char buffer[32]; 

    // رسم کادر دور کارت
    spr.drawRoundRect(0, 0, 236, 95, 6, stateColor);

    // --- سطر اول: وضعیت و زمان ---
    spr.setTextFont(2); 
    spr.setTextSize(1); 
    spr.setTextColor(stateColor, COLOR_BG);
    
    sprintf(buffer, "CH%d-%c %s", i+1, modeChar, stateStr.c_str());
    spr.drawString(buffer, 6, 4); 
    
    spr.setTextColor(TFT_SILVER, COLOR_BG);
    
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
      spr.drawRightString(restTimeBuf, 230, 4, 2);
    }
    else if(ch[i].state != IDLE && ch[i].state != DONE) {
      unsigned long elapsed = millis() - ch[i].startMillis;
      savedTime[i] = elapsed; 
      spr.drawRightString(durationStr(elapsed), 230, 4, 2);
    } 
    else if (ch[i].state == DONE) {
      spr.drawRightString(durationStr(savedTime[i]), 230, 4, 2); 
    } 
    else {
      spr.drawRightString("--:--", 230, 4, 2); 
    }
    
    // ════════ تغییرات هوشمند از اینجا شروع می‌شود ════════
    // بررسی می‌کنیم آیا کانال در حالت تریاژ است یا آخرین عملیاتش تریاژ بوده؟
    bool isTriageDisplay = (ch[i].state == Triage) || (ch[i].state == DONE && ch[i].qt_grade != "" && ch[i].mAh < 20.0f);

    // --- سطر دوم: ولتاژ و (ظرفیت یا نمره تریاژ) ---
    spr.setTextFont(4); 
    spr.setTextColor(TFT_WHITE, COLOR_BG);
    sprintf(buffer, "%.3fV", ch[i].voltNow);
    spr.drawString(buffer, 6, 22);

    spr.setTextColor(TFT_CYAN, COLOR_BG);
    if (isTriageDisplay) {
        String shortGrade = ch[i].qt_grade;
        if (shortGrade == "Testing...") shortGrade = "Testing...";
        else if (shortGrade.length() > 0) shortGrade = "Gr: " + shortGrade.substring(0, 1); // استخراج فقط حرف اول مثل A یا B
        sprintf(buffer, "%s", shortGrade.c_str());
        spr.drawRightString(buffer, 230, 22, 4);
    } else {
        float displayMah = (ch[i].autoPhase == PHASE_FINAL_CHG) ? ch[i].recordedCapacity : ch[i].mAh;
        sprintf(buffer, "%4.0fmAh", displayMah);
        spr.drawRightString(buffer, 230, 22, 4);
    }

    // --- سطر سوم: (مقاومت سنتی یا DCIR) و (جریان یا ریکاوری) ---
    spr.setTextFont(2); 
    spr.setTextColor(TFT_MAGENTA, COLOR_BG);
    if (isTriageDisplay) {
        if (ch[i].qt_dcir > 0) sprintf(buffer, "DCIR: %3.0fmR", ch[i].qt_dcir);
        else sprintf(buffer, "DCIR: ---");
        spr.drawString(buffer, 6, 46);
    } else {
        sprintf(buffer, "IR: %3.1f mR", ch[i].IR);
        spr.drawString(buffer, 6, 46);
    }

    spr.setTextColor(TFT_YELLOW, COLOR_BG);
    if (isTriageDisplay && ch[i].qt_recoveryRate > 0) {
        // در صورت آماده بودن دیتای ریکاوری، به جای جریان نمایش داده می‌شود
        sprintf(buffer, "Rec: %.1fmV/s", ch[i].qt_recoveryRate);
        spr.drawRightString(buffer, 230, 46, 2);
    } else {
        sprintf(buffer, "%4.0f mA", ch[i].currentNow);
        spr.drawRightString(buffer, 230, 46, 2);
    }

    // ════════ خط چهارم: گراف داخل اسپیرایت ════════
    int gX = 6;
    int gY = 64;
    int gW = 224;
    int gH = 28;

    // ---> تغییر: وضعیت DONE هم به شرط نمایش گراف اضافه شد <---
    if ((ch[i].state >= CHARGING && ch[i].state <= Triage) || (ch[i].state == IDLE && (ch[i].autoPhase == PHASE_REST || ch[i].autoPhase == PHASE_REST_POST)) || ch[i].state == DONE) {
      spr.drawRect(gX, gY, gW, gH, 0x2104); // کادر گراف
      
      // خط‌چین 3.7 ولت
      int y37 = (gY + gH - 1) - ((3.7 - 2.5) / (4.3 - 2.5)) * (gH - 2);
      for (int lx = gX + 2; lx < (gX + gW - 2); lx += 4) {
        spr.drawPixel(lx, y37, TFT_DARKGREY); 
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

    // انتقال اسپیرایت به نمایشگر اصلی
    spr.pushSprite(2, yPos);
  }
}

#endif