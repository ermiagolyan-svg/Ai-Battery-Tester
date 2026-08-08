#ifndef BALE_BOT_H
#define BALE_BOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// ─── تنظیمات ربات بله (Bale Messenger) ────────────────────
const String BALE_TOKEN = "1905265019:Nt4DFfg0HPsXdXn_qeyBM0oLBrTiG2CQkgg";
const String BALE_CHAT_ID = "1222218918";

// دسترسی به صفی که در فایل اصلی تعریف شده است
extern QueueHandle_t baleQueue;

// ─── Task ارسال پیام ──────────────────────────────────────
void baleTask(void *pvParameters) {
  String *msgPtr;
  while (true) {
    // تسک منتظر می‌ماند تا پیامی وارد صف شود (بدون درگیر کردن پردازنده مین‌لوپ)
    if (xQueueReceive(baleQueue, &msgPtr, portMAX_DELAY)) {
      
      // بررسی اینکه در حالت AP خالص نباشیم و به وای‌فای متصل باشیم
      if (WiFi.getMode() != WIFI_AP && WiFi.status() == WL_CONNECTED) {
        WiFiClientSecure client;
        client.setInsecure(); // نادیده گرفتن سرتیفیکیت SSL
        
        HTTPClient http;
        // اعمال تایم‌اوت سفارشی برای جلوگیری از قفل شدن در صورت کندی اینترنت
        http.setTimeout(3000); 
        
        String url = "https://tapi.bale.ai/bot" + BALE_TOKEN + "/sendMessage";
        String payload = "chat_id=" + BALE_CHAT_ID + "&text=" + *msgPtr;
        
        http.begin(client, url);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int httpCode = http.POST(payload);
        http.end();
        
        if(httpCode > 0) {
          Serial.println("Bale message sent successfully.");
        } else {
          Serial.printf("Bale message failed (Timeout/Net Error), code: %s\n", http.errorToString(httpCode).c_str());
        }
      } else {
        Serial.println("Bale skipped: WiFi disconnected or running in AP mode.");
      }
      
      // در هر حالت (موفق، خطا، یا حالت AP) پیام حذف و حافظه سریعاً آزاد می‌شود
      delete msgPtr; 
    }
  }
}

// ─── تابع افزودن پیام به صف ───────────────────────────────
void sendBaleNotification(String message) {
  if (baleQueue == NULL) return; // اگر صف ساخته نشده بود خارج شو
  
  // تخصیص پیام در حافظه دینامیک تا زمان ارسال از بین نرود
  String *msgPtr = new String(message);
  
  // ارسال پوینتر پیام به صف (بدون معطلی)
  if (xQueueSend(baleQueue, &msgPtr, 0) != pdPASS) {
    // اگر صف پر بود، حافظه را آزاد کن تا نشتی حافظه پیش نیاید
    delete msgPtr;
    Serial.println("Warning: Bale queue is full! Message dropped.");
  }
}

#endif