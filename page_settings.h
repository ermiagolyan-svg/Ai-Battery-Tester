#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <Arduino.h>

String buildSettingsPage() {
  return R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Settings - Li-ion Analyzer</title>
<style>
  :root { --bg: #0d0d0f; --surface: #161619; --border: #2a2a32; --accent: #00d4aa; --text: #e8e8f0; --muted: #6b6b80; }
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; padding: 20px; direction: rtl; text-align: right; }
  .container { max-width: 1200px; margin: 0 auto; }
  .nav-back { color: var(--accent); text-decoration: none; font-weight: 700; font-size: 15px; display: inline-block; margin-bottom: 20px; transition: 0.2s; }
  .nav-back:hover { text-shadow: 0 0 8px rgba(0,212,170,0.5); }
  
  .settings-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 20px; }
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 14px; padding: 24px; display: flex; flex-direction: column; }
  h3 { color: var(--accent); font-size: 18px; margin-bottom: 12px; display: flex; align-items: center; gap: 8px; }
  p { font-size: 13px; color: var(--muted); margin-bottom: 20px; line-height: 1.6; }
  
  .btn { padding: 12px 20px; background: var(--accent); color: #000; border: none; border-radius: 8px; font-weight: 600; font-size: 14px; cursor: pointer; width: 100%; transition: all 0.2s; margin-top: auto; }
  .btn:hover { filter: brightness(1.1); transform: translateY(-1px); }
  
  .clear-btn { background: #161619; color: #ff6b35; border: 1px solid #ff6b35; padding: 10px; border-radius: 8px; cursor: pointer; flex: 1; text-align: center; font-weight: 600; font-size: 13px; transition: 0.2s; }
  .clear-btn:hover { background: rgba(255,107,53,0.1); }
  .flex-row { display: flex; gap: 10px; flex-wrap: wrap; margin-bottom: 15px; }
  
  .input-field { width: 100%; background: var(--bg); border: 1px solid var(--border); color: var(--text); padding: 12px 14px; border-radius: 8px; margin-bottom: 15px; box-sizing: border-box; font-family: inherit; font-size: 14px; outline: none; transition: border-color 0.2s; }
  .input-field:focus { border-color: var(--accent); }
  .input-ltr { direction: ltr; text-align: left; }
</style>
</head><body>
<div class="container">
  <a href="/" class="nav-back">→ بازگشت به داشبورد اصلی</a>
  
  <div class="settings-grid">
    <div class="card">
      <h3>📡 تنظیمات شبکه وای‌فای</h3>
      <p>شبکه وای‌فای متصل به دستگاه را تغییر دهید. توجه داشته باشید پس از ذخیره رمز عبور، دستگاه به صورت خودکار ریستارت می‌شود.</p>
      <button class="btn" style="background:var(--surface); color:var(--accent); border:1px solid var(--accent); margin-bottom: 15px; margin-top:0;" onclick="scanWiFi()" id="btnScan">جستجوی شبکه‌های اطراف</button>
      
      <select class="input-field input-ltr" id="wifiSSID" style="display:none;"></select>
      <input type="password" class="input-field input-ltr" id="wifiPass" placeholder="Password" style="display:none;">
      
      <button class="btn" id="btnSaveWifi" onclick="saveWiFi()" style="display:none;">ذخیره و اتصال به شبکه</button>
      <div id="resWifi" style="margin-top:12px; font-size:13px; text-align:center; font-weight:bold;"></div>
    </div>
    
    <div class="card">
      <h3>⏰ همگام‌سازی زمان سیستم</h3>
      <p>از آنجایی که دستگاه ماژول ساعت فیزیکی ندارد، برای ثبت دقیق زمان در فایل‌های لاگ (CSV)، ساعت دستگاه را با مرورگر خود همگام کنید.</p>
      <button class="btn" onclick="sync()">همگام‌سازی زمان (Sync RTC)</button>
      <div id="res" style="margin-top:12px; color:var(--accent); font-size:13px; text-align:center; font-weight:bold;"></div>
    </div>
    
    <div class="card">
      <h3>📊 پاکسازی حافظه داخلی</h3>
      <p>شما می‌توانید فایل‌های گزارش قدیمی مربوط به هر کانال را از روی حافظه داخلی (LittleFS) پاک کنید تا فضای خالی ایجاد شود.</p>
      <div class="flex-row">
        <button class="clear-btn" onclick="clearL(0)">کانال ۱</button>
        <button class="clear-btn" onclick="clearL(1)">کانال ۲</button>
        <button class="clear-btn" onclick="clearL(2)">کانال ۳</button>
      </div>
      <button class="clear-btn" style="background:#ff6b35; color:#000; width:100%; flex:none; margin-top:auto;" onclick="clearL(-1)">پاکسازی تمام اطلاعات</button>
    </div>
  </div>
</div>

<script>
function scanWiFi() {
  const btn = document.getElementById('btnScan');
  btn.textContent = 'در حال جستجو...';
  
  const pollScan = () => {
    fetch('/scanwifi').then(r => {
      if (r.status === 202) setTimeout(pollScan, 1000);
      else if (r.status === 200) return r.json();
      else throw new Error('Scan Failed');
    }).then(d => {
      if(!d) return;
      const sel = document.getElementById('wifiSSID');
      sel.innerHTML = '<option value="">انتخاب شبکه...</option>';
      d.networks.forEach(nw => { sel.innerHTML += `<option value="${nw.ssid}">${nw.ssid} (${nw.rssi}dBm)</option>`; });
      sel.style.display = 'block';
      document.getElementById('wifiPass').style.display = 'block';
      document.getElementById('btnSaveWifi').style.display = 'block';
      btn.textContent = 'جستجوی مجدد';
    }).catch(e => { 
      btn.textContent = 'خطا در جستجو!'; 
      setTimeout(() => btn.textContent = 'جستجوی شبکه‌های اطراف', 2000); 
    });
  };
  pollScan();
}

function saveWiFi() {
  const ssid = document.getElementById('wifiSSID').value;
  const pass = document.getElementById('wifiPass').value;
  if(!ssid) return alert('لطفاً یک شبکه وای‌فای انتخاب کنید!');
  
  const res = document.getElementById('resWifi');
  res.style.color = 'var(--accent)';
  res.textContent = 'در حال بررسی اتصال، لطفاً چند ثانیه صبر کنید...';
  
  fetch('/setwifi', { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: `ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}` })
    .then(r => r.text().then(t => ({status: r.status, text: t})))
    .then(obj => {
       res.style.color = obj.status === 200 ? 'var(--accent)' : '#ff6b35';
       res.textContent = obj.text;
       setTimeout(() => res.textContent = '', 5000);
    });
}

function sync() { 
  const epoch = Math.floor(Date.now() / 1000); 
  fetch('/settime',{method:'POST',body:`epoch=${epoch}`})
    .then(r=>r.text()).then(t=>{ 
      const res = document.getElementById('res'); 
      res.textContent = 'زمان با موفقیت همگام شد!'; 
      setTimeout(() => res.textContent = '', 3000);
    }); 
}

function clearL(ch) { 
  if(confirm('آیا از پاکسازی اطلاعات فایل لاگ اطمینان دارید؟')) { 
    fetch('/clearlog',{method:'POST',body:String(ch)}).then(()=>alert('پاکسازی با موفقیت انجام شد.')); 
  } 
}
</script></body></html>
)rawhtml";
}

#endif