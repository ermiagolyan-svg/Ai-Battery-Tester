#ifndef PAGE_SETTINGS_H
#define PAGE_SETTINGS_H

#include <Arduino.h>

String buildSettingsPage() {
  return R"rawhtml(
<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0"><title>Settings</title>
<style>
  :root { --bg: #0d0d0f; --surface: #161619; --border: #2a2a32; --accent: #00d4aa; --text: #e8e8f0; --muted: #6b6b80; }
  body { background: var(--bg); color: var(--text); font-family: system-ui, sans-serif; padding: 20px; }
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 14px; padding: 20px; max-width: 480px; margin-top:15px; margin-bottom:15px;}
  .btn { padding: 10px 20px; background: var(--accent); color: #000; border: none; border-radius: 8px; font-weight: 600; cursor: pointer; margin-top: 10px; width:100%;}
  .clear-btn { background: #161619; color: #ff6b35; border: 1px solid #ff6b35; margin: 4px; padding: 6px 12px; border-radius: 6px; cursor: pointer; display: inline-block;}
  
  .input-field { width: 100%; background: var(--bg); border: 1px solid var(--border); color: var(--text); padding: 10px; border-radius: 6px; margin-bottom: 10px; box-sizing: border-box; }
  .input-field:focus { border-color: var(--accent); outline: none; }
  .custom-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 10px; margin-top: 10px; }
  .custom-item label { font-size: 12px; color: var(--muted); display: block; margin-bottom: 4px; }
</style>
</head><body>
<a href="/" style="color:var(--accent); text-decoration:none;">← Back Dashboard</a>

<div class="card">
  <h3>📡 WiFi Network</h3>
  <p style="font-size:13px; color:var(--muted); margin: 8px 0;">شبکه وای‌فای دستگاه را تغییر دهید. (با ذخیره اطلاعات، دستگاه ریست می‌شود)</p>
  
  <button class="btn" style="background:var(--surface); color:var(--accent); border:1px solid var(--accent); margin-bottom: 10px;" onclick="scanWiFi()" id="btnScan">Scan Networks</button>
  
  <select class="input-field" id="wifiSSID" style="display:none; margin-bottom: 10px;">
    <option value="">Select a network...</option>
  </select>
  
  <input type="password" class="input-field" id="wifiPass" placeholder="Password" style="display:none;">
  <button class="btn" id="btnSaveWifi" onclick="saveWiFi()" style="display:none;">Save & Connect</button>
  <div id="resWifi" style="margin-top:10px; color:var(--accent); font-size:13px; text-align:center;"></div>
</div>

<div class="card">
  <h3>⚡ Smart Triage Profile</h3>
  <p style="font-size:13px; color:var(--muted); margin: 8px 0;">استاندارد گریددهی باتری‌ها را مشخص کنید. این تنظیمات در حافظه ذخیره می‌شوند.</p>
  
  <select class="input-field" id="triageProfile" onchange="toggleCustom()">
    <option value="0">High Drain (Power Tools, Vape)</option>
    <option value="1">Normal (Laptop, Powerbank)</option>
    <option value="2">Unknown (Mixed / Standard Default)</option>
    <option value="3">Custom (Manual Thresholds)</option>
  </select>
  
  <div id="customThresh" style="display:none; padding-top:5px; border-top:1px solid var(--border); margin-top:10px;">
     <p style="font-size:12px; color:var(--accent);">حداکثر مقاومت قابل قبول (میلی‌اهم) برای هر گرید را وارد کنید:</p>
     <div class="custom-grid">
       <div class="custom-item"><label>Grade A (mΩ)</label><input type="number" class="input-field" id="valA"></div>
       <div class="custom-item"><label>Grade B (mΩ)</label><input type="number" class="input-field" id="valB"></div>
       <div class="custom-item"><label>Grade C (mΩ)</label><input type="number" class="input-field" id="valC"></div>
     </div>
  </div>
  
  <button class="btn" onclick="saveTriage()">Save Profile</button>
  <div id="resTriage" style="margin-top:10px; color:var(--accent); font-size:13px; text-align:center;"></div>
</div>

<div class="card">
  <h3>⏰ RTC Synchronization</h3>
  <p style="font-size:13px; color:var(--muted); margin: 8px 0;">اطلاعات ساعت سیستم شما برای ثبت دقیق زمان در لاگ ذخیره می‌شود.</p>
  <button class="btn" onclick="sync()">Sync from Browser</button>
  <div id="res" style="margin-top:10px; color:var(--accent); font-size:13px; text-align:center;"></div>
</div>

<div class="card">
  <h3>📊 Wipe Storage Log</h3>
  <div>
    <button class="clear-btn" onclick="clearL(0)">Clear CH1</button>
    <button class="clear-btn" onclick="clearL(1)">Clear CH2</button>
    <button class="clear-btn" onclick="clearL(2)">Clear CH3</button>
  </div>
  <button class="clear-btn" style="background:#ff6b35; color:#000; width:100%; margin-top:10px; margin-left:0;" onclick="clearL(-1)">Wipe All Channels</button>
</div>

<script>
fetch('/getprofile').then(r=>r.json()).then(d=>{
  document.getElementById('triageProfile').value = d.p;
  document.getElementById('valA').value = d.a;
  document.getElementById('valB').value = d.b;
  document.getElementById('valC').value = d.c;
  toggleCustom();
});

function toggleCustom() {
  document.getElementById('customThresh').style.display = (document.getElementById('triageProfile').value == '3') ? 'block' : 'none';
}

function saveTriage() { 
  const p = document.getElementById('triageProfile').value;
  const a = document.getElementById('valA').value || 80;
  const b = document.getElementById('valB').value || 150;
  const c = document.getElementById('valC').value || 300;
  fetch('/setprofile', {method: 'POST', body: `p=${p}&a=${a}&b=${b}&c=${c}`})
    .then(r=>r.text()).then(t=>{ const res = document.getElementById('resTriage'); res.textContent = t; setTimeout(() => res.textContent = '', 3000); }); 
}

function scanWiFi() {
  const btn = document.getElementById('btnScan');
  btn.textContent = 'Scanning...';
  fetch('/scanwifi').then(r=>r.json()).then(d => {
    const sel = document.getElementById('wifiSSID');
    sel.innerHTML = '<option value="">Select a network...</option>';
    d.networks.forEach(nw => { sel.innerHTML += `<option value="${nw.ssid}">${nw.ssid} (${nw.rssi}dBm)</option>`; });
    sel.style.display = 'block';
    document.getElementById('wifiPass').style.display = 'block';
    document.getElementById('btnSaveWifi').style.display = 'block';
    btn.textContent = 'Scan Networks';
  }).catch(e => { btn.textContent = 'Scan Failed!'; setTimeout(() => btn.textContent = 'Scan Networks', 2000); });
}

function saveWiFi() {
  const ssid = document.getElementById('wifiSSID').value;
  const pass = document.getElementById('wifiPass').value;
  if(!ssid) return alert('شبکه‌ای انتخاب نشده است!');
  fetch('/setwifi', { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: `ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}` })
    .then(r=>r.text()).then(t=>{
       const res = document.getElementById('resWifi');
       res.textContent = t;
       setTimeout(() => res.textContent = '', 4000);
    });
}

function sync() { const epoch = Math.floor(Date.now() / 1000); fetch('/settime',{method:'POST',body:`epoch=${epoch}`}).then(r=>r.text()).then(t=>{ const res = document.getElementById('res'); res.textContent = t; setTimeout(() => res.textContent = '', 3000);}); }
function clearL(ch) { if(confirm('آیا مایل به حذف هستید؟')) { fetch('/clearlog',{method:'POST',body:String(ch)}).then(()=>alert('Done')); } }
</script></body></html>
)rawhtml";
}

#endif