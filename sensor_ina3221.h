#ifndef SENSOR_INA3221_H
#define SENSOR_INA3221_H

#include <Arduino.h>
#include <Wire.h>
#include <INA3221.h>
#include <Preferences.h>

extern Preferences prefs; // دسترسی به حافظه NVS

// ─── مقادیر کالیبراسیون و سخت‌افزار ───────────────────
float volt_cal[3] = {0.9495f, 0.9495f, 0.9495f}; // ضریب پیش‌فرض ولتاژ
float shunt_res[3] = {0.020f, 0.020f, 0.020f};   // مقاومت پیش‌فرض شنت
float WIRE_RESISTANCE_MOHM[3] = {102.7f, 116.4f, 128.5f}; // مقاومت پیش‌فرض سیم‌ها (متغیر شده)

// ─── نمونه‌سازی سنسور ─────────────────────────────────
INA3221 ina3221(0x40);

// ─── راه‌اندازی اولیه سنسور ────────────────────────────
void setupINA3221() {
  float def_w[3] = {95.9f, 116.3f, 114.2f};
  
  // بارگذاری مقادیر کالیبراسیون از حافظه
  for(int i = 0; i < 3; i++) {
    volt_cal[i] = prefs.getFloat(String("vcal" + String(i)).c_str(), 0.9498f);
    shunt_res[i] = prefs.getFloat(String("rcal" + String(i)).c_str(), 0.020f);
    WIRE_RESISTANCE_MOHM[i] = prefs.getFloat(String("wcal" + String(i)).c_str(), def_w[i]);
  }

  ina3221.begin();
  ina3221.reset();
  ina3221.setAverage(3); 
  
  ina3221.setShuntR(0, shunt_res[0]);
  ina3221.setShuntR(1, shunt_res[1]);
  ina3221.setShuntR(2, shunt_res[2]);
}

// ─── خواندن ولتاژ (با اعمال ضریب خطای هر کانال) ────────
float readVoltage(uint8_t c) { 
  return ina3221.getBusVoltage(c) * volt_cal[c]; 
}

// ─── خواندن جریان (تبدیل به میلی‌آمپر) ─────────────────
float readCurrent(uint8_t c) {
  float current = abs(ina3221.getCurrent(c) * 1000.0f); 
  return current;
}

#endif