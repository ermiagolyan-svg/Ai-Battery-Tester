#ifndef TRIAGE_LOGIC_H
#define TRIAGE_LOGIC_H

#include <Arduino.h>

void stopChannel(uint8_t c);

const float TRIAGE_MIN_VALID_CURRENT_A = 0.1f;

// ==============================================================================
// 🧠 مدل شبکه عصبی (AI TinyML / MLP)
// ==============================================================================

const float ai_input_mean[4] = {173.5880, 219.9374, 1.2423, 0.4123};
const float ai_input_scale[4] = {153.8867, 184.9004, 0.9352, 0.2976};

const float ai_W1[4][8] = {
    {1.2494, 1.2423, 0.4054, 0.8977, 1.0386, -1.3105, -2.2269, -0.2796},
    {0.9774, 0.6648, -0.4494, 1.0569, 1.2002, -1.1553, -1.8880, -0.5959},
    {-1.4156, 0.0225, -0.1108, -1.0251, -1.2015, 0.1093, 0.9053, -0.5753},
    {0.6062, 0.3953, -0.3037, 0.0040, 0.9054, -0.8111, -1.0623, 0.0448}
};
const float ai_b1[8] = {-0.9863, 2.3486, 0.9161, 0.4711, 1.6583, -1.8947, -0.1112, 1.6201};

const float ai_W2[8][4] = {
    {-1.0577, -0.6978, -2.9391, 2.4081},
    {-2.9198, 0.4057, 0.7282, 0.2029},
    {0.1835, 0.1213, 0.8910, -0.0787},
    {0.2873, 0.1396, 0.4838, 1.0104},
    {-1.7485, -2.3899, 1.0196, 0.2499},
    {2.1276, -2.2884, -0.0097, -0.4578},
    {0.8960, -0.1261, -2.1067, -0.3912},
    {-0.8978, 2.0714, 0.8680, -1.5481}
};
const float ai_b2[4] = {-0.5272, 0.9210, 1.1192, -0.3202};

String predictBatteryGrade(float ir, float dcir, float rec, float vDrop) {
    if (ir <= 0 || dcir <= 0) return "N/A"; 
    
    // 💡 رفع باگ: تبدیل نرخ ریکاوری دستگاه به مقیاس قابل فهم برای شبکه عصبی
    float scaled_rec = rec / 18.0f;
    
    float x[4] = {
        (ir - ai_input_mean[0]) / ai_input_scale[0],
        (dcir - ai_input_mean[1]) / ai_input_scale[1],
        (scaled_rec - ai_input_mean[2]) / ai_input_scale[2],
        (vDrop - ai_input_mean[3]) / ai_input_scale[3]
    };
    
    float h[8] = {0};
    for (int j = 0; j < 8; j++) {
        h[j] = ai_b1[j];
        for (int i = 0; i < 4; i++) {
            h[j] += x[i] * ai_W1[i][j];
        }
        if (h[j] < 0) h[j] = 0; 
    }
    
    float out[4] = {0};
    for (int j = 0; j < 4; j++) {
        out[j] = ai_b2[j];
        for (int i = 0; i < 8; i++) {
            out[j] += h[i] * ai_W2[i][j];
        }
    }
    
    int bestClass = 0;
    float maxVal = out[0];
    for (int j = 1; j < 4; j++) {
        if (out[j] > maxVal) {
            maxVal = out[j];
            bestClass = j;
        }
    }
    
    if (bestClass == 0) return "A (High Drain)";
    if (bestClass == 1) return "B (Standard/Laptop)";
    if (bestClass == 2) return "C (Weak/Low Drain)";
    return "D (Dead/Heater)";
}
// ==============================================================================

void triageAbortNoLoad(uint8_t c, const char* stageLabel) {
  ch[c].qt_dcir = 0.0f;
  ch[c].qt_thermalStatus = String("No Current @ ") + stageLabel;
  ch[c].qt_grade = "N/A"; 
  Serial.printf("|- ERROR: Current near 0 at %s pulse. Battery disconnected or BMS protected. Aborting remaining steps.\n", stageLabel);

  digitalWrite(hwPins[c].pin_d500, LOW);
  digitalWrite(hwPins[c].pin_d1000, LOW);
  ch[c].qtPhase = QT_REST_FINAL;
  ch[c].qtPhaseStartMillis = millis();
}

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

         ch[c].qt_grade = predictBatteryGrade(ch[c].IR, ch[c].qt_dcir, ch[c].qt_recoveryRate, ch[c].qt_voltDrop1500);
         Serial.printf("|- AI Grade Prediction: %s\n", ch[c].qt_grade.c_str());
         
      } else {
         ch[c].qt_dcir = 0.0f;
         ch[c].qt_thermalStatus = "No Current @ 1500mA";
         ch[c].qt_grade = "N/A"; 
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