#ifndef PAGE_INDEX_H
#define PAGE_INDEX_H

#include <Arduino.h>

String buildIndexPage() {
  return R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Li-ion Battery Analyzer V6.8.2</title>
<style>
  :root { --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --ok: #00e676; --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace; }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; min-height: 100vh; direction: rtl; text-align: right; }
  header { background: var(--surface); border-bottom: 1px solid var(--border); padding: 14px 20px; display: flex; align-items: center; justify-content: space-between; position: sticky; top: 0; z-index: 100; direction: ltr; }
  
  .logo { display: flex; align-items: center; gap: 10px; }
  .logo-icon { width: 32px; height: 32px; background: linear-gradient(135deg, var(--accent), var(--accent2)); border-radius: 8px; display: flex; align-items: center; justify-content: center; font-size: 16px; }
  .logo-text { font-size: 16px; font-weight: 700; text-align: left; }
  .logo-sub { font-size: 11px; color: var(--muted); text-align: left; }
  
  .header-right { display: flex; align-items: center; gap: 12px; direction: rtl; }
  .sys-time { font-family: var(--mono); font-size: 12px; color: var(--muted); background: var(--surface2); padding: 4px 10px; border-radius: 6px; border: 1px solid var(--border); direction: ltr; }
  
  .nav-links { display: flex; gap: 5px; }
  .nav-links a { text-decoration: none; color: var(--muted); font-size: 13px; padding: 5px 12px; border-radius: 6px; border: 1px solid var(--border); transition: all .15s; display: flex; align-items: center; gap: 4px; font-weight: 600;}
  .nav-links a:hover { color: var(--text); border-color: var(--accent); }
  .nav-links a.special { color: var(--accent); border-color: var(--accent); background: rgba(0,212,170,.1); }
  .nav-links a.special:hover { background: var(--accent); color: #000; }
  
  /* پنل تنظیمات سراسری */
  .global-panel { background: var(--surface); margin: 20px auto 0; max-width: 1500px; width: calc(100% - 40px); border-radius: 16px; border: 1px solid var(--border); padding: 16px 24px; display: flex; gap: 30px; flex-wrap: wrap; align-items: flex-start; }
  .panel-col { flex: 1; min-width: 280px; display: flex; flex-direction: column; gap: 8px; }
  .panel-label { font-size: 13px; color: var(--text); font-weight: 700; }
  .info-btn { cursor: pointer; color: var(--accent); background: rgba(0,212,170,0.1); border-radius: 50%; width: 22px; height: 22px; display: inline-flex; align-items: center; justify-content: center; font-size: 13px; border: 1px solid rgba(0,212,170,0.3); transition: 0.2s; }
  .info-btn:hover { background: var(--accent); color: #000; }
  .panel-select { background: #0a0a0c; color: var(--text); border: 1px solid var(--border); padding: 12px 14px; border-radius: 8px; font-size: 14px; outline: none; transition: border-color 0.2s; cursor: pointer; font-family: inherit; width: 100%; direction: rtl; }
  .panel-select:focus { border-color: var(--accent); }
  .panel-desc { font-size: 12px; color: #a1a1aa; line-height: 1.6; margin-top: 4px; }

  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(380px, 1fr)); gap: 16px; padding: 20px; max-width: 1500px; margin: 0 auto; direction: rtl;}
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 16px; overflow: hidden; display: flex; flex-direction: column; transition: border-color 0.3s;}
  
  .card-header { padding: 14px 18px 10px; display: flex; align-items: center; justify-content: space-between; border-bottom: 1px solid var(--border); direction: ltr; }
  .ch-label { display: flex; align-items: center; gap: 8px; }
  .ch-dot { width: 10px; height: 10px; border-radius: 50%; background: var(--muted); transition: all 0.3s; }
  @keyframes p { 0%,100%{opacity:1} 50%{opacity:.4} }
  .ch-name { font-weight: 700; font-size: 16px; text-align: left; }
  .ch-num { font-size: 12px; color: var(--muted); font-family: var(--mono); font-weight: bold; text-align: left; }
  .badge { font-size: 11px; font-weight: 600; padding: 3px 9px; border-radius: 20px; transition: all 0.3s; }
  
  .metrics { display: grid; grid-template-columns: 1fr 1fr; gap: 1px; background: var(--border); direction: ltr; text-align: left; }
  .metric { background: var(--surface); padding: 16px 18px; display: flex; flex-direction: column; gap: 4px; }
  .metric-label { font-size: 11px; color: var(--muted); letter-spacing: .5px; text-transform: uppercase; }
  .metric-value { font-family: var(--mono); font-size: 26px; font-weight: 700; color: var(--text); line-height: 1; }
  .metric-value.accent { color: var(--accent); }
  .metric-unit { font-size: 14px; color: var(--muted); margin-left: 2px; }
  
  .graph-wrap { padding: 12px 18px; background: var(--surface); border-top: 1px solid var(--border); direction: ltr; text-align: left; }
  .graph-header { font-size: 11px; color: var(--muted); margin-bottom: 8px; display: flex; justify-content: space-between;}
  .voltage-bar-wrap { padding: 10px 18px; background: var(--surface); border-top: 1px solid var(--border); direction: ltr; }
  .voltage-bar-row { display: flex; justify-content: space-between; margin-bottom: 4px; }
  .voltage-bar { height: 5px; background: var(--surface2); border-radius: 4px; overflow: hidden; }
  .voltage-bar-fill { height: 100%; background: linear-gradient(90deg, var(--warn), var(--accent), var(--ok)); transition: width .5s; }
  
  .card-footer { padding: 14px 18px; background: var(--surface2); border-top: 1px solid var(--border); }
  .form-row { display: flex; gap: 8px; margin-bottom: 10px; }
  
  .input-name { width: 100%; background: var(--surface); border: 1px solid var(--border); color: var(--text); padding: 12px; border-radius: 8px; font-size: 13px; outline: none; transition: border-color 0.2s; text-align: right; font-family: inherit; }
  .input-name:focus { border-color: var(--accent); }
  
  /* گرید ۵ دکمه‌ای */
  .action-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
  .action-grid .full-width { grid-column: span 2; }
  
  .btn-big { background: var(--surface); border: 1px solid var(--border); color: var(--text); height: 50px; border-radius: 10px; font-size: 13px; font-weight: 700; cursor: pointer; transition: all 0.2s; display: flex; align-items: center; justify-content: center; gap: 6px; box-shadow: 0 4px 6px rgba(0,0,0,0.15); font-family: inherit; }
  .btn-big:hover { background: #2a2a32; border-color: var(--accent); transform: translateY(-2px); box-shadow: 0 6px 12px rgba(0,212,170,0.1); }
  .btn-big:active { transform: translateY(1px); box-shadow: none; }
  
  .btn-big.active-btn { border-width: 2px; }
  .btn-big.chg.active-btn { border-color: #00ffff; color: #00ffff; background: rgba(0,255,255,0.1); box-shadow: 0 0 16px rgba(0,255,255,0.2); }
  .btn-big.d500.active-btn { border-color: #ffa500; color: #ffa500; background: rgba(255,165,0,0.1); box-shadow: 0 0 16px rgba(255,165,0,0.2); }
  .btn-big.d1000.active-btn { border-color: #ff0000; color: #ff0000; background: rgba(255,0,0,0.1); box-shadow: 0 0 16px rgba(255,0,0,0.2); }
  .btn-big.d1500.active-btn { border-color: #ff0000; color: #ff0000; background: rgba(255,0,0,0.1); box-shadow: 0 0 16px rgba(255,0,0,0.2); }
  .btn-big.trg.active-btn { border-color: #ff00ff; color: #ff00ff; background: rgba(255,0,255,0.1); box-shadow: 0 0 16px rgba(255,0,255,0.2); }

  .btn { padding: 0 12px; height: 42px; border-radius: 8px; border: none; cursor: pointer; font-size: 13px; font-weight: 700; display: flex; align-items: center; justify-content: center; flex: 1; transition: all 0.2s; font-family: inherit; }
  .btn-stop { background: rgba(255, 107, 53, 0.1); color: var(--warn); border: 1px solid var(--warn); width: 100%; }
  .btn-stop:hover { background: var(--warn); color: #fff; }
  .btn-report { background: var(--accent2); color: #fff; }
  .btn-reset { background: transparent; color: var(--muted); border: 1px solid var(--border); }
  .btn-reset:hover { border-color: var(--text); color: var(--text); }
  
  .log-container { background: #0a0a0c; border-top: 1px solid var(--border); flex-grow: 1; }
  .log-header { font-size: 12px; padding: 10px 18px; color: var(--muted); border-bottom: 1px solid #1e1e23; display: flex; justify-content: space-between; align-items: center; direction: rtl;}
  .log-btn { background: #1e1e23; border: 1px solid var(--border); color: var(--text); border-radius: 4px; padding: 2px 12px; cursor: pointer; font-size: 13px; margin-right: 4px; font-family: inherit;}
  .log-btn:hover { border-color: var(--accent); color: var(--accent); }
  
  .log-content { padding: 10px 18px; font-family: var(--mono); font-size: 13px; color: #a1a1aa; height: 160px; overflow: auto; direction: ltr; text-align: left; }
  .log-content::-webkit-scrollbar { width: 6px; height: 6px; }
  .log-content::-webkit-scrollbar-thumb { background: #2a2a32; border-radius: 4px; }
  
  .csv-table { width: 100%; border-collapse: collapse; text-align: left; }
  .csv-table th, .csv-table td { padding: 4px 12px; border-bottom: 1px solid #1e1e23; white-space: nowrap; }
  .csv-table th { color: var(--muted); font-weight: bold; position: sticky; top: 0; background: #0a0a0c; z-index: 1; }
  
  #toast { position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%); background: var(--surface2); border: 1px solid var(--accent); color: var(--text); padding: 12px 24px; border-radius: 10px; font-size: 14px; opacity: 0; transition: opacity .3s; pointer-events: none; z-index: 999; box-shadow: 0 10px 30px rgba(0,0,0,0.5); font-weight: bold; }
  #toast.show { opacity: 1; }

  /* استایل مربوط به مودال‌های پاپ‌آپ */
  .modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); z-index: 1000; align-items: center; justify-content: center; backdrop-filter: blur(5px); }
  .modal-content { background: var(--surface); padding: 24px; border-radius: 16px; border: 1px solid var(--accent); width: 90%; max-width: 420px; box-shadow: 0 10px 40px rgba(0,0,0,0.5); }
  .modal h3 { color: var(--accent); margin-bottom: 12px; font-size: 18px; display: flex; align-items: center; gap: 8px;}
  .modal p { font-size: 14px; color: var(--text); line-height: 1.6; margin-bottom: 15px;}
  .custom-grid { display: grid; grid-template-columns: 1fr; gap: 12px; margin-bottom: 20px; }
  .custom-grid div { display: flex; justify-content: space-between; align-items: center; background: #0a0a0c; padding: 10px 14px; border-radius: 8px; border: 1px solid var(--border); }
  .custom-grid label { font-size: 14px; font-weight: bold; color: var(--text); }
  .custom-grid input { width: 120px; background: var(--surface); border: 1px solid var(--border); color: var(--text); padding: 8px; border-radius: 6px; text-align: center; font-family: var(--mono); outline: none; }
  .custom-grid input:focus { border-color: var(--accent); }

  @media (max-width: 700px) {
    .logo-sub, .sys-time { display: none; }
    .nav-links a span { display: none; }
    .nav-links a { padding: 8px 12px; font-size: 16px; border: none; background: var(--surface2); }
    header { padding: 12px 15px; }
    .header-right { gap: 6px; }
    .grid { padding: 12px; }
    .global-panel { width: calc(100% - 24px); padding: 16px; gap: 16px; }
  }
</style>
</head><body>
<header>
  <div class="logo">
    <div class="logo-icon">⚡</div>
    <div>
      <div class="logo-text">Li-ion Analyzer</div>
      <div class="logo-sub">Internal Resistance & Capacity Tester</div>
    </div>
  </div>
  <div class="header-right">
    <nav class="nav-links">
      <a href="/triage" class="special" title="Triage Manager">⚕️ <span>تریاژ</span></a>
      <a href="/logs" title="CSV Manager">📋 <span>فایل‌ها</span></a>
      <a href="/settings" title="Settings">⚙️ <span>تنظیمات</span></a>
    </nav>
    <div class="sys-time" id="sysTime">--:--:--</div>
  </div>
</header>

<!-- پنل کنترل سراسری -->
<div class="global-panel">
  <div class="panel-col">
    <div style="display:flex; align-items:center; gap:10px; margin-bottom:4px;">
      <label class="panel-label">حالت کارکرد دستگاه (Mode)</label>
      <span class="info-btn" onclick="showInfo('mode')">ℹ️</span>
    </div>
    <select class="panel-select" id="globalMode" onchange="updateModeUI()">
      <option value="auto">اتوماتیک (اجرای متوالی مراحل)</option>
      <option value="manual">دستی (اجرای یک مرحله و توقف)</option>
    </select>
  </div>
  <div class="panel-col">
    <div style="display:flex; align-items:center; gap:10px; margin-bottom:4px;">
      <label class="panel-label">پروفایل تریاژ (Triage Thresholds)</label>
      <span class="info-btn" onclick="showInfo('profile')">ℹ️</span>
    </div>
    <select class="panel-select" id="triageProfile" onchange="checkProfileChange()">
      <option value="0">باتری‌های High Drain (پاور تولز / ویپ)</option>
      <option value="1">باتری‌های معمولی (لپ‌تاپ / پاوربانک)</option>
      <option value="2">استاندارد ترکیبی (پیش‌فرض)</option>
      <option value="3">کاستوم (ورود مقادیر دستی)</option>
    </select>
  </div>
</div>

<div class="grid" id="channelGrid"></div>
<div id="toast"></div>

<!-- مودال اخطار مقادیر کاستوم -->
<div id="customModal" class="modal">
  <div class="modal-content">
    <h3>⚙️ تنظیمات پروفایل کاستوم</h3>
    <p>لطفاً حداکثر مقاومت قابل قبول (میلی‌اهم) را برای هر گرید وارد کرده و تأیید کنید:</p>
    <div class="custom-grid">
       <div><label>گرید A (عالی)</label><input type="number" id="valA" placeholder="mΩ"></div>
       <div><label>گرید B (خوب)</label><input type="number" id="valB" placeholder="mΩ"></div>
       <div><label>گرید C (متوسط)</label><input type="number" id="valC" placeholder="mΩ"></div>
    </div>
    <div style="display:flex; gap:10px;">
       <button class="btn" style="background:var(--accent); width:100%;" onclick="confirmCustom()">تأیید و ذخیره</button>
       <button class="btn" style="background:#1e1e23; color:#fff;" onclick="cancelCustom()">انصراف</button>
    </div>
  </div>
</div>

<!-- مودال پاپ‌آپ راهنما (Info) -->
<div id="infoModal" class="modal">
   <div class="modal-content">
      <h3 id="infoTitle">راهنما</h3>
      <p id="infoText" style="white-space: pre-wrap;"></p>
      <button class="btn" style="background:var(--surface2); color:var(--text); border:1px solid var(--border); width:100%; margin-top:10px;" onclick="closeInfo()">متوجه شدم</button>
   </div>
</div>

<script>
const STATE = {IDLE:0, CHARGING:1, D500:2, D1000:3, D1500:4, IR:5, TRIAGE:6, DONE:7};
const CH_COLORS = ['#00d4aa','#0096ff','#ff6b35'];
let globalMode = 'auto'; 
let customConfirmed = true; 
let previousProfile = "2"; 

// راه‌اندازی اولیه و گرفتن تنظیمات ذخیره شده
fetch('/getprofile').then(r=>r.json()).then(d=>{
  document.getElementById('triageProfile').value = d.p;
  document.getElementById('valA').value = d.a;
  document.getElementById('valB').value = d.b;
  document.getElementById('valC').value = d.c;
  previousProfile = d.p;
  if(d.p == 3) customConfirmed = true; 
});

function updateModeUI() {
  globalMode = document.getElementById('globalMode').value;
  const grid = document.getElementById('channelGrid');
  Array.from(grid.children).forEach(card => card.setAttribute('data-mode', 'invalid'));
  fetchData();
}

// مدیریت تغییر پروفایل
function checkProfileChange() {
  const sel = document.getElementById('triageProfile');
  if (sel.value === '3') {
      customConfirmed = false;
      document.getElementById('customModal').style.display = 'flex';
  } else {
      previousProfile = sel.value;
      fetch('/setprofile', {method: 'POST', body: `p=${sel.value}&a=80&b=150&c=300`});
      showToast('پروفایل با موفقیت به حالت استاندارد تغییر کرد.');
  }
}

function confirmCustom() {
  const a = document.getElementById('valA').value || 80;
  const b = document.getElementById('valB').value || 150;
  const c = document.getElementById('valC').value || 300;
  fetch('/setprofile', {method: 'POST', body: `p=3&a=${a}&b=${b}&c=${c}`}).then(r=>{
      customConfirmed = true;
      previousProfile = '3';
      document.getElementById('customModal').style.display = 'none';
      showToast('مقادیر کاستوم با موفقیت ذخیره شدند.');
  });
}

function cancelCustom() {
  document.getElementById('customModal').style.display = 'none';
  document.getElementById('triageProfile').value = previousProfile;
  if (previousProfile == 3) customConfirmed = true;
}

// مدیریت پاپ‌آپ راهنما
function showInfo(type) {
    const title = document.getElementById('infoTitle');
    const text = document.getElementById('infoText');
    if (type === 'mode') {
        title.innerHTML = '⚙️ راهنمای حالت کارکرد';
        text.innerText = 'حالت اتوماتیک (Auto):\nدر این حالت با انتخاب یک گزینه، دستگاه تمام مراحل استاندارد شامل شارژ، استراحت و دشارژ را به‌صورت کاملاً هوشمند و متوالی برای استخراج ظرفیت دقیق انجام می‌دهد.\n\nحالت دستی (Manual):\nتنها همان فازی که انتخاب می‌کنید (مثلاً فقط شارژ کردن) یک‌بار اجرا شده و سپس دستگاه به طور کامل متوقف می‌شود.';
    } else {
        title.innerHTML = '⚕️ راهنمای پروفایل تریاژ';
        text.innerText = 'الگوریتم هوشمند دستگاه، باتری‌ها را بر اساس مقاومت داخلی در کلاس‌های A (عالی), B (خوب), C (متوسط) و D (ضعیف) دسته‌بندی می‌کند.\n\n- برای باتری‌های ابزارآلات (Power Tools) مقاومت باید بسیار پایین باشد.\n- می‌توانید با انتخاب گزینه "کاستوم"، سقف مجاز مقاومت برای هر گرید را به دلخواه خود وارد کنید تا دستگاه بر همان اساس تصمیم بگیرد.';
    }
    document.getElementById('infoModal').style.display = 'flex';
}
function closeInfo() { document.getElementById('infoModal').style.display = 'none'; }

function zoomLog(ch, step) {
  const el = document.getElementById(`csv-log-${ch}`);
  if(el) {
    let currentSize = parseFloat(window.getComputedStyle(el).fontSize);
    let newSize = currentSize + step;
    if(newSize >= 8 && newSize <= 24) el.style.fontSize = newSize + 'px';
  }
}

async function fetchAndDrawGraph(i) {
  try {
    const r = await fetch('/api/graph?ch=' + i);
    const data = await r.json();
    window['graphData_'+i] = data; 
    
    const canvas = document.getElementById(`graph-val-${i}`);
    if(!canvas) return;
    const ctx = canvas.getContext('2d');
    const w = canvas.width; const h = canvas.height;
    ctx.clearRect(0, 0, w, h);
    
    let ptsEl = document.getElementById(`points-val-${i}`);
    if(ptsEl) ptsEl.textContent = data.length + " pts";
    
    const minV = 2.5; const maxV = 4.3;
    const paddingLeft = 40; 
    
    ctx.fillStyle = '#6b6b80';
    ctx.font = '11px Consolas, monospace';
    ctx.textAlign = 'left';
    ctx.textBaseline = 'middle';
    
    [2.5, 3.0, 3.5, 4.0, 4.2].forEach(v => {
        let y = h - ((v - minV) / (maxV - minV)) * h;
        let textY = Math.max(6, Math.min(y, h - 6)); 
        
        ctx.beginPath();
        ctx.strokeStyle = 'rgba(42, 42, 50, 0.5)';
        ctx.lineWidth = 1;
        ctx.moveTo(paddingLeft - 5, y);
        ctx.lineTo(w, y);
        ctx.stroke();
        ctx.fillText(v.toFixed(1) + 'V', 0, textY);
    });

    if(!data || data.length < 2) return;
    
    ctx.beginPath();
    ctx.strokeStyle = CH_COLORS[i]; ctx.lineWidth = 3; ctx.lineCap = 'round'; ctx.lineJoin = 'round';
    for(let j=0; j<data.length; j++) {
      let x = paddingLeft + (j / Math.max(1, data.length - 1)) * (w - paddingLeft);
      let y = h - ((data[j] - minV) / (maxV - minV)) * h;
      if(j===0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();
  } catch(e) {}
}

async function fetchLogCSV(i) {
  try {
    const r = await fetch('/log' + (i+1));
    const txt = await r.text();
    const logBox = document.getElementById(`csv-log-${i}`);
    
    if(logBox) {
      if(txt === "Empty" || txt.trim() === "") {
        logBox.innerHTML = "No log data yet.";
      } else {
        const isScrolledToBottom = logBox.scrollHeight - logBox.clientHeight <= logBox.scrollTop + 5;
        const rows = txt.trim().split('\n');
        let tableHTML = '<table class="csv-table">';
        
        rows.forEach((row, index) => {
          const cols = row.split(',');
          tableHTML += '<tr>';
          cols.forEach((col, colIdx) => {
            if(colIdx < 9) {
              if(index === 0) tableHTML += `<th>${col.trim()}</th>`;
              else tableHTML += `<td>${col.trim()}</td>`;
            }
          });
          tableHTML += '</tr>';
        });
        tableHTML += '</table>';
        
        logBox.innerHTML = tableHTML;
        if (isScrolledToBottom) logBox.scrollTop = logBox.scrollHeight;
      }
    }
  } catch(e) {}
}

function renderCards(data) {
  const grid = document.getElementById('channelGrid');
  
  data.channels.forEach((ch, i) => {
    const id = `card-${i}`; let card = document.getElementById(id);
    
    const isResting = (ch.state === STATE.IDLE && (ch.autophase === 2 || ch.autophase === 4));
    const isRunning = (ch.state >= STATE.CHARGING && ch.state <= STATE.TRIAGE);
    const isTriage = (ch.state === STATE.TRIAGE) || (ch.state === STATE.DONE && ch.qt_grade && ch.qt_grade !== "");
    
    let sn = 'IDLE';
    let modeLabel = (ch.auto > 0) ? ' [Auto]' : '';
    
    let stateColor = '#6b6b80'; 
    if (isResting) stateColor = '#87ceeb'; 
    else if (ch.state === STATE.IR) stateColor = '#ffff00'; 
    else if (ch.state === STATE.CHARGING) stateColor = '#00ffff'; 
    else if (ch.state === STATE.D500) stateColor = '#ffa500'; 
    else if (ch.state === STATE.D1000) stateColor = '#ff0000'; 
    else if (ch.state === STATE.D1500) stateColor = '#ff0000'; 
    else if (ch.state === STATE.TRIAGE) stateColor = '#ff00ff'; 
    else if (ch.state === STATE.DONE) stateColor = '#00ff00'; 

    if(isResting) {
      if (ch.autophase === 2) sn = (ch.auto === 4) ? 'RESTING (45s)' : 'RESTING (5 MIN)';
      else sn = 'RESTING (1 MIN)';
    }
    else if(ch.state===STATE.CHARGING) sn = (ch.topup > 0 ? 'TOP-UP' : 'CHARGING') + modeLabel;
    else if(ch.state===STATE.D500) sn = 'TEST 0.5A' + modeLabel;
    else if(ch.state===STATE.D1000) sn = 'TEST 1.0A' + modeLabel;
    else if(ch.state===STATE.D1500) sn = 'TEST 1.5A' + modeLabel;
    else if(ch.state===STATE.IR) sn = 'MEASURING IR';
    else if(ch.state===STATE.TRIAGE) sn = 'SMART TRIAGE';
    else if(ch.state===STATE.DONE) sn = 'FINISHED';

    let displayElapsed = ch.elapsed;
    let elapsedStyle = "font-size:20px;";
    if (isResting) {
      let m = Math.floor(ch.rest_rem / 60);
      let s = ch.rest_rem % 60;
      displayElapsed = `-${m.toString().padStart(2,'0')}:${s.toString().padStart(2,'0')}`;
      elapsedStyle = "font-size:20px; color:#87ceeb;";
    }
    
    const vPct = Math.max(0, Math.min(100, ((parseFloat(ch.v) - 2.8) / (4.2 - 2.8)) * 100));
    const isFocused = (document.activeElement && document.activeElement.id === `name-${i}`);
    const currentTriageState = card ? card.getAttribute('data-istriage') : null;
    const currentMode = card ? card.getAttribute('data-mode') : null;
    const forceRebuild = (currentTriageState !== String(isTriage)) || (currentMode !== globalMode);

    let cardBorderColor = isRunning ? CH_COLORS[i] : (isResting ? '#87ceeb' : (ch.state===STATE.DONE ? '#00ff00' : '#2a2a32'));

    if (!card || forceRebuild || (card.getAttribute('data-state') != ch.state && !isFocused) || (isResting && card.getAttribute('data-resting') !== 'true')) {
      if (!card) { card = document.createElement('div'); card.id = id; grid.appendChild(card); }
      card.className = `card`;
      card.style.borderColor = cardBorderColor;
      card.setAttribute('data-state', ch.state);
      card.setAttribute('data-resting', isResting ? 'true' : 'false');
      card.setAttribute('data-istriage', String(isTriage));
      card.setAttribute('data-mode', globalMode);
      
      let metric2Label = isTriage ? 'Recovery' : 'Current';
      let metric2Value = isTriage ? (ch.qt_rec && parseFloat(ch.qt_rec)>0 ? parseFloat(ch.qt_rec).toFixed(1) : '---') : ch.i;
      let metric2Unit  = isTriage ? 'mV/s' : 'mA';
      
      let metric3Label = isTriage ? 'Grade' : 'Capacity';
      let metric3Value = isTriage ? (ch.qt_grade === "Testing..." ? "Testing..." : (ch.qt_grade ? ch.qt_grade.charAt(0) : "---")) : parseFloat(ch.mah).toFixed(0);
      let metric3Unit  = isTriage ? '' : 'mAh';
      
      let metric4Label = isTriage ? 'DCIR' : 'Resistance';
      let metric4Value = isTriage ? (ch.qt_dcir && parseFloat(ch.qt_dcir)>0 ? parseFloat(ch.qt_dcir).toFixed(0) : '---') : parseFloat(ch.ir).toFixed(1);
      let metric4Unit  = 'mΩ';

      let footerHtml = '';
      if (ch.state === STATE.DONE) {
        footerHtml = `<div class="form-row" style="direction:ltr;"><a href="/log${i+1}" class="btn" style="background:#333; color:#fff; text-decoration:none; display:flex; align-items:center; justify-content:center; flex:1;" download>📥 دانلود CSV</a><button class="btn btn-report" onclick="downloadGraph(${i})">📸 خروجی گراف</button><button class="btn btn-reset" onclick="resetCh(${i})">↺ ریست</button></div>`;
      } else {
        // کلاس‌های اکتیو برای رنگی شدن دکمه‌های روشن
        let bCharge = (ch.state === STATE.CHARGING) ? 'active-btn chg' : '';
        let b500 = (ch.state === STATE.D500) ? 'active-btn d500' : '';
        let b1000 = (ch.state === STATE.D1000) ? 'active-btn d1000' : '';
        let b1500 = (ch.state === STATE.D1500) ? 'active-btn d1500' : '';
        let bTrg = (ch.state === STATE.TRIAGE) ? 'active-btn trg' : '';

        // مسیر دستورات API وابسته به انتخاب مد دستی یا اتو
        let actCharge = 'charge'; 
        let act500 = (globalMode === 'auto') ? 'auto500' : 'd500';
        let act1000 = (globalMode === 'auto') ? 'auto1000' : 'd1000';
        let act1500 = (globalMode === 'auto') ? 'auto1500' : 'd1500';
        let actTrg = (globalMode === 'auto') ? 'autotriage' : 'triage';
        
        let onCharge = (ch.state === STATE.CHARGING) ? `stopTest(${i})` : `startAct(${i}, '${actCharge}')`;
        let on500 = (ch.state === STATE.D500) ? `stopTest(${i})` : `startAct(${i}, '${act500}')`;
        let on1000 = (ch.state === STATE.D1000) ? `stopTest(${i})` : `startAct(${i}, '${act1000}')`;
        let on1500 = (ch.state === STATE.D1500) ? `stopTest(${i})` : `startAct(${i}, '${act1500}')`;
        let onTrg = (ch.state === STATE.TRIAGE) ? `stopTest(${i})` : `startAct(${i}, '${actTrg}')`;

        // متون دکمه‌ها کاملا اختصاصی طبق درخواست
        let lblCharge = ch.state === STATE.CHARGING ? '⏹ توقف' : (globalMode === 'auto' ? '⚡ شارژ' : '⚡ شارژ (دستی)');
        let lbl500 = ch.state === STATE.D500 ? '⏹ توقف' : (globalMode === 'auto' ? '🔋 دشارژ 0.5 آمپر' : '🔋 0.5 آمپر (دستی)');
        let lbl1000 = ch.state === STATE.D1000 ? '⏹ توقف' : (globalMode === 'auto' ? '🔥 دشارژ 1 آمپر' : '🔥 1 آمپر (دستی)');
        let lbl1500 = ch.state === STATE.D1500 ? '⏹ توقف' : (globalMode === 'auto' ? '💥 دشارژ 1.5 آمپر' : '💥 1.5 آمپر (دستی)');
        let lblTrg = ch.state === STATE.TRIAGE ? '⏹ توقف' : (globalMode === 'auto' ? '⚕️ تریاژ خودکار' : '⚕️ تریاژ دستی');

        footerHtml = `
          ${(ch.state === STATE.IDLE && !isResting) ? `<div class="form-row"><input class="input-name" id="name-${i}" type="text" placeholder="شناسه باتری (اختیاری)"></div>` : ''}
          <div class="action-grid">
            <button class="btn-big ${bCharge}" onclick="${onCharge}">${lblCharge}</button>
            <button class="btn-big ${b500}" onclick="${on500}">${lbl500}</button>
            <button class="btn-big ${b1000}" onclick="${on1000}">${lbl1000}</button>
            <button class="btn-big ${b1500}" onclick="${on1500}">${lbl1500}</button>
            <button class="btn-big full-width ${bTrg}" onclick="${onTrg}">${lblTrg}</button>
          </div>
        `;
        
        if (isResting) {
          footerHtml += `<div class="form-row" style="margin-top:10px;"><button class="btn btn-stop" onclick="stopTest(${i})">■ توقف آنالیز (در حال استراحت)</button></div>`;
        }
      }

      card.innerHTML = `
        <div id="state-bar-${i}" style="height: 4px; width: 100%; background: ${stateColor}; box-shadow: 0 0 8px ${stateColor}; transition: background 0.3s;"></div>
        <div class="card-header">
            <div class="ch-label">
                <div class="ch-dot" id="dot-${i}" style="background: ${cardBorderColor}; box-shadow: 0 0 8px ${cardBorderColor}; ${isRunning||isResting ? 'animation: p 1.5s infinite;' : ''}"></div>
                <div><div class="ch-name" id="name-label-${i}">${ch.name || 'Channel '+(i+1)}</div><div class="ch-num" style="color:${CH_COLORS[i]}">CH${i+1} · #${i+1}-</div></div>
            </div>
            <span class="badge" style="background: rgba(0,0,0,0.3); border: 1px solid ${stateColor}; color: ${stateColor};" id="badge-val-${i}">${sn}</span>
        </div>
        <div class="metrics">
          <div class="metric"><span class="metric-label">Voltage</span><span class="metric-value ${(isRunning||isResting)?'accent':''}" id="v-val-${i}">${ch.v}<span class="metric-unit">V</span></span></div>
          <div class="metric"><span class="metric-label">${metric2Label}</span><span class="metric-value" id="i-val-${i}">${metric2Value}<span class="metric-unit">${metric2Unit}</span></span></div>
          <div class="metric"><span class="metric-label">${metric3Label}</span><span class="metric-value" style="color:${CH_COLORS[i]}" id="mah-val-${i}">${metric3Value}</span><span class="metric-unit">${metric3Unit}</span></div>
          <div class="metric"><span class="metric-label">${metric4Label}</span><span class="metric-value" style="color:${CH_COLORS[i]}" id="ir-val-${i}">${metric4Value}</span><span class="metric-unit">${metric4Unit}</span></div>
          <div class="metric"><span class="metric-label">Elapsed Time</span><span class="metric-value" style="${elapsedStyle}" id="elapsed-val-${i}">${displayElapsed}</span></div>
          <div class="metric"><span class="metric-label">V_Start</span><span class="metric-value" style="font-size:20px" id="vstart-val-${i}">${ch.vstart}V</span></div>
        </div>
        <div class="graph-wrap">
          <div class="graph-header"><span>Live Voltage Curve</span><span id="points-val-${i}">0 pts</span></div>
          <canvas id="graph-val-${i}" width="400" height="120" style="width:100%; height:110px; background:var(--surface2); border-radius:6px;"></canvas>
        </div>
        <div class="voltage-bar-wrap"><div class="voltage-bar-row"><span style="font-size:11px;color:var(--muted)">Voltage level</span><span style="font-size:11px;color:var(--muted)" id="vpct-val-${i}">${vPct.toFixed(0)}%</span></div><div class="voltage-bar"><div class="voltage-bar-fill" id="vbar-val-${i}" style="width:${vPct}%"></div></div></div>
        
        <div class="card-footer">
          ${footerHtml}
        </div>
        
        <div class="log-container">
          <div class="log-header">
            <div>
              <button class="log-btn" onclick="zoomLog(${i}, 1)">A+</button>
              <button class="log-btn" onclick="zoomLog(${i}, -1)">A-</button>
            </div>
            <span>پیش‌نمایش لاگ (CH${i+1})</span>
          </div>
          <div class="log-content" id="csv-log-${i}">Loading...</div>
        </div>
      `;
    } else {
      card.style.borderColor = cardBorderColor;
      
      let sBar = document.getElementById(`state-bar-${i}`);
      sBar.style.background = stateColor;
      sBar.style.boxShadow = `0 0 8px ${stateColor}`;
      
      let dot = document.getElementById(`dot-${i}`);
      dot.style.background = cardBorderColor;
      dot.style.boxShadow = `0 0 8px ${cardBorderColor}`;
      dot.style.animation = (isRunning || isResting) ? 'p 1.5s infinite' : 'none';

      let badge = document.getElementById(`badge-val-${i}`);
      badge.textContent = sn;
      badge.style.borderColor = stateColor;
      badge.style.color = stateColor;

      document.getElementById(`v-val-${i}`).innerHTML = `${ch.v}<span class="metric-unit">V</span>`;
      
      if(isTriage) {
          let recVal = ch.qt_rec && parseFloat(ch.qt_rec)>0 ? parseFloat(ch.qt_rec).toFixed(1) : '---';
          let grVal = ch.qt_grade === "Testing..." ? "Testing..." : (ch.qt_grade ? ch.qt_grade.charAt(0) : "---");
          let dcVal = ch.qt_dcir && parseFloat(ch.qt_dcir)>0 ? parseFloat(ch.qt_dcir).toFixed(0) : '---';
          
          document.getElementById(`i-val-${i}`).innerHTML = `${recVal}<span class="metric-unit">mV/s</span>`;
          document.getElementById(`mah-val-${i}`).innerText = grVal;
          document.getElementById(`ir-val-${i}`).innerText = dcVal;
      } else {
          document.getElementById(`i-val-${i}`).innerHTML = `${ch.i}<span class="metric-unit">mA</span>`;
          document.getElementById(`mah-val-${i}`).innerText = parseFloat(ch.mah).toFixed(0);
          document.getElementById(`ir-val-${i}`).innerText = parseFloat(ch.ir).toFixed(1);
      }
      
      const elpEl = document.getElementById(`elapsed-val-${i}`);
      elpEl.textContent = displayElapsed;
      if (isResting) elpEl.style.color = '#87ceeb'; else elpEl.style.color = '';
      
      document.getElementById(`vstart-val-${i}`).textContent = `${ch.vstart}V`;
      document.getElementById(`vpct-val-${i}`).textContent = `${vPct.toFixed(0)}%`;
      document.getElementById(`vbar-val-${i}`).style.width = `${vPct}%`;
      if (!isFocused && ch.name) document.getElementById(`name-label-${i}`).textContent = ch.name;
    }

    fetchAndDrawGraph(i);
    fetchLogCSV(i);
  });
}

function downloadGraph(i) {
  const data = window['graphData_'+i];
  if(!data || data.length === 0) {
    alert('دیتا برای رسم نمودار موجود نیست.');
    return;
  }
  
  const name = document.getElementById(`name-label-${i}`).textContent;
  const mah = document.getElementById(`mah-val-${i}`).textContent;
  const ir = document.getElementById(`ir-val-${i}`).textContent;
  const v_end = document.getElementById(`v-val-${i}`).textContent;
  
  const canvas = document.createElement('canvas');
  canvas.width = 1200;
  canvas.height = 800; 
  const ctx = canvas.getContext('2d');
  
  ctx.fillStyle = '#0d0d0f';
  ctx.fillRect(0, 0, canvas.width, canvas.height);
  
  ctx.strokeStyle = '#2a2a32';
  ctx.lineWidth = 4;
  ctx.strokeRect(20, 20, canvas.width - 40, canvas.height - 40);

  ctx.fillStyle = '#00d4aa';
  ctx.font = 'bold 38px system-ui, sans-serif';
  ctx.fillText('Battery Analyzer Report - CH' + (i+1), 50, 80);
  
  ctx.fillStyle = '#2a2a32';
  ctx.fillRect(50, 110, 1100, 2);

  ctx.fillStyle = '#e8e8f0';
  ctx.font = '30px system-ui, sans-serif';
  ctx.fillText('Battery Name: ' + name, 50, 170);
  
  let parsedMah = parseFloat(mah);
  if (mah.match(/[A-Za-z]/) || parsedMah <= 5) {
    let displayGrade = mah.match(/[A-Za-z]/) ? mah : "Done";
    ctx.fillText('Triage Status: ' + displayGrade, 50, 230);
  } else {
    ctx.fillText('Tested Capacity: ' + mah + ' mAh', 50, 230);
  }
  
  ctx.fillText('Internal Resistance: ' + ir + ' mΩ', 650, 170);
  ctx.fillText('End Voltage: ' + v_end + ' V', 650, 230);

  const gX = 80;
  const gY = 320;
  const gW = 1040;
  const gH = 420;
  
  ctx.fillStyle = '#161619';
  ctx.fillRect(gX, gY, gW, gH);
  ctx.strokeStyle = '#2a2a32';
  ctx.lineWidth = 2;
  ctx.strokeRect(gX, gY, gW, gH);
  
  ctx.fillStyle = '#6b6b80';
  ctx.font = '22px system-ui, sans-serif';
  ctx.textAlign = 'right';
  const minV = 2.5; const maxV = 4.3;
  
  for (let v = 2.5; v <= 4.3; v += 0.3) {
     let y = gY + gH - ((v - minV)/(maxV - minV)) * gH;
     ctx.fillText(v.toFixed(1) + 'V', gX - 15, y + 8);
     ctx.beginPath();
     ctx.strokeStyle = '#2a2a32';
     ctx.moveTo(gX, y);
     ctx.lineTo(gX + gW, y);
     ctx.stroke();
  }

  ctx.beginPath();
  ctx.strokeStyle = CH_COLORS[i];
  ctx.lineWidth = 6;
  ctx.lineCap = 'round';
  ctx.lineJoin = 'round';
  
  for(let j = 0; j < data.length; j++) {
    let x = gX + (j / Math.max(1, data.length - 1)) * gW;
    let y = gY + gH - ((data[j] - minV) / (maxV - minV)) * gH;
    if(j === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  }
  ctx.stroke();

  const link = document.createElement('a');
  let safeName = name.replace(/[^a-z0-9]/gi, '_');
  link.download = `Report_CH${i+1}_${safeName}.png`;
  link.href = canvas.toDataURL('image/png');
  link.click();
}

async function fetchData() { try { const r=await fetch('/data'); const d=await r.json(); renderCards(d); document.getElementById('sysTime').textContent=d.time; } catch(e){} }

async function startAct(i, act) { 
  // جلوگیری از اجرای دستگاه در صورتی که مقادیر کاستوم تأیید نشده باشد
  if (document.getElementById('triageProfile').value === '3' && !customConfirmed) {
      alert("اخطار امنیتی ⚠️\n\nشما پروفایل کاستوم را انتخاب کرده‌اید اما مقادیر آن را وارد یا تأیید نکرده‌اید. لطفاً ابتدا مقادیر را از منوی بالا تأیید کنید.");
      return;
  }
  
  const el = document.getElementById(`name-${i}`); 
  let name = el ? el.value.trim() : '';
  const r = await fetch('/action',{method:'POST',body:`ch=${i}&act=${act}&name=${encodeURIComponent(name)}`}); 
  if(r.ok) { showToast('دستور با موفقیت به دستگاه ارسال شد.'); if(el) el.value = ''; }
  fetchData(); 
}

async function stopTest(i) { const r=await fetch('/stop',{method:'POST',body:String(i)}); fetchData(); }
async function resetCh(i) { await fetch('/reset',{method:'POST',body:String(i)}); fetchData(); }
function showToast(m) { const t=document.getElementById('toast'); t.textContent=m; t.classList.add('show'); setTimeout(()=>t.classList.remove('show'),3500); }

fetchData(); setInterval(fetchData,1000);
</script></body></html>
)rawhtml";
}

#endif