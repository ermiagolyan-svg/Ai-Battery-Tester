#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <Arduino.h>

const char PAGE_SETTINGS_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<title>Settings - Li-ion Analyzer</title>
<style>
  :root { 
    --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; 
    --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace;
    --dir-sign: 1; 
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; min-height: 100vh; min-height: 100dvh; direction: rtl; text-align: right; transition: all 0.3s ease; padding-inline-start: max(70px, env(safe-area-inset-left)); overflow-x: hidden; padding-top: env(safe-area-inset-top); padding-bottom: max(20px, env(safe-area-inset-bottom)); margin: 0; }
  
  .main-content { padding: 20px; max-width: 1200px; margin: 0 auto; width: 100%; box-sizing: border-box; }

  /* Overlay بک‌گراند برای زمانی که کشو باز است */
  .sidebar-overlay { position: fixed; inset: 0; background: rgba(0,0,0,0.6); z-index: 95; opacity: 0; pointer-events: none; transition: opacity 0.3s ease; backdrop-filter: blur(2px); }
  .sidebar-overlay.active { opacity: 1; pointer-events: auto; }
  
  /* تاپ‌بار مخصوص موبایل */
  .mobile-top-bar { display: none; }

  /* سایدبار کشویی */
  header#mainSidebar { position: fixed; top: 0; bottom: 0; inset-inline-start: 0; width: 70px; background: var(--surface); border-inline-end: 1px solid var(--border); display: flex; flex-direction: column; padding-top: max(16px, env(safe-area-inset-top)); padding-bottom: max(20px, env(safe-area-inset-bottom)); z-index: 100; transition: width 0.3s cubic-bezier(0.4, 0, 0.2, 1); overflow-x: hidden; white-space: nowrap; }
  header#mainSidebar.open { width: 260px; box-shadow: 0 0 24px rgba(0,0,0,0.7); }
  
  .top-bar-sidebar { display: flex; align-items: center; padding: 0 13px 16px; gap: 16px; min-height: 50px; }
  .menu-toggle { width: 44px; height: 44px; display: flex; align-items: center; justify-content: center; background: transparent; border: none; color: var(--text); cursor: pointer; border-radius: 50%; flex-shrink: 0; transition: background 0.2s; }
  .menu-toggle:hover { background: var(--surface2); }
  
  .logo-icon { font-size: 20px; width: 24px; height: 24px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  
  /* مخفی/نمایان شدن متون کشو */
  .text, .logo-text { opacity: 0; transition: opacity 0.2s ease; font-weight: 600; }
  .logo-text { font-size: 16px; }
  header#mainSidebar.open .text, header#mainSidebar.open .logo-text { opacity: 1; transition-delay: 0.1s; }
  
  .nav-links { display: flex; flex-direction: column; gap: 6px; flex: 1; margin-top: 10px; }
  .nav-links a, .bottom-section a { text-decoration: none; color: var(--text); font-size: 14px; padding: 0 12px; height: 44px; margin: 0 13px; border-radius: 50px; display: flex; align-items: center; gap: 16px; transition: background 0.2s, color 0.2s; border: none; font-weight: normal; }
  .nav-links a:hover, .bottom-section a:hover { background: var(--surface2); }
  .nav-links a.special { background: rgba(0, 212, 170, 0.1); color: var(--accent); }
  .nav-links a.special:hover { background: rgba(0, 212, 170, 0.2); }
  .icon { font-size: 18px; width: 20px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  
  .bottom-section { display: flex; flex-direction: column; gap: 6px; margin-top: auto; padding-top: 16px; border-top: 1px solid var(--border); }
  
  /* عناصر مختص صفحه Settings */
  .nav-back { color: var(--accent); text-decoration: none; font-weight: 700; font-size: 15px; display: inline-block; margin-bottom: 20px; transition: 0.2s; direction: inherit;}
  .nav-back:hover { text-shadow: 0 0 8px rgba(0,212,170,0.5); }
  
  .settings-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 20px; }
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 14px; padding: 24px; display: flex; flex-direction: column; }
  h3 { color: var(--accent); font-size: 18px; margin-bottom: 12px; display: flex; align-items: center; gap: 8px; }
  p { font-size: 13px; color: var(--muted); margin-bottom: 20px; line-height: 1.6; }
  
  .btn { padding: 12px 20px; background: var(--accent); color: #000; border: none; border-radius: 8px; font-weight: 600; font-size: 14px; cursor: pointer; width: 100%; transition: all 0.2s; margin-top: auto; font-family: inherit;}
  .btn:hover { filter: brightness(1.1); transform: translateY(-1px); }
  
  .clear-btn { background: #161619; color: #ff6b35; border: 1px solid #ff6b35; padding: 10px; border-radius: 8px; cursor: pointer; flex: 1; text-align: center; font-weight: 600; font-size: 13px; transition: 0.2s; font-family: inherit;}
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

  @media (max-width: 700px) {
    /* تغییرات اصلی بدنه برای هدر بالایی */
    body { padding-inline-start: env(safe-area-inset-left); padding-top: calc(64px + env(safe-area-inset-top)); padding-bottom: max(40px, env(safe-area-inset-bottom)); }
    .main-content { padding: 15px; }
    
    /* نوار افقی بالای صفحه در موبایل */
    .mobile-top-bar { display: flex; position: fixed; top: 0; left: 0; right: 0; height: calc(64px + env(safe-area-inset-top)); padding: env(safe-area-inset-top) 16px 0; background: var(--bg); align-items: center; justify-content: space-between; z-index: 90; }
    .mobile-top-bar .title-group { display: flex; align-items: center; gap: 12px; font-size: 18px; font-weight: 500; }
    
    /* تبدیل سایدبار به کشوی متحرک */
    header#mainSidebar { 
       width: 280px; 
       transform: translateX(calc(100% * var(--dir-sign))); 
       transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1); 
       border-inline-end: none;
       background: var(--surface);
    }
    header#mainSidebar.open { 
       transform: translateX(0); 
       box-shadow: 0 0 40px rgba(0,0,0,0.8); 
    }
    
    .top-bar-sidebar { padding: 0 16px 16px; }
    .nav-links a, .bottom-section a { margin: 0 12px; }
    .text, .logo-text { opacity: 1; }
    
    .settings-grid { grid-template-columns: 1fr; gap: 15px; }
  }
