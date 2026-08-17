#ifndef BALE_BOT_H
#define BALE_BOT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h> // اضافه شده برای خواندن فایل دیتابیس

// ─── تنظیمات ربات بله (Bale Messenger) ────────────────────
const String BALE_TOKEN = "1905265019:Nt4DFfg0HPsXdXn_qeyBM0oLBrTiG2CQkgg";
const String BALE_CHAT_ID = "1222218918";

// دسترسی به صفی که در فایل اصلی تعریف شده است
extern QueueHandle_t baleQueue;

// ─── تابع ارسال فایل CSV (آپلود استریم برای جلوگیری از کرش) ─────
void sendBaleBackupCSV() {
  File file = LittleFS.open("/global_log.csv", "r");
  if (!file) {
    Serial.println("Bale Backup Failed: global_log.csv not found.");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure(); // نادیده گرفتن سرتیفیکیت SSL

  if (!client.connect("tapi.bale.ai", 443)) {
    Serial.println("Bale Backup Failed: Connection error.");
    file.close();
    return;
  }

  // ساخت هدرهای فرمت Multipart
  String boundary = "----BaleFormBoundary" + String(millis());
  String head = "--" + boundary + "\r\n";
  head += "Content-Disposition: form-data; name=\"chat_id\"\r\n\r\n";
  head += BALE_CHAT_ID + "\r\n";
  head += "--" + boundary + "\r\n";
  head += "Content-Disposition: form-data; name=\"document\"; filename=\"global_log_backup.csv\"\r\n";
  head += "Content-Type: text/csv\r\n\r\n";

  String tail = "\r\n--" + boundary + "--\r\n";

  uint32_t contentLength = head.length() + file.size() + tail.length();

  // ارسال هدرهای HTTP
  client.println("POST /bot" + BALE_TOKEN + "/sendDocument HTTP/1.1");
  client.println("Host: tapi.bale.ai");
  client.println("Content-Type: multipart/form-data; boundary=" + boundary);
  client.println("Content-Length: " + String(contentLength));
  client.println();

  // ارسال بخش اول بدنه
  client.print(head);

  // خواندن و ارسال فایل به صورت تکه‌تکه (Chunking - مصرف رم بسیار پایین)
  uint8_t buf[512];
  while (file.available()) {
    size_t len = file.read(buf, sizeof(buf));
    client.write(buf, len);
  }
  file.close();

  // ارسال پایان فرم
  client.print(tail);

  // خواندن پاسخ سرور بله
  String response = client.readStringUntil('\n');
  Serial.println("Bale Backup Upload Response: " + response);
  
  client.stop();
}

// ─── Task ارسال پیام ──────────────────────────────────────
void baleTask(void *pvParameters) {
  String *msgPtr;
  while (true) {
    // تسک منتظر می‌ماند تا پیامی وارد صف شود
    if (xQueueReceive(baleQueue, &msgPtr, portMAX_DELAY)) {
      
      // بررسی اتصال وای‌فای
      if (WiFi.getMode() != WIFI_AP && WiFi.status() == WL_CONNECTED) {
        
        // 🌟 اگر پیام، فرمان ارسال بکاپ بود:
        if (*msgPtr == "[BACKUP_CSV]") {
           Serial.println("Starting Cloud Backup to Bale...");
           sendBaleBackupCSV();
        } 
        // 🌟 در غیر این صورت پیام متنی عادی ارسال شود:
        else {
          WiFiClientSecure client;
          client.setInsecure(); 
          
          HTTPClient http;
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
        }
      } else {
        Serial.println("Bale skipped: WiFi disconnected or running in AP mode.");
      }
      
      // در هر حالت حافظه سریعاً آزاد می‌شود
      delete msgPtr; 
    }
  }
}

// ─── تابع افزودن پیام به صف ───────────────────────────────
void sendBaleNotification(String message) {
  if (baleQueue == NULL) return; 
  
  String *msgPtr = new String(message);
  
  if (xQueueSend(baleQueue, &msgPtr, 0) != pdPASS) {
    delete msgPtr;
    Serial.println("Warning: Bale queue is full! Message dropped.");
  }
}

#endif