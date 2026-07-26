#ifndef SENSOR_INA3221_H
#define SENSOR_INA3221_H

#include <Arduino.h>
#include <Wire.h>
#include <INA3221.h> // Rob Tillaart's INA3221 library

// ─── مقادیر کالیبراسیون و سخت‌افزار ───────────────────
const float SHUNT_OHMS = 0.020f;  // R020
const float WIRE_RESISTANCE_MOHM[3] = {95.9f, 116.3f, 114.2f}; // مقاومت سیم‌ها برای ۳ کانال

// ─── نمونه‌سازی سنسور ─────────────────────────────────
INA3221 ina3221(0x40);

// ─── راه‌اندازی اولیه سنسور ────────────────────────────
void setupINA3221() {
  ina3221.begin();
  ina3221.reset();
  ina3221.setAverage(3); // دقت بالاتر پیش‌فرض (حذف نویز)
  ina3221.setShuntR(0, SHUNT_OHMS);
  ina3221.setShuntR(1, SHUNT_OHMS);
  ina3221.setShuntR(2, SHUNT_OHMS);
}

// ─── خواندن ولتاژ (با ضریب کالیبراسیون) ─────────────────
float readVoltage(uint8_t c) { 
  return ina3221.getBusVoltage(c) * 0.9498; 
}

// ─── خواندن جریان (تبدیل به میلی‌آمپر) ─────────────────
float readCurrent(uint8_t c) {
  float current = abs(ina3221.getCurrent(c) * 1000.0f); 
  return current;
}

#endif