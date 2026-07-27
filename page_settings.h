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
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; padding: 20px; direction: rtl; text-align: right; transition: all 0.3s ease;}
  .container { max-width: 1200px; margin: 0 auto; }
  .nav-back { color: var(--accent); text-decoration: none; font-weight: 700; font-size: 15px; display: inline-block; margin-bottom: 20px; transition: 0.2s; direction: ltr;}
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
  <a href="/" class="nav-back" id="btnBack" data-i18n="back_dash">→ بازگشت به داشبورد اصلی</a>
  
  <div class="settings-grid">
    
    <!-- پنل انتخاب زبان -->
    <div class="card">
      <h3 data-i18n="lang_title">🌐 انتخاب زبان (Language)</h3>
      <p data-i18n="lang_desc">زبان رابط کاربری را انتخاب کنید. این تنظیم در مرورگر شما ذخیره شده و پردازنده دستگاه را درگیر نمی‌کند.</p>
      <select class="input-field" id="langSelect" onchange="changeLang(this.value)" style="margin-top:auto;">
        <option value="fa">فارسی (Persian)</option>
        <option value="en">English</option>
      </select>
    </div>

    <div class="card">
      <h3 data-i18n="wifi_title">📡 تنظیمات شبکه وای‌فای</h3>
      <p data-i18n="wifi_desc">شبکه وای‌فای متصل به دستگاه را تغییر دهید. توجه داشته باشید پس از ذخیره رمز عبور، دستگاه به صورت خودکار ریستارت می‌شود.</p>
      <button class="btn" style="background:var(--surface); color:var(--accent); border:1px solid var(--accent); margin-bottom: 15px; margin-top:0;" onclick="scanWiFi()" id="btnScan" data-i18n="wifi_scan">جستجوی شبکه‌های اطراف</button>
      
      <select class="input-field input-ltr" id="wifiSSID" style="display:none;"></select>
      <input type="password" class="input-field input-ltr" id="wifiPass" placeholder="Password" style="display:none;">
      
      <button class="btn" id="btnSaveWifi" onclick="saveWiFi()" style="display:none;" data-i18n="wifi_save">ذخیره و اتصال به شبکه</button>
      <div id="resWifi" style="margin-top:12px; font-size:13px; text-align:center; font-weight:bold;"></div>
    </div>
    
    <div class="card">
      <h3 data-i18n="time_title">⏰ همگام‌سازی زمان سیستم</h3>
      <p data-i18n="time_desc">برای ثبت دقیق زمان در فایل‌های لاگ (CSV)، ساعت دستگاه را با مرورگر خود همگام کنید.</p>
      <button class="btn" onclick="sync()" data-i18n="time_sync">همگام‌سازی زمان (Sync RTC)</button>
      <div id="res" style="margin-top:12px; color:var(--accent); font-size:13px; text-align:center; font-weight:bold;"></div>
    </div>
    
    <div class="card">
      <h3 data-i18n="storage_title">📊 پاکسازی حافظه داخلی</h3>
      <p data-i18n="storage_desc">شما می‌توانید فایل‌های گزارش قدیمی مربوط به هر کانال را از روی حافظه داخلی پاک کنید تا فضای خالی ایجاد شود.</p>
      <div class="flex-row">
        <button class="clear-btn" onclick="clearL(0)" data-i18n="clear_ch1">کانال ۱</button>
        <button class="clear-btn" onclick="clearL(1)" data-i18n="clear_ch2">کانال ۲</button>
        <button class="clear-btn" onclick="clearL(2)" data-i18n="clear_ch3">کانال ۳</button>
      </div>
      <button class="clear-btn" style="background:#ff6b35; color:#000; width:100%; flex:none; margin-top:auto;" onclick="clearL(-1)" data-i18n="clear_all">پاکسازی تمام اطلاعات</button>
    </div>
  </div>
</div>

<script>
// --- بخش دیکشنری و تغییر زبان (سمت مرورگر) ---
const dict = {
  'fa': {
    'back_dash': '→ بازگشت به داشبورد اصلی',
    'lang_title': '🌐 انتخاب زبان (Language)',
    'lang_desc': 'زبان رابط کاربری را انتخاب کنید. این تنظیم در مرورگر شما ذخیره شده و پردازنده دستگاه را درگیر نمی‌کند.',
    'wifi_title': '📡 تنظیمات شبکه وای‌فای',
    'wifi_desc': 'شبکه وای‌فای متصل به دستگاه را تغییر دهید. توجه داشته باشید پس از ذخیره رمز عبور، دستگاه به صورت خودکار ریستارت می‌شود.',
    'wifi_scan': 'جستجوی شبکه‌های اطراف',
    'wifi_save': 'ذخیره و اتصال به شبکه',
    'time_title': '⏰ همگام‌سازی زمان سیستم',
    'time_desc': 'برای ثبت دقیق زمان در فایل‌های لاگ (CSV)، ساعت دستگاه را با مرورگر خود همگام کنید.',
    'time_sync': 'همگام‌سازی زمان (Sync RTC)',
    'storage_title': '📊 پاکسازی حافظه داخلی',
    'storage_desc': 'شما می‌توانید فایل‌های گزارش قدیمی مربوط به هر کانال را از روی حافظه داخلی پاک کنید تا فضای خالی ایجاد شود.',
    'clear_ch1': 'کانال ۱',
    'clear_ch2': 'کانال ۲',
    'clear_ch3': 'کانال ۳',
    'clear_all': 'پاکسازی تمام اطلاعات'
  },
  'en': {
    'back_dash': '← Back to Dashboard',
    'lang_title': '🌐 Language Selection',
    'lang_desc': 'Select your preferred UI language. This is saved locally in your browser to keep the ESP32 fast.',
    'wifi_title': '📡 WiFi Network Settings',
    'wifi_desc': 'Change the WiFi network connected to the device. The analyzer will automatically reboot after saving.',
    'wifi_scan': 'Scan Networks',
    'wifi_save': 'Save & Connect',
    'time_title': '⏰ System Time Sync',
    'time_desc': 'Sync the internal clock with your browser to ensure accurate timestamps in your CSV log files.',
    'time_sync': 'Sync Time (RTC)',
    'storage_title': '📊 Storage Wipe',
    'storage_desc': 'You can safely delete old CSV log files for each channel from the internal LittleFS memory.',
    'clear_ch1': 'Channel 1',
    'clear_ch2': 'Channel 2',
    'clear_ch3': 'Channel 3',
    'clear_all': 'Wipe All Data'
  }
};

