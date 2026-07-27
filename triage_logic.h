#ifndef TRIAGE_LOGIC_H
#define TRIAGE_LOGIC_H

#include <Arduino.h>

// برای اینکه این فایل بتواند تابع stopChannel که در فایل اصلی است را صدا بزند:
void stopChannel(uint8_t c);

// ─── تعریف پروفایل‌های باتری 18650 ────────────────────────────
enum BatteryProfile {
  PROFILE_HIGH_DRAIN, 
  PROFILE_LAPTOP,     
  PROFILE_UNKNOWN,    
  PROFILE_CUSTOM      
};

// متغیرهای دینامیک برای آستانه‌های گریددهی (پیش‌فرض روی پروفایل ناشناخته)
float triage_grade_a_mohm = 80.0f;
float triage_grade_b_mohm = 150.0f;
float triage_grade_c_mohm = 300.0f;

// تابع تغییر پروفایل بر اساس استانداردهای جهانی
void setTriageProfile(BatteryProfile profile) {
  switch (profile) {
    case PROFILE_HIGH_DRAIN:
      // باتری‌های ابزار برقی (مثلاً Sony VTC, Samsung INR)
      triage_grade_a_mohm = 40.0f;
      triage_grade_b_mohm = 70.0f;
      triage_grade_c_mohm = 120.0f;
      break;
    case PROFILE_LAPTOP:
      // باتری‌های لپ‌تاپ و پاوربانک (مثلاً Panasonic NCR, Sanyo)
      triage_grade_a_mohm = 100.0f;
      triage_grade_b_mohm = 180.0f;
      triage_grade_c_mohm = 350.0f;
      break;
    case PROFILE_UNKNOWN:
      // استاندارد متعادل برای سلول‌های ناشناخته و بازیافتی
      triage_grade_a_mohm = 80.0f;
      triage_grade_b_mohm = 150.0f;
      triage_grade_c_mohm = 300.0f;
      break;
    case PROFILE_CUSTOM:
      // در این حالت مقادیر از طریق رابط وب یا حافظه تنظیم می‌شوند
      break;
  }
}

// حداقل جریانی که زیر بار باید ببینیم
const float TRIAGE_MIN_VALID_CURRENT_A = 0.1f;

// ─── تبدیل مقاومت اندازه‌گیری‌شده (mΩ) به حرف گرید ───────────────
// متن‌ها به حالت عمومی‌تری تغییر یافتند تا برای همه پروفایل‌ها منطقی باشند
String gradeFromMohm(float mohm) {
  if (mohm <= triage_grade_a_mohm) return "A (Excellent)";
  if (mohm <= triage_grade_b_mohm) return "B (Good)";
  if (mohm <= triage_grade_c_mohm) return "C (Acceptable)";
  return "D (Weak/Dead)";
}

// ─── خروج زودهنگام از پله‌های باقی‌مانده ───
void triageAbortNoLoad(uint8_t c, const char* stageLabel) {
  ch[c].qt_dcir = 0.0f;
  ch[c].qt_thermalStatus = String("No Current @ ") + stageLabel;
  ch[c].qt_grade = "Error (No Load)";
  Serial.printf("|- ERROR: Current near 0 at %s pulse. Battery disconnected or BMS protected. Aborting remaining steps.\n", stageLabel);

  digitalWrite(hwPins[c].pin_d500, LOW);
  digitalWrite(hwPins[c].pin_d1000, LOW);
  ch[c].qtPhase = QT_REST_FINAL;
  ch[c].qtPhaseStartMillis = millis();
}

