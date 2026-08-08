#ifndef PAGE_TRIAGE_H
#define PAGE_TRIAGE_H

#include <Arduino.h>

String buildTriagePage() {
  return R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Triage - Under Construction</title>
<style>
  body { background: #0d0d0f; color: #e8e8f0; font-family: system-ui, sans-serif; text-align: center; padding: 50px; direction: rtl; }
  .container { border: 1px solid #2a2a32; padding: 30px; border-radius: 12px; background: #161619; display: inline-block; margin-top: 50px; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }
  h1 { color: #ff6b35; margin-bottom: 15px; }
  p { color: #6b6b80; font-size: 14px; margin-bottom: 25px; }
  .btn-back { color: #00d4aa; text-decoration: none; font-weight: bold; border: 1px solid #00d4aa; padding: 10px 20px; border-radius: 6px; transition: 0.2s; background: rgba(0,212,170,.1); display: inline-block; }
  .btn-back:hover { background: #00d4aa; color: #000; }
</style>
</head>
<body>
  <div class="container">
    <h1>🚧 بخش تریاژ در حال ساخت است 🚧</h1>
    <p>این صفحه به زودی با قابلیت‌ها و امکانات جدید به‌روزرسانی خواهد شد.</p>
    <a href="/" class="btn-back">→ بازگشت به داشبورد</a>
  </div>
</body>
</html>
)rawhtml";
}

#endif