</style>
</head><body>

<!-- Mobile Top Bar -->
<div class="mobile-top-bar">
  <div class="title-group">
    <button class="menu-toggle" onclick="toggleSidebar()">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor"><path d="M3 16h18v-2H3v2zm0-5h18v-2H3v2z"/></svg>
    </button>
    <span>AI Battery Studio <span style="font-size:12px; color:var(--muted)">▾</span></span>
  </div>
  <div></div>
</div>

<!-- Sidebar Overlay -->
<div class="sidebar-overlay" id="sidebarOverlay" onclick="toggleSidebar()"></div>

<!-- Sidebar -->
<header id="mainSidebar">
  <div class="top-bar-sidebar">
    <button class="menu-toggle" onclick="toggleSidebar()">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor"><path d="M3 16h18v-2H3v2zm0-5h18v-2H3v2z"/></svg>
    </button>
    <div class="logo-icon">✨</div>
    <div class="logo-text">AI Battery Studio</div>
  </div>
  
  <nav class="nav-links">
    <a href="/" title="Dashboard"><span class="icon">🏠</span> <span class="text" data-i18n="nav_home">داشبورد</span></a>
    <a href="/pack_builder"  title="AI Pack Builder"><span class="icon">🧠</span> <span class="text" data-i18n="nav_pack">ساخت پک</span></a>
    <a href="/triage" title="Triage Manager"><span class="icon">⚕️</span> <span class="text" data-i18n="nav_triage">تریاژ</span></a>
    <a href="/worker_manager" title="Worker Manager"><span class="icon">🧑‍🔧</span> <span class="text" data-i18n="nav_worker">مدیریت ورکر</span></a>
    <a href="/logs" title="CSV Manager"><span class="icon">📋</span> <span class="text" data-i18n="nav_logs">فایل‌ها</span></a>
  </nav>

  <div class="bottom-section">
    <a href="/settings" title="Settings" style="background:var(--surface2)"><span class="icon">⚙️</span> <span class="text" data-i18n="nav_settings">تنظیمات</span></a>
  </div>
</header>

<div class="main-content">
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
    'nav_home': 'داشبورد', 'nav_pack': 'ساخت پک', 'nav_triage': 'تریاژ', 'nav_worker': 'مدیریت ورکر', 'nav_logs': 'فایل‌ها', 'nav_settings': 'تنظیمات',
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
    'nav_home': 'Dashboard', 'nav_pack': 'Pack Builder', 'nav_triage': 'Triage', 'nav_worker': 'Worker Manager', 'nav_logs': 'Logs', 'nav_settings': 'Settings',
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

function toggleSidebar() {
  const sidebar = document.getElementById('mainSidebar');
  const overlay = document.getElementById('sidebarOverlay');
  sidebar.classList.toggle('open');
  overlay.classList.toggle('active');
}

let currentLang = 'fa'; 

function changeLang(lang) {
  currentLang = lang;
  localStorage.setItem('sysLang', lang); 
  document.documentElement.style.setProperty('--dir-sign', lang === 'fa' ? '1' : '-1');
  
  if (lang === 'fa') {
    document.body.style.direction = 'rtl';
    document.body.style.textAlign = 'right';
  } else {
    document.body.style.direction = 'ltr';
    document.body.style.textAlign = 'left';
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

#endif