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
  
  .wifi-pill { display: inline-flex; align-items: center; gap: 8px; background: rgba(0,212,170,0.1); border: 1px solid rgba(0,212,170,0.4); color: var(--accent); padding: 6px 12px; border-radius: 20px; font-size: 12px; transition:0.2s; }
  .wifi-pill span { cursor: pointer; }
  .wifi-pill:hover { border-color: var(--accent); }
  .wifi-del { color: #ff3366; cursor: pointer; font-weight: bold; font-family: monospace; padding: 0 4px; transition: 0.2s;}
  .wifi-del:hover { background: rgba(255,51,102,0.2); border-radius: 50%; }
  
  .flex-row { display: flex; gap: 10px; flex-wrap: wrap; margin-bottom: 15px; }
  
  .input-field { width: 100%; background: var(--bg); border: 1px solid var(--border); color: var(--text); padding: 12px 14px; border-radius: 8px; margin-bottom: 15px; box-sizing: border-box; font-family: inherit; font-size: 14px; outline: none; transition: border-color 0.2s; }
  .input-field:focus { border-color: var(--accent); }
  .input-ltr { direction: ltr; text-align: left; }
</style>
</head><body>
<div class="container">
  <a href="/" class="nav-back" id="btnBack" data-i18n="back_dash">→ بازگشت به داشبورد اصلی</a>
  
  <div class="settings-grid">
    
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
      <p data-i18n="wifi_desc">شبکه‌های ذخیره شده روی دستگاه به صورت خودکار متصل می‌شوند. با اضافه کردن شبکه جدید، دستگاه ریستارت خواهد شد.</p>
      
      <div id="saved-wifi-container" style="margin-bottom:15px; display:none; background:var(--bg); padding:10px; border-radius:8px; border:1px solid var(--border);">
        <div style="font-size:12px; margin-bottom:8px; color:var(--muted);" data-i18n="wifi_saved_lbl">شبکه‌های ذخیره شده در دستگاه:</div>
        <div id="saved-wifi-list" style="display:flex; gap:8px; flex-wrap:wrap;"></div>
      </div>

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
      <h3 data-i18n="storage_title">📊 پاکسازی دیتابیس یکپارچه</h3>
      <p data-i18n="storage_desc">با این گزینه تمام رکوردهای ادغام شده و لاگ‌های خام از حافظه دستگاه پاک شده و شماره‌انداز باتری‌ها ریست می‌شود.</p>
      <button class="clear-btn" style="background:#ff6b35; color:#000; width:100%; flex:none; margin-top:auto;" onclick="clearGlobalLog()" data-i18n="clear_all">پاکسازی کامل دیتابیس (Global Wipe)</button>
    </div>
  </div>
</div>

<script>
const dict = {
  'fa': {
    'back_dash': '→ بازگشت به داشبورد اصلی',
    'lang_title': '🌐 انتخاب زبان (Language)',
    'lang_desc': 'زبان رابط کاربری را انتخاب کنید. این تنظیم در مرورگر شما ذخیره شده و پردازنده دستگاه را درگیر نمی‌کند.',
    'wifi_title': '📡 تنظیمات شبکه وای‌فای',
    'wifi_desc': 'شبکه‌های ذخیره شده روی دستگاه به صورت خودکار متصل می‌شوند. با اضافه کردن شبکه جدید، دستگاه ریستارت خواهد شد.',
    'wifi_saved_lbl': 'شبکه‌های ذخیره شده در دستگاه:',
    'wifi_scan': 'جستجوی شبکه‌های اطراف',
    'wifi_save': 'ذخیره و اتصال به شبکه',
    'wifi_success': 'اتصال موفق! شبکه در دستگاه ذخیره شد و در حال ریستارت است...',
    'wifi_error': 'خطا: رمز عبور اشتباه است یا مودم خارج از برد است!',
    'wifi_checking': 'در حال بررسی اتصال، لطفاً چند ثانیه صبر کنید...',
    'time_title': '⏰ همگام‌سازی زمان سیستم',
    'time_desc': 'برای ثبت دقیق زمان در فایل‌های لاگ (CSV)، ساعت دستگاه را با مرورگر خود همگام کنید.',
    'time_sync': 'همگام‌سازی زمان (Sync RTC)',
    'storage_title': '📊 پاکسازی دیتابیس یکپارچه',
    'storage_desc': 'با این گزینه تمام رکوردهای ادغام شده و لاگ‌های خام از حافظه دستگاه پاک شده و شماره‌انداز باتری‌ها ریست می‌شود.',
    'clear_all': 'پاکسازی کامل دیتابیس (Global Wipe)'
  },
  'en': {
    'back_dash': '← Back to Dashboard',
    'lang_title': '🌐 Language Selection',
    'lang_desc': 'Select your preferred UI language. This is saved locally in your browser to keep the ESP32 fast.',
    'wifi_title': '📡 WiFi Network Settings',
    'wifi_desc': 'Networks saved on the device will connect automatically. Adding a new network will restart the device.',
    'wifi_saved_lbl': 'Networks saved on device:',
    'wifi_scan': 'Scan Networks',
    'wifi_save': 'Save & Connect',
    'wifi_success': 'Connected! Network saved. Device is restarting...',
    'wifi_error': 'Error: Incorrect password or out of range!',
    'wifi_checking': 'Checking connection, please wait...',
    'time_title': '⏰ System Time Sync',
    'time_desc': 'Sync the internal clock with your browser to ensure accurate timestamps in your CSV log files.',
    'time_sync': 'Sync Time (RTC)',
    'storage_title': '📊 Global Database Wipe',
    'storage_desc': 'This option wipes all unified records and raw logs from the device memory and resets the battery ID counter.',
    'clear_all': 'Wipe Global Database'
  }
};

let currentLang = 'fa'; 

function changeLang(lang) {
  currentLang = lang;
  localStorage.setItem('sysLang', lang); 
  
  if (lang === 'fa') {
    document.body.style.direction = 'rtl';
    document.body.style.textAlign = 'right';
    document.getElementById('btnBack').style.direction = 'rtl';
  } else {
    document.body.style.direction = 'ltr';
    document.body.style.textAlign = 'left';
    document.getElementById('btnBack').style.direction = 'ltr';
  }

  document.querySelectorAll('[data-i18n]').forEach(el => {
    const key = el.getAttribute('data-i18n');
    if (dict[lang] && dict[lang][key]) el.innerText = dict[lang][key];
  });
}

function loadDeviceWiFi() {
  fetch('/getwifi').then(r => r.json()).then(saved => {
    let container = document.getElementById('saved-wifi-container');
    let list = document.getElementById('saved-wifi-list');
    
    if(!saved || saved.length === 0) {
        container.style.display = 'none';
        return;
    }
    
    container.style.display = 'block';
    list.innerHTML = '';
    
    saved.forEach(nw => {
        let pill = document.createElement('div');
        pill.className = 'wifi-pill';
        
        let label = document.createElement('span');
        label.textContent = nw.s;
        label.onclick = () => {
            const sel = document.getElementById('wifiSSID');
            sel.innerHTML = `<option value="${nw.s}">${nw.s}</option>`;
            sel.style.display = 'block';
            
            const pass = document.getElementById('wifiPass');
            pass.value = nw.p;
            pass.style.display = 'block';
            
            document.getElementById('btnSaveWifi').style.display = 'block';
        };
        
        let delBtn = document.createElement('div');
        delBtn.className = 'wifi-del';
        delBtn.textContent = 'x';
        delBtn.title = currentLang === 'fa' ? 'حذف از حافظه دستگاه' : 'Delete from device memory';
        delBtn.onclick = (e) => {
            e.stopPropagation();
            if(confirm(currentLang === 'fa' ? `حذف شبکه ${nw.s} از حافظه دستگاه؟` : `Delete network ${nw.s} from device?`)) {
                fetch('/delwifi', {method: 'POST', body: nw.s}).then(() => loadDeviceWiFi());
            }
        };
        
        pill.appendChild(label);
        pill.appendChild(delBtn);
        list.appendChild(pill);
    });
  }).catch(e => console.log('No wifi data found on device.'));
}

window.onload = () => {
  const savedLang = localStorage.getItem('sysLang') || 'fa';
  document.getElementById('langSelect').value = savedLang;
  changeLang(savedLang);
  loadDeviceWiFi(); 
};

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
      document.getElementById('wifiPass').value = '';
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
  res.textContent = dict[currentLang]['wifi_checking'];
  
  fetch('/setwifi', { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: `ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}` })
    .then(r => ({status: r.status}))
    .then(obj => {
       res.style.color = obj.status === 200 ? 'var(--accent)' : '#ff6b35';
       res.textContent = obj.status === 200 ? dict[currentLang]['wifi_success'] : dict[currentLang]['wifi_error'];
       
       if (obj.status === 200) {
           setTimeout(() => res.textContent = '', 5000);
       }
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

function clearGlobalLog() { 
  const msg = currentLang === 'fa' ? 'آیا از پاکسازی کل دیتابیس یکپارچه و ریست شدن شماره‌انداز اطمینان دارید؟' : 'Are you sure you want to wipe the global database and reset the counter?';
  const successMsg = currentLang === 'fa' ? 'پاکسازی با موفقیت انجام شد.' : 'Database wiped successfully.';
  if(confirm(msg)) { 
    fetch('/clearlog',{method:'POST'}).then(()=>alert(successMsg)); 
  } 
}
</script></body></html>
)rawhtml";
}

#endif