#ifndef PAGE_PACK_BUILDER_H
#define PAGE_PACK_BUILDER_H

#include <Arduino.h>

const char PAGE_PACK_BUILDER_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<title>AI Smart Pack Builder</title>
<script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js"></script>
<style>
  :root { 
    --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; 
    --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --danger: #e74c3c; 
    --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace; 
    --dir-sign: 1; 
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; min-height: 100vh; min-height: 100dvh; direction: rtl; text-align: right; transition: background 0.3s ease; padding-inline-start: max(70px, env(safe-area-inset-left)); overflow-x: hidden; padding-top: env(safe-area-inset-top); padding-bottom: max(20px, env(safe-area-inset-bottom)); margin: 0; }
  
  .main-content { padding: 20px; max-width: 1100px; margin: 0 auto; width: 100%; box-sizing: border-box; }

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
  
  /* عناصر مختص صفحه Pack Builder */
  .header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); padding-bottom: 15px; margin-bottom: 20px; direction: inherit;}
  .btn-back { color: var(--accent); text-decoration: none; font-weight: bold; border: 1px solid var(--accent); padding: 6px 14px; border-radius: 6px; background: rgba(0,212,170,.1); transition: 0.2s; direction: inherit;}
  .btn-back:hover { background: var(--accent); color: #000; }
  
  .engine-card { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 20px; position: relative; overflow: hidden; margin-bottom: 25px;}
  .engine-card::before { content: ''; position: absolute; top: 0; left: 0; width: 100%; height: 4px; background: linear-gradient(90deg, var(--accent), var(--accent2)); }
  
  .engine-title { font-size: 18px; font-weight: bold; margin-bottom: 10px; display: flex; align-items: center; gap: 8px;}
  .engine-desc { font-size: 13px; color: var(--muted); margin-bottom: 20px; line-height:1.6;}
  
  .input-row { display: flex; gap: 15px; margin-bottom: 20px; align-items: flex-start; flex-wrap: wrap;}
  .input-group { display: flex; flex-direction: column; gap: 5px; flex: 1; min-width: 120px;}
  .input-group label { font-size: 12px; color: var(--muted); }
  .input-group input { background: var(--bg); border: 1px solid var(--border); color: var(--text); padding: 10px; border-radius: 8px; outline: none; width: 100%; box-sizing: border-box; font-family: var(--mono); text-align: center; direction: ltr; font-size:16px; font-weight:bold;}
  
  .btn-run { color: #fff; border: none; padding: 12px 24px; border-radius: 8px; font-weight: bold; cursor: pointer; font-size: 14px; transition: 0.2s; background: linear-gradient(135deg, var(--accent2), var(--accent)); height: 43px; display: flex; align-items: center; justify-content: center; min-width: 250px; margin-top: 21px; font-family: inherit;}
  .btn-run:hover { opacity: 0.9; transform: translateY(-2px); box-shadow: 0 5px 15px rgba(0, 212, 170, 0.3);}
  
  .alert-box { background: rgba(255, 107, 53, 0.1); border: 1px solid var(--warn); color: var(--text); padding: 15px; border-radius: 8px; margin-bottom: 20px; display: none; font-size: 13px; line-height: 1.6;}
  .alert-box strong { color: var(--warn); }
  
  #pack-result-section { display: none; animation: fadeIn 0.5s;}
  @keyframes fadeIn { from { opacity: 0; transform: translateY(10px); } to { opacity: 1; transform: translateY(0); } }
  
  .pack-blocks { display: flex; flex-direction: column; gap: 15px; margin-top: 20px;}
  .pack-block { background: var(--bg); border: 2px solid var(--border); border-radius: 8px; padding: 15px; position: relative; direction: ltr;}
  .pack-block::after { content: 'Negative Terminals Aligned Bottom'; position: absolute; bottom: -20px; left: 50%; transform: translateX(-50%); font-size: 10px; color: var(--muted); white-space: nowrap;}
  .block-header { display: flex; justify-content: space-between; margin-bottom: 10px; font-weight: bold; border-bottom: 1px solid var(--border); padding-bottom: 5px;}
  .block-cells { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 10px;}
  .cell-pill { background: var(--surface2); border: 1px solid var(--border); border-radius: 6px; padding: 8px; text-align: center; font-size: 11px;}
  
  .master-graph-container { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 15px; margin-bottom: 20px; direction: ltr; }
  .master-title { display: flex; justify-content: space-between; align-items: center; font-size: 14px; font-weight: bold; margin-bottom: 10px; color: var(--accent); }
  
  .btn-pdf { background: linear-gradient(135deg, #ff3366, #ff6b35); color: white; border: none; padding: 6px 12px; border-radius: 6px; font-weight: bold; cursor: pointer; transition: 0.2s; font-family: inherit; font-size: 12px;}
  .btn-pdf:hover { opacity: 0.9; transform: translateY(-1px); }
  
  #loader { display: none; color: var(--accent); margin-bottom: 15px; font-weight: bold; text-align: center; width: 100%;}
  
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
    
    .input-row { flex-direction: column; }
    .btn-run { width: 100%; }
    .header { flex-direction: column; gap: 10px; align-items: flex-start; }
  }

</style>
</head>
<body>

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
    <a href="/pack_builder"  title="AI Pack Builder" style="background:var(--surface2)"><span class="icon">🧠</span> <span class="text" data-i18n="nav_pack">ساخت پک</span></a>
    <a href="/triage" title="Triage Manager"><span class="icon">⚕️</span> <span class="text" data-i18n="nav_triage">تریاژ</span></a>
    <a href="/worker_manager" title="Worker Manager"><span class="icon">🧑‍🔧</span> <span class="text" data-i18n="nav_worker">مدیریت ورکر</span></a>
    <a href="/logs" title="CSV Manager"><span class="icon">📋</span> <span class="text" data-i18n="nav_logs">فایل‌ها</span></a>
  </nav>

  <div class="bottom-section">
    <a href="/settings" title="Settings"><span class="icon">⚙️</span> <span class="text" data-i18n="nav_settings">تنظیمات</span></a>
  </div>
</header>

<div class="main-content">
  <div class="header">
    <h2>🧠 <span data-i18n="title_main">AI Smart Pack Builder</span></h2>
    <a href="/" class="btn-back" id="btnBack" data-i18n="back_dash">→ بازگشت به داشبورد</a>
  </div>

  <div class="engine-card">
    <div class="engine-title"><span style="color:var(--text)" data-i18n="engine1_title">موتور هوشمند محاسبه تلورانس و ساخت پک</span></div>
    <div class="engine-desc" data-i18n="engine1_desc">سیستم به صورت خودکار ذات سلول‌های شما را تحلیل کرده و با محاسبه فیزیکی Current Hogging، امن‌ترین تلورانس و چیدمان را برای ظرفیت دلخواه استخراج می‌کند.</div>
    
    <div class="input-row">
      <div class="input-group">
        <label data-i18n="lbl_pack_s">تعداد سری (S)</label>
        <input type="number" id="target-s" value="4" min="1" max="24">
      </div>
      <div class="input-group">
        <label data-i18n="lbl_pack_p">تعداد موازی دلخواه (P)</label>
        <input type="number" id="target-p" value="8" min="1" max="50">
      </div>
      <div class="input-group">
        <label data-i18n="lbl_amps">جریان مصرفی دائم (Amps)</label>
        <input type="number" id="target-amps" value="7" min="1" max="200">
      </div>
      
      <button class="btn-run" onclick="runSmartPackBuilder()" data-i18n="btn_run">تحلیل هوشمند و مونتاژ پک 🚀</button>
    </div>
  </div>

  <div id="loader" data-i18n="loader_txt">در حال اسکن دیتابیس و اجرای بهینه‌سازی (Hill Climbing)...</div>
  
  <div id="ai-alert" class="alert-box"></div>

  <div id="pack-result-section">
    <div id="master-container" class="master-graph-container">
      <div class="master-title">
         <span data-i18n="master_title">📊 رفتار میانگین گراف تخلیه برای سلول‌های پک</span>
         <button class="btn-pdf" onclick="generatePackPDF()">📄 دانلود خروجی PDF</button>
      </div>
      <canvas id="master-canvas" width="800" height="120" style="width:100%; height:120px;"></canvas>
    </div>

    <div id="pack-stats" style="display:flex; justify-content:space-around; background:var(--surface); border:1px solid var(--border); padding:15px; border-radius:8px; font-weight:bold; margin-bottom:15px; direction:ltr; flex-wrap:wrap; gap:10px;"></div>
    <div id="pack-layout" class="pack-blocks"></div>
  </div>
</div>

<script>
const dict = {
  'fa': {
    'nav_home': 'داشبورد', 'nav_pack': 'ساخت پک', 'nav_triage': 'تریاژ', 'nav_worker': 'مدیریت ورکر', 'nav_logs': 'فایل‌ها', 'nav_settings': 'تنظیمات',  
    'back_dash': '→ بازگشت به داشبورد',
    'title_main': 'موتور هوشمند ساخت پک',
    'engine1_title': 'موتور هوشمند محاسبه تلورانس و کلاستربندی',
    'engine1_desc': 'سیستم به صورت خودکار ذات سلول‌های شما را تحلیل کرده و با استفاده از الگوریتم Hill Climbing کمترین اختلاف ظرفیت و ایمن‌ترین تقسیم جریان را تضمین می‌کند.',
    'lbl_pack_s': 'تعداد سری (S)',
    'lbl_pack_p': 'تعداد موازی (P)',
    'lbl_amps': 'جریان دائم (Amps)',
    'btn_run': 'تحلیل هوشمند و مونتاژ پک 🚀',
    'loader_txt': 'در حال اسکن لاگ‌ها و اجرای بهینه‌ساز شبکه عصبی...',
    'master_title': '📊 رفتار میانگین گراف تخلیه برای سلول‌های پک',
    'alert_no_cell': 'هیچ سلول معتبری در دیتابیس یافت نشد.',
    'alert_impossible': '❌ با سلول‌های موجود در دیتاست شما، ساخت پک {S}S{P_NEW}P با جریان {A} آمپر به هیچ‌وجه از نظر حرارتی ایمن نیست. دیتاست نیازمند باتری‌های قوی‌تر (High-Drain) یا تعداد بیشتری سلول هم‌رده است.',
    'alert_fallback_inventory': '📦 <strong>کمبود موجودی:</strong> تعداد سلول‌های دیتابیس برای ساخت {S}S{P_OLD}P کافی نبود. سیستم به صورت خودکار بر اساس موجودی، پک را به <strong>{S}S{P_NEW}P</strong> کاهش داد.',
    'alert_fallback_thermal': '⚠️ <strong>مداخله سیستم ایمنی:</strong> سلول‌های شما برای ساخت {S}S{P_OLD}P تحت جریان {A} آمپر خطر Overheat داشتند. سیستم پک ایمن را با ساختار <strong>{S}S{P_NEW}P</strong> طراحی کرد.',
    'alert_fallback_both': '📦⚠️ <strong>محدودیت موجودی و ایمنی:</strong> ابتدا به دلیل کمبود سلول، ساختار به {S}S{P_INV}P کاهش یافت. سپس برای جلوگیری از خطر حرارتی تحت جریان {A} آمپر، سیستم امن‌ترین پک ممکن را به صورت <strong>{S}S{P_NEW}P</strong> نهایی کرد.',
  },
  'en': {
    'nav_home': 'Dashboard', 'nav_pack': 'Pack Builder', 'nav_triage': 'Triage Manager', 'nav_worker': 'Worker Manager', 'nav_logs': 'CSV Manager', 'nav_settings': 'Settings',
    'back_dash': '← Dashboard',
    'title_main': 'AI Smart Pack Builder',
    'engine1_title': 'Smart Tolerance & Assembly Engine',
    'engine1_desc': 'The system runs a Hill Climbing optimization to minimize capacity deviation while strictly preventing current hogging overheating.',
    'lbl_pack_s': 'Series (S)',
    'lbl_pack_p': 'Parallel (P)',
    'lbl_amps': 'Continuous Load (Amps)',
    'btn_run': 'Smart Analyze & Build Pack 🚀',
    'loader_txt': 'Scanning logs & running optimizer...',
    'master_title': '📊 Average Discharge Profile of Assembled Pack',
    'alert_no_cell': 'No valid triaged cells found in database.',
    'alert_impossible': '❌ Based on your dataset, building a {S}S{P_NEW}P pack for {A}A continuous load is thermally unsafe. You need higher-drain cells or a larger uniform dataset.',
    'alert_fallback_inventory': '📦 <strong>Inventory Shortage:</strong> Not enough cells for {S}S{P_OLD}P. The system automatically scaled down based on availability to <strong>{S}S{P_NEW}P</strong>.',
    'alert_fallback_thermal': '⚠️ <strong>Safety Intervention:</strong> Your dataset could not safely support {S}S{P_OLD}P at {A}A without overheating. The AI automatically scaled down to the safest configuration: <strong>{S}S{P_NEW}P</strong>.',
    'alert_fallback_both': '📦⚠️ <strong>Inventory & Safety Limits:</strong> Shortage reduced target to {S}S{P_INV}P. Then, to prevent overheating at {A}A, the system finalized the safest configuration at <strong>{S}S{P_NEW}P</strong>.',
  }
};

function toggleSidebar() {
  const sidebar = document.getElementById('mainSidebar');
  const overlay = document.getElementById('sidebarOverlay');
  sidebar.classList.toggle('open');
  overlay.classList.toggle('active');
}

let currentLang = localStorage.getItem('sysLang') || 'fa';
function applyLanguage() {
  document.documentElement.style.setProperty('--dir-sign', currentLang === 'fa' ? '1' : '-1');
  if (currentLang === 'fa') {
    document.body.style.direction = 'rtl';
    document.body.style.textAlign = 'right';
  } else {
    document.body.style.direction = 'ltr';
    document.body.style.textAlign = 'left';
  }
  document.querySelectorAll('[data-i18n]').forEach(el => {
    let key = el.getAttribute('data-i18n');
    if (dict[currentLang][key]) el.innerHTML = dict[currentLang][key];
  });
}
window.onload = applyLanguage;

let allCells = [];
let globalBestBlocks = null;
let globalBestStats = null;
let globalTotalAmps = 0;

// =======================================================
// آپدیت: هماهنگی سیستم استخراج داده با تگ‌های جدید فازها
// =======================================================
async function fetchDeviceLogs() {
  let rawData = [];
  try {
    const res = await fetch('/global_log');
    const text = await res.text();
    if (text !== 'Empty' && text.trim().length > 0) {
      const lines = text.trim().split('\n');
      for (let i = 1; i < lines.length; i++) {
        const cols = lines[i].split(',');
        if (cols.length < 13) continue;
        
        let chStr = cols[0].replace('CH', '').trim();
        let c = parseInt(chStr) || 1;
        let rawId = cols[1].trim();
        let mode = cols[5] ? cols[5].trim() : "";
        
        // 🛑 سد دفاعی هوشمند (Whitelist): فقط تگ‌های استاندارد (شروع با B-) مجاز هستند
        if (!rawId.startsWith("B-")) continue;
        
        // شرط حذف تست‌های دارای خطا یا کنسل شده
        if (mode.includes("Canceled") || mode.includes("Fault")) continue;
        
        let cleanId = rawId; 
        
        let cap = parseFloat(cols[6]) || 0;
        let ir = parseFloat(cols[7]) || 0;
        let grade = cols[8] ? cols[8].trim() : "Unknown";
        let dcir = parseFloat(cols[9]) || 0;
        
        let curve = [];
        for(let j = 13; j < cols.length; j++) {
            if(cols[j] !== undefined && cols[j].trim() !== "") curve.push(parseFloat(cols[j]));
        }
        rawData.push({ cleanId, mode, cap, ir, grade, dcir, curve, ch: c });
      }
    }
  } catch(e) {}

  let grouped = {};
  rawData.forEach(row => {
      if (!grouped[row.cleanId]) grouped[row.cleanId] = { triage: null, capTests: [] };
      
      // استخراج دیتای تریاژ برای گرفتن مقاومت DCIR
      if (row.mode.includes("Triage")) {
          if (row.dcir > 0 || !grouped[row.cleanId].triage) {
              grouped[row.cleanId].triage = row;
          }
      } 
      // استخراج گراف و ظرفیت فقط از فاز خالص دشارژ
      if (row.mode.includes("[DISCHARGE]") || row.mode.includes("Manual Dischg")) {
          if (row.cap > 0) grouped[row.cleanId].capTests.push(row);
      }
  });

  allCells = [];
  for (let id in grouped) {
      let g = grouped[id];
      if (!g.triage || g.capTests.length === 0) continue;
      
      g.capTests.sort((a, b) => b.cap - a.cap);
      let bestCapTest = g.capTests[0];
      
      // شرط bestCapTest.cap < 400 طبق درخواست شما حذف شد

      allCells.push({ 
          id: id, cap: bestCapTest.cap, ir: bestCapTest.ir, dcir: g.triage.dcir, 
          grade: g.triage.grade, curve: bestCapTest.curve 
      });
  }
}

async function runSmartPackBuilder() {
  document.getElementById('loader').style.display = 'block';
  document.getElementById('pack-result-section').style.display = 'none';
  const alertBox = document.getElementById('ai-alert');
  alertBox.style.display = 'none';
  
  await fetchDeviceLogs();
  let validCells = allCells.filter(c => c.grade !== 'D' && c.grade !== 'Unknown' && c.grade !== 'N/A');

  if(validCells.length === 0) {
    alertBox.innerHTML = dict[currentLang]['alert_no_cell'];
    alertBox.style.display = 'block';
    document.getElementById('loader').style.display = 'none';
    return;
  }

  setTimeout(() => {
    let reqS = parseInt(document.getElementById('target-s').value);
    let origP = parseInt(document.getElementById('target-p').value);
    let totalAmps = parseFloat(document.getElementById('target-amps').value);

    let maxDatasetDcir = Math.max(...validCells.map(c => Math.abs(c.dcir)));
    let safeCurrent = 2.5; 
    if (maxDatasetDcir < 35) safeCurrent = 12.0;
    else if (maxDatasetDcir <= 80) safeCurrent = 5.0;

    let currentP = origP;
    let inventoryFallbackTriggered = false;
    let thermalFallbackTriggered = false;
    let inventoryMaxP = origP;

    if (validCells.length < (reqS * origP)) {
        inventoryFallbackTriggered = true;
        currentP = Math.floor(validCells.length / reqS);
        inventoryMaxP = currentP; 
    }

    let bestFinalBlocks = null;
    let bestFinalStats = null;

    while (currentP >= 1) {
        let totalNeeded = reqS * currentP;

        let pool = [...validCells].sort((a, b) => {
            if (a.cap !== b.cap) return a.cap - b.cap; 
            return b.dcir - a.dcir; 
        });
        let selectedCells = pool.slice(pool.length - totalNeeded); 

        function evaluate(testBlocks) {
            let caps = testBlocks.map(b => b.cells.reduce((sum, c) => sum + c.cap, 0));
            let capDiff = Math.max(...caps) - Math.min(...caps);
            
            let maxIOverall = 0;
            testBlocks.forEach(b => {
                let dcirs = b.cells.map(c => Math.abs(c.dcir));
                let rp = 1 / dcirs.reduce((sum, r) => sum + (1/r), 0);
                let vDrop_mV = totalAmps * rp; 
                let maxICell = vDrop_mV / Math.min(...dcirs); 
                if (maxICell > maxIOverall) maxIOverall = maxICell;
            });
            
            let penalty = maxIOverall > safeCurrent ? (maxIOverall - safeCurrent) * 10000 : 0;
            let cost = (capDiff * 100) + (maxIOverall * 500) + penalty;
            return { cost, capDiff, maxIOverall };
        }

        let foundSafeForThisP = false;
        let localBestBlocks = null;
        let localBestStats = null;

        for (let attempt = 1; attempt <= 4; attempt++) {
            let poolCopy = [...selectedCells];
            
            if (attempt === 1) poolCopy.sort((a, b) => b.cap - a.cap); 
            else if (attempt === 2) poolCopy.sort((a, b) => Math.abs(a.dcir) - Math.abs(b.dcir)); 
            else poolCopy.sort(() => Math.random() - 0.5); 

            let blocks = Array.from({length: reqS}, (_, i) => ({ id: `S${i+1}`, cells: [] }));
            let idx = 0;
            for (let lap = 0; lap < currentP; lap++) {
                let order = (lap % 2 === 0) ? blocks : [...blocks].reverse();
                order.forEach(block => { block.cells.push(poolCopy[idx++]); });
            }

            let currentEval = evaluate(blocks);
            let bestCost = currentEval.cost;
            
            const ITERATIONS = 200000;
            for (let i = 0; i < ITERATIONS; i++) {
                let b1 = Math.floor(Math.random() * reqS);
                let b2 = Math.floor(Math.random() * reqS);
                if (b1 === b2) continue;
                
                let c1 = Math.floor(Math.random() * currentP);
                let c2 = Math.floor(Math.random() * currentP);
                
                let temp = blocks[b1].cells[c1];
                blocks[b1].cells[c1] = blocks[b2].cells[c2];
                blocks[b2].cells[c2] = temp;
                
                let ev = evaluate(blocks);
                if (ev.cost < bestCost) {
                    bestCost = ev.cost;
                    currentEval = ev; 
                } else {
                    temp = blocks[b1].cells[c1];
                    blocks[b1].cells[c1] = blocks[b2].cells[c2];
                    blocks[b2].cells[c2] = temp;
                }
            }

            if (!localBestStats || currentEval.cost < localBestStats.cost) {
                localBestStats = currentEval;
                localBestBlocks = blocks.map(b => ({ id: b.id, cells: [...b.cells] }));
            }

            if (currentEval.maxIOverall <= safeCurrent) {
                foundSafeForThisP = true;
                break; 
            }
        }

        if (foundSafeForThisP || (localBestStats && localBestStats.maxIOverall <= safeCurrent)) {
            bestFinalBlocks = localBestBlocks;
            bestFinalStats = localBestStats;
            break; 
        } else {
            currentP--;
            thermalFallbackTriggered = true;
        }
    }

    if (!bestFinalBlocks || currentP === 0) {
        alertBox.innerHTML = dict[currentLang]['alert_impossible']
            .replace(/\{S\}/g, reqS)
            .replace(/\{P_NEW\}/g, inventoryMaxP) 
            .replace(/\{A\}/g, totalAmps);
        alertBox.style.display = 'block';
    } else {
        if (inventoryFallbackTriggered && thermalFallbackTriggered) {
            alertBox.innerHTML = dict[currentLang]['alert_fallback_both']
                .replace(/\{S\}/g, reqS)
                .replace(/\{P_OLD\}/g, origP)
                .replace(/\{P_INV\}/g, inventoryMaxP)
                .replace(/\{A\}/g, totalAmps)
                .replace(/\{P_NEW\}/g, currentP);
            alertBox.style.display = 'block';
        } else if (inventoryFallbackTriggered) {
            alertBox.innerHTML = dict[currentLang]['alert_fallback_inventory']
                .replace(/\{S\}/g, reqS)
                .replace(/\{P_OLD\}/g, origP)
                .replace(/\{P_NEW\}/g, currentP);
            alertBox.style.display = 'block';
        } else if (thermalFallbackTriggered) {
            alertBox.innerHTML = dict[currentLang]['alert_fallback_thermal']
                .replace(/\{S\}/g, reqS)
                .replace(/\{P_OLD\}/g, origP)
                .replace(/\{A\}/g, totalAmps)
                .replace(/\{P_NEW\}/g, currentP);
            alertBox.style.display = 'block';
        }
        
        globalBestBlocks = bestFinalBlocks;
        globalBestStats = bestFinalStats;
        globalTotalAmps = totalAmps;
        
        renderPackUI(bestFinalBlocks, bestFinalStats, totalAmps);
    }
    
    document.getElementById('loader').style.display = 'none';
  }, 100);
}

function renderPackUI(blocks, stats, totalAmps) {
    let avgPackIr = blocks.reduce((sum, b) => {
        let dcirs = b.cells.map(c => Math.abs(c.dcir));
        let rp = 1 / dcirs.reduce((s, r) => s + (1/r), 0);
        return sum + rp;
    }, 0);
    
    let statsHtml = `
      <span style="color:var(--accent2)">Total Pack IR: ${avgPackIr.toFixed(1)} mΩ</span>
      <span style="color:var(--warn)">Max Capacity Deviation: ${stats.capDiff.toFixed(1)} mAh</span>
      <span style="color:#f1c40f">Max Cell Load: ${stats.maxIOverall.toFixed(2)} A</span>
    `;
    
    let layoutHtml = "";
    blocks.forEach(block => {
        let blockTotalCap = block.cells.reduce((sum, c) => sum + c.cap, 0);
        let dcirs = block.cells.map(c => Math.abs(c.dcir));
        let rp = 1 / dcirs.reduce((s, r) => s + (1/r), 0);
        let vDrop_mV = totalAmps * rp;
        let blockMaxI = vDrop_mV / Math.min(...dcirs);

        layoutHtml += `<div class="pack-block">
            <div class="block-header">
                <span style="color:var(--accent)">Block ${block.id}</span>
                <span style="color:var(--muted)">Cap: ${blockTotalCap.toFixed(0)} mAh | Eq.IR: ${rp.toFixed(1)} mΩ | <span style="color:var(--warn)">Max I: ${blockMaxI.toFixed(2)}A</span></span>
            </div>
            <div class="block-cells">`;
        
        block.cells.sort((a,b) => a.dcir - b.dcir).forEach(cell => {
            let cellI = vDrop_mV / Math.abs(cell.dcir);
            layoutHtml += `<div class="cell-pill">
                <div style="font-weight:bold; color:var(--text)">${cell.id}</div>
                <div style="color:var(--muted); font-family:var(--mono); font-size:10px;">${cell.cap}mAh | ${Math.abs(cell.dcir).toFixed(1)}Ω</div>
                <div style="color:var(--accent); font-family:var(--mono); font-size:10px; margin-top:3px;">Load: ${cellI.toFixed(2)}A</div>
            </div>`;
        });
        
        layoutHtml += `</div></div>`;
    });
    
    document.getElementById('pack-stats').innerHTML = statsHtml;
    document.getElementById('pack-layout').innerHTML = layoutHtml;
    document.getElementById('pack-result-section').style.display = 'block';
    
    let allIncludedCells = blocks.flatMap(b => b.cells);
    drawMasterGraph(allIncludedCells);
}

function drawMasterGraph(pool) {
  const canvas = document.getElementById('master-canvas');
  if(!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width; const h = canvas.height;
  
  let minV = 5.0, maxV = 0.0;
  let avgC = [];
  let maxLength = Math.max(...pool.map(c => c.curve ? c.curve.length : 0));
  
  if (maxLength === 0) return;

  for(let i=0; i<maxLength; i++) {
    let sum = 0, count = 0;
    pool.forEach(c => { if(c.curve && c.curve[i] !== undefined) { sum += c.curve[i]; count++; }});
    let val = count > 0 ? sum/count : 0;
    avgC.push(val);
    if(val > 0 && val < minV) minV = val;
    if(val > 0 && val > maxV) maxV = val;
  }
  
  let padding = (maxV - minV) * 0.15;
  if(padding === 0) padding = 0.1;
  minV -= padding; maxV += padding;
  
  ctx.clearRect(0, 0, w, h);
  ctx.lineWidth = 2.5;
  ctx.lineCap = 'round';
  ctx.lineJoin = 'round';
  ctx.beginPath();
  ctx.strokeStyle = '#00d4aa';
  
  for(let i = 0; i < avgC.length; i++) {
    if(avgC[i] === 0) continue;
    let x = (i / Math.max(1, avgC.length - 1)) * w;
    let y = h - ((avgC[i] - minV) / (maxV - minV)) * h;
    if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
  }
  ctx.stroke();
}

function generatePackPDF() {
    if(!globalBestBlocks || globalBestBlocks.length === 0) return;
    
    try {
        const { jsPDF } = window.jspdf;
        const doc = new jsPDF();
        
        const pageWidth = doc.internal.pageSize.getWidth();
        const pageHeight = doc.internal.pageSize.getHeight();
        let y = 15;
        
        doc.setFontSize(16); doc.setFont("helvetica", "bold");
        doc.text(`AI Smart Pack Assembly Report`, 15, y);
        y += 8;
        doc.setFontSize(10); doc.setFont("helvetica", "normal"); doc.setTextColor(100);
        let sCount = globalBestBlocks.length;
        let pCount = globalBestBlocks[0].cells.length;
        doc.text(`Configuration: ${sCount}S${pCount}P | Target Load: ${globalTotalAmps}A`, 15, y);
        y += 15;

        const canvas = document.getElementById('master-canvas');
        if(canvas) {
            const imgData = canvas.toDataURL('image/png');
            doc.addImage(imgData, 'PNG', 15, y, pageWidth - 30, 25);
            y += 35;
        }

        globalBestBlocks.forEach((block, index) => {
            if (y > pageHeight - 50) { doc.addPage(); y = 15; }
            
            let blockTotalCap = block.cells.reduce((sum, c) => sum + c.cap, 0);
            let dcirs = block.cells.map(c => Math.abs(c.dcir));
            let rp = 1 / dcirs.reduce((s, r) => s + (1/r), 0);
            let vDrop_mV = globalTotalAmps * rp;
            
            doc.setFillColor(240, 240, 245);
            doc.rect(15, y, pageWidth - 30, 8, 'F');
            doc.setTextColor(0); doc.setFontSize(11); doc.setFont("helvetica", "bold");
            doc.text(`Block ${block.id}`, 18, y + 6);
            
            doc.setFontSize(9); doc.setTextColor(80);
            doc.text(`Total Cap: ${blockTotalCap.toFixed(0)} mAh | Eq.IR: ${rp.toFixed(1)} mOhm`, 50, y + 6);
            y += 12;

            doc.setFontSize(8); doc.setTextColor(120); doc.setFont("helvetica", "bold");
            doc.text("Cell ID", 20, y);
            doc.text("Cap (mAh)", 60, y);
            doc.text("DCIR (mOhm)", 100, y);
            doc.text("Cell Load (A)", 150, y);
            y += 5;

            doc.setFont("helvetica", "normal"); doc.setTextColor(0);
            block.cells.forEach(cell => {
                if (y > pageHeight - 15) { doc.addPage(); y = 15; }
                let cellI = vDrop_mV / Math.abs(cell.dcir);
                
                doc.text(cell.id, 20, y);
                doc.text(cell.cap.toString(), 60, y);
                doc.text(Math.abs(cell.dcir).toFixed(1), 100, y);
                doc.text(cellI.toFixed(2), 150, y);
                y += 5;
            });
            y += 5; 
        });

        doc.save(`Pack_Assembly_${sCount}S${pCount}P_${Date.now()}.pdf`);
    } catch (err) {
        alert("Error generating PDF: " + err);
    }
}
</script>
</body>
</html>
)rawhtml";

#endif