let currentLang = 'fa'; // زبان پیش‌فرض

function changeLang(lang) {
  currentLang = lang;
  localStorage.setItem('sysLang', lang); // ذخیره در مرورگر کاربر
  
  // تنظیم راست‌چین یا چپ‌چین شدن کل صفحه
  if (lang === 'fa') {
    document.body.style.direction = 'rtl';
    document.body.style.textAlign = 'right';
    document.getElementById('btnBack').style.direction = 'rtl';
  } else {
    document.body.style.direction = 'ltr';
    document.body.style.textAlign = 'left';
    document.getElementById('btnBack').style.direction = 'ltr';
  }

  // جایگذاری متن‌های جدید بدون رفرش صفحه
  document.querySelectorAll('[data-i18n]').forEach(el => {
    const key = el.getAttribute('data-i18n');
    if (dict[lang] && dict[lang][key]) {
      el.innerText = dict[lang][key];
    }
  });
}

// بارگذاری زبان هنگام باز شدن صفحه
window.onload = () => {
  const savedLang = localStorage.getItem('sysLang') || 'fa';
  document.getElementById('langSelect').value = savedLang;
  changeLang(savedLang);
};

// --- توابع اصلی کنترل دستگاه ---
function scanWiFi() {
  const btn = document.getElementById('btnScan');
  btn.textContent = currentLang === 'fa' ? 'در حال جستجو...' : 'Scanning...';
  
  const pollScan = () => {
    fetch('/scanwifi').then(r => {
      if (r.status === 202) setTimeout(pollScan, 1000);
      else if (r.status === 200) return r.json();
      else throw new Error('Scan Failed');
    }).then(d => {
      if(!d) return;
      const sel = document.getElementById('wifiSSID');
      sel.innerHTML = currentLang === 'fa' ? '<option value="">انتخاب شبکه...</option>' : '<option value="">Select a network...</option>';
      d.networks.forEach(nw => { sel.innerHTML += `<option value="${nw.ssid}">${nw.ssid} (${nw.rssi}dBm)</option>`; });
      sel.style.display = 'block';
      document.getElementById('wifiPass').style.display = 'block';
      document.getElementById('btnSaveWifi').style.display = 'block';
      btn.textContent = dict[currentLang]['wifi_scan'];
    }).catch(e => { 
      btn.textContent = currentLang === 'fa' ? 'خطا در جستجو!' : 'Scan Failed!'; 
      setTimeout(() => btn.textContent = dict[currentLang]['wifi_scan'], 2000); 
    });
  };
  pollScan();
}

function saveWiFi() {
  const ssid = document.getElementById('wifiSSID').value;
  const pass = document.getElementById('wifiPass').value;
  if(!ssid) return alert(currentLang === 'fa' ? 'لطفاً یک شبکه انتخاب کنید!' : 'Please select a network!');
  
  const res = document.getElementById('resWifi');
  res.style.color = 'var(--accent)';
  res.textContent = currentLang === 'fa' ? 'در حال بررسی اتصال، لطفاً چند ثانیه صبر کنید...' : 'Checking connection, please wait...';
  
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
      res.textContent = currentLang === 'fa' ? 'زمان با موفقیت همگام شد!' : 'Time synchronized successfully!'; 
      setTimeout(() => res.textContent = '', 3000);
    }); 
}

function clearL(ch) { 
  const msg = currentLang === 'fa' ? 'آیا از پاکسازی اطلاعات فایل لاگ اطمینان دارید؟' : 'Are you sure you want to delete this log?';
  const successMsg = currentLang === 'fa' ? 'پاکسازی با موفقیت انجام شد.' : 'Data wiped successfully.';
  if(confirm(msg)) { 
    fetch('/clearlog',{method:'POST',body:String(ch)}).then(()=>alert(successMsg)); 
  } 
}
</script></body></html>
)rawhtml";
}

#endif