// ─── تابع مدیریت فازهای هوشمند تریاژ ───────────────────────────
void processTriage(uint8_t c) {
  unsigned long elapsedTriage = millis() - ch[c].qtPhaseStartMillis;

  if (elapsedTriage >= 5000) { 
    float currentV = readVoltage(c);
    float pulseCurrentNow = readCurrent(c); 
    float wireVoltageDrop = (pulseCurrentNow / 1000.0f) * (WIRE_RESISTANCE_MOHM[c] / 1000.0f);
    float preciseVoltNow = currentV + wireVoltageDrop;
    ch[c].voltNow = preciseVoltNow; 

    Serial.printf("\n[CH%d - Triage Debug] Time: %lu ms\n", c + 1, millis());
    Serial.printf("|- Raw V: %.3f V | Wire Drop: %.3f V | Precise V: %.3f V | I: %.0f mA\n", currentV, wireVoltageDrop, preciseVoltNow, pulseCurrentNow);

    if (ch[c].qtPhase == QT_REST_INIT) {
      Serial.println("|- Phase: QT_REST_INIT (Finishing Initial Rest)");
      ch[c].qt_voltBase = preciseVoltNow;     
      Serial.printf("|- Volt Base Set to: %.3f V\n", ch[c].qt_voltBase);
      Serial.println("|- Action: Starting 500mA Pulse");
      
      digitalWrite(hwPins[c].pin_d500, HIGH); 
      ch[c].qtPhase = QT_PULSE_500;
      ch[c].qtPhaseStartMillis = millis();
    }
    else if (ch[c].qtPhase == QT_PULSE_500) {
      Serial.println("|- Phase: QT_PULSE_500 (Finishing 500mA Pulse)");
      ch[c].qt_voltDrop500 = ch[c].qt_voltBase - preciseVoltNow; 
      Serial.printf("|- Voltage Drop 500mA: %.3f V\n", ch[c].qt_voltDrop500);

      digitalWrite(hwPins[c].pin_d500, LOW); 

      if ((pulseCurrentNow / 1000.0f) <= TRIAGE_MIN_VALID_CURRENT_A) {
        triageAbortNoLoad(c, "500mA");
      } else {
        Serial.println("|- Action: Stopping 500mA, Starting Rest 1");
        ch[c].qtPhase = QT_REST_1;
        ch[c].qtPhaseStartMillis = millis();
      }
    }
    else if (ch[c].qtPhase == QT_REST_1) {
      Serial.println("|- Phase: QT_REST_1 (Finishing Rest 1)");
      ch[c].qt_voltRec500 = preciseVoltNow; 
      
      float endOfPulseVolt = ch[c].qt_voltBase - ch[c].qt_voltDrop500;
      ch[c].qt_recoveryRate = ((preciseVoltNow - endOfPulseVolt) * 1000.0f) / 5.0f; 
      Serial.printf("|- Recovery Rate: %.1f mV/s\n", ch[c].qt_recoveryRate);
      Serial.println("|- Action: Starting 1000mA Pulse");

      digitalWrite(hwPins[c].pin_d1000, HIGH); 
      ch[c].qtPhase = QT_PULSE_1000;
      ch[c].qtPhaseStartMillis = millis();
    }
    else if (ch[c].qtPhase == QT_PULSE_1000) {
      Serial.println("|- Phase: QT_PULSE_1000 (Finishing 1000mA Pulse)");
      ch[c].qt_voltDrop1000 = ch[c].qt_voltRec500 - preciseVoltNow; 
      Serial.printf("|- Voltage Drop 1000mA: %.3f V\n", ch[c].qt_voltDrop1000);

      digitalWrite(hwPins[c].pin_d1000, LOW);

      if ((pulseCurrentNow / 1000.0f) <= TRIAGE_MIN_VALID_CURRENT_A) {
        triageAbortNoLoad(c, "1000mA");
      } else {
        Serial.println("|- Action: Stopping 1000mA, Starting Rest 2");
        ch[c].qtPhase = QT_REST_2;
        ch[c].qtPhaseStartMillis = millis();
      }
    }
    else if (ch[c].qtPhase == QT_REST_2) {
      Serial.println("|- Phase: QT_REST_2 (Finishing Rest 2)");
      ch[c].qt_voltRec1000 = preciseVoltNow;
      Serial.println("|- Action: Starting 1500mA Pulse");

      digitalWrite(hwPins[c].pin_d500, HIGH); 
      digitalWrite(hwPins[c].pin_d1000, HIGH);
      ch[c].qtPhase = QT_PULSE_1500;
      ch[c].qtPhaseStartMillis = millis();
    }
    else if (ch[c].qtPhase == QT_PULSE_1500) {
      Serial.println("|- Phase: QT_PULSE_1500 (Finishing 1500mA Pulse)");
      ch[c].qt_voltDrop1500 = ch[c].qt_voltRec1000 - preciseVoltNow; 
      Serial.printf("|- Voltage Drop 1500mA: %.3f V\n", ch[c].qt_voltDrop1500);
      
      float currentNow = readCurrent(c);
      float currentA = currentNow / 1000.0f;
      
      if (currentA > TRIAGE_MIN_VALID_CURRENT_A) {
         ch[c].qt_dcir = (ch[c].qt_voltDrop1500 / currentA) * 1000.0f;
         Serial.printf("|- Calculated DCIR: %.1f mOhm\n", ch[c].qt_dcir);

         float thermalWatts = (currentA * currentA) * (ch[c].qt_dcir / 1000.0f);
         if (thermalWatts <= 0.15f) ch[c].qt_thermalStatus = "Excellent (Cool)";
         else if (thermalWatts <= 0.35f) ch[c].qt_thermalStatus = "Good (Warm)";
         else if (thermalWatts <= 0.60f) ch[c].qt_thermalStatus = "Warning (Hot)";
         else ch[c].qt_thermalStatus = "Danger (Overheat)";
         Serial.printf("|- Thermal Prediction: %s (%.2f W)\n", ch[c].qt_thermalStatus.c_str(), thermalWatts);

         ch[c].qt_grade = gradeFromMohm(ch[c].qt_dcir);
         Serial.printf("|- Final Grade: %s (based on %.1f mOhm)\n", ch[c].qt_grade.c_str(), ch[c].qt_dcir);
      } else {
         ch[c].qt_dcir = 0.0f;
         ch[c].qt_thermalStatus = "No Current @ 1500mA";
         ch[c].qt_grade = "Error (No Load)";
         Serial.println("|- ERROR: Current is near 0. Battery disconnected or BMS protected.");
      }

      Serial.println("|- Action: Stopping All, Starting Final Rest");
      digitalWrite(hwPins[c].pin_d500, LOW);
      digitalWrite(hwPins[c].pin_d1000, LOW);
      ch[c].qtPhase = QT_REST_FINAL;
      ch[c].qtPhaseStartMillis = millis();
    }
    else if (ch[c].qtPhase == QT_REST_FINAL) {
      Serial.println("|- Phase: QT_REST_FINAL (Finishing Final Rest)");
      Serial.println("|- Action: Triage Complete, Stopping Channel\n---------------------------------");
      stopChannel(c); 
    }
  }
}

#endif