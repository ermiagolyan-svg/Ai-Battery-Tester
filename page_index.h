#ifndef PAGE_INDEX_H
#define PAGE_INDEX_H

#include <Arduino.h>

const char PAGE_INDEX_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">

<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<meta name="apple-mobile-web-app-title" content="Battery Studio">
<meta name="mobile-web-app-capable" content="yes">
<meta name="theme-color" content="#0d0d0f">
<link rel="apple-touch-icon" href="data:image/svg+xml,<svg xmlns=%22http://www.w3.org/2000/svg%22 viewBox=%220 0 100 100%22><text y=%22.9em%22 font-size=%2290%22>🔋</text></svg>">

<style>
  a, button { touch-action: manipulation; }
  :root { 
    --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; 
    --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --ok: #00e676; 
    --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace;
    --dir-sign: 1; 
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; min-height: 100vh; min-height: 100dvh; direction: rtl; text-align: right; transition: background 0.4s ease; padding-inline-start: max(70px, env(safe-area-inset-left)); overflow-x: hidden; padding-top: max(8px, env(safe-area-inset-top)); padding-bottom: max(20px, env(safe-area-inset-bottom)); }
  
  .sidebar-overlay { position: fixed; inset: 0; background: rgba(0,0,0,0.6); z-index: 95; opacity: 0; pointer-events: none; transition: opacity 0.3s ease; backdrop-filter: blur(2px); }
  .sidebar-overlay.active { opacity: 1; pointer-events: auto; }
  
  .mobile-top-bar { display: none; }

  header#mainSidebar { position: fixed; top: 0; bottom: 0; inset-inline-start: 0; width: 70px; background: var(--surface); border-inline-end: 1px solid var(--border); display: flex; flex-direction: column; padding-top: max(16px, env(safe-area-inset-top)); padding-bottom: max(20px, env(safe-area-inset-bottom)); z-index: 100; transition: width 0.3s cubic-bezier(0.4, 0, 0.2, 1); overflow-x: hidden; white-space: nowrap; }
  header#mainSidebar.open { width: 260px; box-shadow: 0 0 24px rgba(0,0,0,0.7); }
  
  .top-bar-sidebar { display: flex; align-items: center; padding: 0 13px 16px; gap: 16px; min-height: 50px; }
  
  .menu-toggle { width: 44px; height: 44px; display: flex; align-items: center; justify-content: center; background: transparent; border: none; color: var(--text); cursor: pointer; border-radius: 50%; flex-shrink: 0; transition: background 0.2s; }
  .menu-toggle:hover { background: rgba(255,255,255,0.05); }
  
  .logo-icon { font-size: 20px; width: 24px; height: 24px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  
  .text, .logo-text { opacity: 0; transition: opacity 0.2s ease; font-weight: 600; }
  .logo-text { font-size: 16px; }
  header#mainSidebar.open .text, header#mainSidebar.open .logo-text { opacity: 1; transition-delay: 0.1s; }
  
  .nav-links { display: flex; flex-direction: column; gap: 6px; flex: 1; margin-top: 10px; }
  .nav-links a, .bottom-section a { text-decoration: none; color: var(--text); font-size: 14px; padding: 0 12px; height: 44px; margin: 0 13px; border-radius: 50px; display: flex; align-items: center; gap: 16px; transition: background 0.2s, color 0.2s; border: none; font-weight: normal; }
  .nav-links a:hover, .bottom-section a:hover { background: rgba(255,255,255,0.05); }
  .nav-links a.special { background: rgba(0, 212, 170, 0.1); color: var(--accent); }
  .nav-links a.special:hover { background: rgba(0, 212, 170, 0.2); }
  .icon { font-size: 18px; width: 20px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  
  .bottom-section { display: flex; flex-direction: column; gap: 6px; margin-top: auto; padding-top: 16px; border-top: 1px solid var(--border); }
  .sys-time { padding: 0 24px; font-family: var(--mono); font-size: 12px; color: var(--muted); direction: ltr; text-align: left; opacity: 0; transition: opacity 0.2s ease; margin-top: 8px; }
  header#mainSidebar.open .sys-time { opacity: 0.6; transition-delay: 0.1s; }

  .global-panel { background: var(--surface); margin: 16px 20px 0; max-width: 1500px; border-radius: 16px; border: 1px solid var(--border); padding: 16px 24px; display: flex; gap: 30px; flex-wrap: wrap; align-items: flex-start; transition: background 0.4s ease, border-color 0.4s ease; }
  .panel-col { flex: 1; min-width: 240px; display: flex; flex-direction: column; gap: 8px; }
  .panel-label { font-size: 13px; color: var(--text); font-weight: 700; }
  .info-btn { cursor: pointer; color: var(--accent); background: rgba(0,212,170,0.1); border-radius: 50%; width: 22px; height: 22px; display: inline-flex; align-items: center; justify-content: center; font-size: 13px; border: 1px solid rgba(0,212,170,0.3); transition: 0.2s; }
  .info-btn:hover { background: var(--accent); color: #000; }
  
  .panel-select { background: rgba(0,0,0,0.2); color: var(--text); border: 1px solid rgba(255,255,255,0.1); padding: 12px 14px; border-radius: 8px; font-size: 14px; outline: none; transition: border-color 0.2s; cursor: pointer; font-family: inherit; width: 100%; direction: inherit; }
  .panel-select:focus { border-color: var(--accent); }

  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 16px; padding: 20px; max-width: 1540px; margin: 0 auto; direction: inherit;}
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 16px; overflow: hidden; display: flex; flex-direction: column; transition: border-color 0.4s ease, background 0.4s ease; position: relative;}
  
  .card-header { padding: 14px 18px 10px; display: flex; align-items: center; justify-content: space-between; border-bottom: 1px solid rgba(255,255,255,0.05); direction: ltr; }
  .ch-label { display: flex; align-items: center; gap: 8px; }
  .ch-dot { width: 10px; height: 10px; border-radius: 50%; background: var(--muted); transition: all 0.3s; }
  @keyframes p { 0%,100%{opacity:1} 50%{opacity:.4} }
  .ch-name { font-weight: 700; font-size: 16px; text-align: left; }
  .ch-num { font-size: 12px; color: var(--muted); font-family: var(--mono); font-weight: bold; text-align: left; }
  .badge { font-size: 11px; font-weight: 600; padding: 3px 9px; border-radius: 20px; transition: all 0.3s; }
  
  .metrics { display: grid; grid-template-columns: 1fr 1fr; gap: 1px; background: rgba(255,255,255,0.05); direction: ltr; text-align: left; border-bottom: 1px solid rgba(255,255,255,0.05); }
  .metric { background: var(--surface); padding: 16px 18px; display: flex; flex-direction: column; gap: 4px; transition: background 0.4s ease; }
  .metric-label { font-size: 11px; color: var(--muted); letter-spacing: .5px; text-transform: uppercase; }
  .metric-value { font-family: var(--mono); font-size: 26px; font-weight: 700; color: var(--text); line-height: 1; }
  .metric-value.accent { color: var(--accent); }
  .metric-unit { font-size: 14px; color: var(--muted); margin-left: 2px; }
  
  .graph-wrap { padding: 12px 18px; background: transparent; direction: ltr; text-align: left; }
  .graph-header { font-size: 11px; color: var(--muted); margin-bottom: 8px; display: flex; justify-content: space-between;}
  .voltage-bar-wrap { padding: 10px 18px; background: transparent; border-top: 1px solid rgba(255,255,255,0.05); direction: ltr; }
  .voltage-bar-row { display: flex; justify-content: space-between; margin-bottom: 6px; }
  .voltage-bar { height: 6px; background: rgba(0,0,0,0.3); border-radius: 4px; overflow: hidden; }
  .voltage-bar-fill { height: 100%; background: linear-gradient(90deg, var(--warn), var(--accent), var(--ok)); transition: width .5s; }
  
  .card-footer { padding: 14px 18px; background: transparent; border-top: 1px solid rgba(255,255,255,0.05); }
  .form-row { display: flex; gap: 8px; margin-bottom: 10px; position: relative; }
  
  .input-name { width: 100%; background: rgba(0,0,0,0.2); border: 1px solid rgba(255,255,255,0.1); color: var(--text); padding: 12px; border-radius: 8px; font-size: 13px; outline: none; transition: border-color 0.2s, box-shadow 0.2s; font-family: inherit; direction: inherit; }
  .input-name:focus { border-color: var(--accent); box-shadow: 0 0 8px rgba(0, 212, 170, 0.2); }
  
  /* --- اصلاح استایل‌های موبایل و دراپ‌داون --- */
  .dropdown-list { position: absolute; top: calc(100% - 8px); left: 0; right: 0; background: var(--surface); border: 1px solid var(--border); border-radius: 8px; max-height: 220px; overflow-y: auto; z-index: 200; display: none; box-shadow: 0 10px 30px rgba(0,0,0,0.5); direction: ltr; text-align: left; }
  .dropdown-item { padding: 10px 12px; cursor: pointer; border-bottom: 1px solid rgba(255,255,255,0.05); transition: background 0.2s; display: flex; justify-content: space-between; align-items: center; gap: 8px; flex-wrap: wrap; }
  .dropdown-item:last-child { border-bottom: none; }
  .dropdown-item:hover { background: rgba(255,255,255,0.05); }
  
  /* رنگ بندی وضعیت باتری‌ها */
  .tag-name { font-size: 13.5px; font-weight: bold; display: flex; align-items: center; gap: 5px; }
  .tag-name.ready { color: var(--ok); }
  .tag-name.ready::before { content: "●"; font-size: 10px; color: var(--ok); text-shadow: 0 0 8px rgba(0,230,118,0.8); }
  .tag-name.incomplete { color: var(--warn); }
  .tag-name.incomplete::before { content: "○"; font-size: 10px; color: var(--warn); }
  
  .triage-info { display: flex; gap: 4px; align-items: center; font-family: var(--mono); flex-wrap: wrap; justify-content: flex-end; }
  .stat-badge { background: rgba(255,255,255,0.05); color: var(--muted); font-size: 11px; padding: 0 6px; border-radius: 6px; display: inline-flex; align-items: center; height: 22px; box-sizing: border-box; }
  .grade-badge { font-size: 11px; font-weight: bold; padding: 0 8px; border-radius: 6px; color: #000; display: inline-flex; align-items: center; height: 22px; box-sizing: border-box; }
  .grade-A { background: var(--ok); } .grade-B { background: var(--accent2); } .grade-C { background: var(--warn); } .grade-D { background: #ff0000; color: #fff; } .grade-- { background: var(--muted); color: #fff; }
  
  .dropdown-list::-webkit-scrollbar { width: 6px; }
  .dropdown-list::-webkit-scrollbar-thumb { background: rgba(255,255,255,0.1); border-radius: 4px; }
  
  .action-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
  .action-grid .full-width { grid-column: span 2; }
  
  .btn-big { background: rgba(255,255,255,0.03); border: 1px solid rgba(255,255,255,0.1); color: var(--text); height: 50px; border-radius: 10px; font-size: 13px; font-weight: 700; cursor: pointer; transition: all 0.2s; display: flex; align-items: center; justify-content: center; gap: 6px; font-family: inherit; }
  .btn-big:hover { background: rgba(255,255,255,0.08); border-color: rgba(255,255,255,0.2); transform: translateY(-1px); }
  .btn-big:active { transform: translateY(1px); }
  
  .btn-big.active-btn { border-width: 2px; }
  .btn-big.chg.active-btn { border-color: #00ffff; color: #00ffff; background: rgba(0,255,255,0.1); box-shadow: 0 0 16px rgba(0,255,255,0.2); }
  .btn-big.d500.active-btn { border-color: #ffff00; color: #ffff00; background: rgba(255,255,0,0.1); box-shadow: 0 0 16px rgba(255,255,0,0.2); }
  .btn-big.d1000.active-btn { border-color: #ffa500; color: #ffa500; background: rgba(255,165,0,0.1); box-shadow: 0 0 16px rgba(255,165,0,0.2); }
  .btn-big.d1500.active-btn { border-color: #ff0000; color: #ff0000; background: rgba(255,0,0,0.1); box-shadow: 0 0 16px rgba(255,0,0,0.2); }  
  .btn-big.trg.active-btn { border-color: #ff00ff; color: #ff00ff; background: rgba(255,0,255,0.1); box-shadow: 0 0 16px rgba(255,0,255,0.2); }

  .btn { padding: 0 12px; height: 42px; border-radius: 8px; border: none; cursor: pointer; font-size: 13px; font-weight: 700; display: flex; align-items: center; justify-content: center; flex: 1; transition: all 0.2s; font-family: inherit; }
  .btn-stop { background: rgba(255, 107, 53, 0.1); color: var(--warn); border: 1px solid var(--warn); width: 100%; }
  .btn-stop:hover { background: var(--warn); color: #fff; }
  .btn-report { background: var(--accent2); color: #fff; }
  .btn-reset { background: transparent; color: var(--muted); border: 1px solid rgba(255,255,255,0.1); }
  .btn-reset:hover { border-color: var(--text); color: var(--text); }
  
  .log-container { background: rgba(0,0,0,0.2); border-top: 1px solid rgba(255,255,255,0.05); flex-grow: 1; }
  .log-header { font-size: 12px; padding: 10px 18px; color: var(--muted); border-bottom: 1px solid rgba(255,255,255,0.05); display: flex; justify-content: space-between; align-items: center; direction: inherit;}
  .log-btn { background: rgba(255,255,255,0.05); border: 1px solid rgba(255,255,255,0.1); color: var(--text); border-radius: 4px; padding: 2px 12px; cursor: pointer; font-size: 13px; margin: 0 4px; font-family: inherit;}
  .log-btn:hover { border-color: var(--accent); color: var(--accent); }
  
  .log-content { padding: 10px 18px; font-family: var(--mono); font-size: 13px; color: #a1a1aa; height: 160px; overflow: auto; direction: ltr; text-align: left; }
  .log-content::-webkit-scrollbar { width: 6px; height: 6px; }
  .log-content::-webkit-scrollbar-thumb { background: rgba(255,255,255,0.1); border-radius: 4px; }
  
  .csv-table { width: 100%; border-collapse: collapse; text-align: left; }
  .csv-table th, .csv-table td { padding: 6px 12px; border-bottom: 1px solid rgba(255,255,255,0.05); white-space: nowrap; }
  .csv-table th { color: var(--text); font-weight: bold; position: sticky; top: 0; background: var(--surface); z-index: 1; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }

  #toast { position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%); background: var(--surface2); border: 1px solid var(--accent); color: var(--text); padding: 12px 24px; border-radius: 10px; font-size: 14px; opacity: 0; transition: opacity .3s; pointer-events: none; z-index: 999; box-shadow: 0 10px 30px rgba(0,0,0,0.5); font-weight: bold; }
  #toast.show { opacity: 1; }

  .modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.8); z-index: 1000; align-items: center; justify-content: center; backdrop-filter: blur(5px); }
  .modal-content { background: var(--surface); padding: 24px; border-radius: 16px; border: 1px solid var(--accent); width: 90%; max-width: 420px; box-shadow: 0 10px 40px rgba(0,0,0,0.5); }
  .modal h3 { color: var(--accent); margin-bottom: 12px; font-size: 18px; display: flex; align-items: center; gap: 8px;}
  .modal p { font-size: 14px; color: var(--text); line-height: 1.6; margin-bottom: 15px;}

  @media (max-width: 700px) {
    body { padding-inline-start: env(safe-area-inset-left); padding-top: calc(64px + env(safe-area-inset-top)); padding-bottom: max(40px, env(safe-area-inset-bottom)); }
    
    .mobile-top-bar { display: flex; position: fixed; top: 0; left: 0; right: 0; height: calc(64px + env(safe-area-inset-top)); padding: env(safe-area-inset-top) 16px 0; background: var(--bg); align-items: center; justify-content: space-between; z-index: 90; transition: background 0.4s ease; }
    .mobile-top-bar .title-group { display: flex; align-items: center; gap: 12px; font-size: 18px; font-weight: 500; }
    
    header#mainSidebar { 
       width: 280px; 
       transform: translateX(calc(100% * var(--dir-sign))); 
       transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1), background 0.4s ease; 
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
    
    .grid { grid-template-columns: minmax(260px, 1fr); padding: 12px; gap: 12px; }
    .global-panel { margin: 12px 12px 0; padding: 14px; gap: 12px; }
  }

</style>
</head><body>

<div class="mobile-top-bar">
  <div class="title-group">
    <button class="menu-toggle" onclick="toggleSidebar()">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor"><path d="M3 16h18v-2H3v2zm0-5h18v-2H3v2z"/></svg>
    </button>
    <span>AI Battery Studio <span style="font-size:12px; color:var(--muted)">▾</span></span>
  </div>
  <div></div>
</div>

<div class="sidebar-overlay" id="sidebarOverlay" onclick="toggleSidebar()"></div>

<header id="mainSidebar">
  <div class="top-bar-sidebar">
    <button class="menu-toggle" onclick="toggleSidebar()">
      <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor"><path d="M3 16h18v-2H3v2zm0-5h18v-2H3v2z"/></svg>
    </button>
    <div class="logo-icon">✨</div>
    <div class="logo-text">AI Battery Studio</div>
  </div>
  
  <nav class="nav-links">
    <a href="/" title="Dashboard" style="background:rgba(255,255,255,0.08)"><span class="icon">🏠</span> <span class="text" data-i18n="nav_home">داشبورد</span></a>
    <a href="/pack_builder" title="AI Pack Builder"><span class="icon">🧠</span> <span class="text" data-i18n="nav_pack">ساخت پک</span></a>
    <a href="/triage" title="Triage Manager"><span class="icon">⚕️</span> <span class="text" data-i18n="nav_triage">تریاژ</span></a>
    <a href="/worker_manager" title="Worker Manager"><span class="icon">🧑‍🔧</span> <span class="text" data-i18n="nav_worker">مدیریت ورکر</span></a>
    <a href="/logs" title="CSV Manager"><span class="icon">📋</span> <span class="text" data-i18n="nav_logs">فایل‌ها</span></a>
  </nav>

  <div class="bottom-section">
    <a href="/settings" title="Settings"><span class="icon">⚙️</span> <span class="text" data-i18n="nav_settings">تنظیمات</span></a>
    <div class="sys-time" id="sysTime">--:--:--</div>
  </div>
</header>

<div class="global-panel">
  <div class="panel-col">
    <div style="display:flex; align-items:center; gap:10px; margin-bottom:4px;">
      <label class="panel-label" data-i18n="mode_lbl">حالت کارکرد دستگاه (Mode)</label>
      <span class="info-btn" onclick="showInfo('mode')">ℹ️</span>
    </div>
    <select class="panel-select" id="globalMode" onchange="updateModeUI()">
      <option id="opt_auto" value="auto">اتوماتیک (اجرای متوالی مراحل)</option>
      <option id="opt_manual" value="manual">دستی (اجرای یک مرحله و توقف)</option>
    </select>
  </div>
</div>

<div class="grid" id="channelGrid"></div>
<div id="toast"></div>

<div id="infoModal" class="modal">
   <div class="modal-content">
      <h3 id="infoTitle">راهنما</h3>
      <p id="infoText" style="white-space: pre-wrap; direction: inherit;"></p>
      <button class="btn" style="background:var(--surface2); color:var(--text); border:1px solid var(--border); width:100%; margin-top:10px;" onclick="closeInfo()" data-i18n="info_ok">متوجه شدم</button>
   </div>
</div>

<script>
const dict = {
  'fa': {
     nav_home: 'داشبورد', nav_pack: 'ساخت پک', nav_triage: 'تریاژ', nav_worker: 'مدیریت ورکر', nav_logs: 'فایل‌ها', nav_settings: 'تنظیمات',
     mode_lbl: 'حالت کارکرد دستگاه (Mode)',
     mode_auto: 'اتوماتیک (اجرای متوالی مراحل)', mode_manual: 'دستی (اجرای یک مرحله و توقف)',
     info_ok: 'متوجه شدم',
     toast_cmd: 'دستور با موفقیت به دستگاه ارسال شد.',
     err_notag_chg: 'اخطار ⚠️\n\nبرای شروع شارژ در حالت اتوماتیک، انتخاب نام باتری (تگ تریاژ) الزامی است!',
     err_notag_dis: 'اخطار ⚠️\n\nبرای شروع دیس‌شارژ در حالت اتوماتیک، انتخاب نام باتری (تگ تریاژ) الزامی است!',
     btn_stop: '⏹ توقف', btn_chg: '⚡ شارژ', btn_chg_m: '⚡ شارژ (دستی)', btn_500: '🔋 دشارژ 0.5A', btn_500_m: '🔋 0.5A (دستی)',
     btn_1000: '🔥 دشارژ 1.0A', btn_1000_m: '🔥 1.0A (دستی)', btn_1500: '💥 دشارژ 1.5A', btn_1500_m: '💥 1.5A (دستی)',
     btn_trg: '⚕️ تریاژ خودکار', btn_trg_m: '⚕️ تریاژ دستی', btn_stop_rest: '■ توقف آنالیز (در حال استراحت)',
     btn_dl: '📥 دانلود CSV', btn_graph: '📸 خروجی گراف', btn_rst: '↺ ریست',
     lbl_prev: 'پیش‌نمایش لاگ', lbl_no_data: 'هیچ لاگی یافت نشد.', plc_id: 'تگ دستی (خالی بگذارید: MAN-xxx)', sel_tag: 'تگ (تریاژ: خالی = خودکار B-xxx)',
     info_mode_t: '⚙️ راهنمای حالت کارکرد', info_mode_d: 'حالت اتوماتیک (Auto):\nدر این حالت دستگاه تمام مراحل استاندارد را متوالی اجرا می‌کند.\n\nحالت دستی (Manual):\nتنها همان فازی که انتخاب می‌کنید یک‌بار اجرا می‌شود.'
  },
  'en': {
     nav_home: 'Dashboard', nav_pack: 'Pack Builder', nav_triage: 'Triage', nav_worker: 'Worker Manager', nav_logs: 'Logs', nav_settings: 'Settings',
     mode_lbl: 'Operation Mode',
     mode_auto: 'Automatic (Sequential Phases)', mode_manual: 'Manual (Single Phase)',
     info_ok: 'Got it',
     toast_cmd: 'Command sent to device.',
     err_notag_chg: 'Warning ⚠️\n\nA Triage Tag MUST be selected to start Auto Charge!',
     err_notag_dis: 'Warning ⚠️\n\nA Triage Tag MUST be selected to start Auto Discharge!',
     btn_stop: '⏹ Stop', btn_chg: '⚡ Charge', btn_chg_m: '⚡ Charge (Manual)', btn_500: '🔋 0.5A Disch.', btn_500_m: '🔋 0.5A (Manual)',
     btn_1000: '🔥 1.0A Disch.', btn_1000_m: '🔥 1.0A (Manual)', btn_1500: '💥 1.5A Disch.', btn_1500_m: '💥 1.5A (Manual)',
     btn_trg: '⚕️ Auto Triage', btn_trg_m: '⚕️ Manual Triage', btn_stop_rest: '■ Stop Analysis (Resting)',
     btn_dl: '📥 Down. CSV', btn_graph: '📸 Export Graph', btn_rst: '↺ Reset',
     lbl_prev: 'Log Preview', lbl_no_data: 'No log data yet.', plc_id: 'Manual ID (Blank = MAN-xxx)', sel_tag: 'Tag (Blank for Auto Triage)',
     info_mode_t: '⚙️ Operation Mode Guide', info_mode_d: 'Auto Mode:\nExecutes all standard testing phases sequentially.\n\nManual Mode:\nRuns only the selected phase and stops completely.'
  }
};

let currentLang = 'fa';
let availableTags = []; 

function applyLanguage(lang) {
  currentLang = lang;
  document.documentElement.style.setProperty('--dir-sign', lang === 'fa' ? '1' : '-1');
  if(lang === 'fa') { document.body.style.direction = 'rtl'; document.body.style.textAlign = 'right'; } 
  else { document.body.style.direction = 'ltr'; document.body.style.textAlign = 'left'; }
  document.querySelectorAll('[data-i18n]').forEach(el => {
      let key = el.getAttribute('data-i18n');
      if(dict[lang][key]) el.innerText = dict[lang][key];
  });
  if(document.getElementById('opt_auto')) document.getElementById('opt_auto').innerText = dict[lang]['mode_auto'];
  if(document.getElementById('opt_manual')) document.getElementById('opt_manual').innerText = dict[lang]['mode_manual'];
  fetchData(); 
}

function toggleSidebar() {
  const sidebar = document.getElementById('mainSidebar');
  const overlay = document.getElementById('sidebarOverlay');
  sidebar.classList.toggle('open');
  overlay.classList.toggle('active');
}

window.onload = () => {
  currentLang = localStorage.getItem('sysLang') || 'fa';
  applyLanguage(currentLang);
  let savedMode = localStorage.getItem('sysMode');
  if (savedMode) document.getElementById('globalMode').value = savedMode;
  updateModeUI();
};

const STATE = {IDLE:0, CHARGING:1, D500:2, D1000:3, D1500:4, IR:5, TRIAGE:6, DONE:7};
const CH_COLORS = ['#00d4aa','#0096ff','#ff6b35'];
let globalMode = 'auto'; 

function updateModeUI() {
  globalMode = document.getElementById('globalMode').value;
  localStorage.setItem('sysMode', globalMode); 
  const grid = document.getElementById('channelGrid');
  Array.from(grid.children).forEach(card => card.setAttribute('data-mode', 'invalid'));
  if (globalMode === 'manual') {
      document.documentElement.style.setProperty('--bg', '#17100b');
      document.documentElement.style.setProperty('--surface', '#1f1611');
      document.documentElement.style.setProperty('--border', '#38261d');
  } else {
      document.documentElement.style.setProperty('--bg', '#091011');
      document.documentElement.style.setProperty('--surface', '#0f181a');
      document.documentElement.style.setProperty('--border', '#1a2b2e');
  }
  fetchData();
}

function showInfo(type) {
    const title = document.getElementById('infoTitle');
    const text = document.getElementById('infoText');
    if (type === 'mode') {
        title.innerHTML = dict[currentLang]['info_mode_t'];
        text.innerHTML = dict[currentLang]['info_mode_d'];
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
    const paddingLeft = 60; 
    ctx.fillStyle = '#6b6b80'; ctx.font = '22px Consolas, monospace'; ctx.textAlign = 'left'; ctx.textBaseline = 'middle';
    [2.5, 3.0, 3.5, 4.0, 4.2].forEach(v => {
        let y = h - ((v - minV) / (maxV - minV)) * h;
        let textY = Math.max(12, Math.min(y, h - 12)); 
        ctx.beginPath(); ctx.strokeStyle = 'rgba(255, 255, 255, 0.08)'; ctx.lineWidth = 2; ctx.moveTo(paddingLeft - 10, y); ctx.lineTo(w, y); ctx.stroke();
        ctx.fillText(v.toFixed(1) + 'V', 0, textY);
    });
    if(!data || data.length < 2) return;
    ctx.beginPath(); ctx.strokeStyle = CH_COLORS[i]; ctx.lineWidth = 6; ctx.lineCap = 'round'; ctx.lineJoin = 'round';
    for(let j=0; j<data.length; j++) {
      let x = paddingLeft + (j / Math.max(1, data.length - 1)) * (w - paddingLeft);
      let y = h - ((data[j] - minV) / (maxV - minV)) * h;
      if(j===0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();
  } catch(e) {}
}

async function fetchGlobalLogCSV() {
  try {
    const r = await fetch('/global_log');
    const txt = await r.text();
    
    let tempTags = {}; // شیء موقت برای ادغام هوشمند ردیف‌های هر تگ
    
    if(txt && txt !== "Empty" && txt.trim() !== "") {
        const rows = txt.trim().split('\n');
        
        const headers = rows[0].split(',').map(h => h.trim().toLowerCase());
        let modeCol = headers.findIndex(h => h.includes('mode') || h.includes('test'));
        let gradeCol = headers.findIndex(h => h.includes('grade'));
        let mahCol = headers.findIndex(h => h.includes('mah') || h.includes('cap'));
        let irCol = headers.findIndex(h => (h.includes('ir') || h.includes('res')) && !h.includes('dcir') && !h.includes('dir'));
        let dcirCol = headers.findIndex(h => h.includes('dcir'));

        for(let rIdx = 1; rIdx < rows.length; rIdx++) {
            const cols = rows[rIdx].split(',');
            if (cols.length > 1) {
                let tag = cols[1].trim();
                if (tag && tag !== "Battery ID" && tag !== "No-Tag-Selected" && !tag.startsWith("#CH") && !tag.startsWith("MAN-")) {
                    
                    if (!tempTags[tag]) {
                        tempTags[tag] = { id: tag, grade: "-", mah: "-", ir: "-", dcir: "-", hasValidDischarge: false };
                    }
                    
                    let mode = modeCol !== -1 && cols[modeCol] ? cols[modeCol].trim().toUpperCase() : "";
                    
                    if (gradeCol !== -1 && cols[gradeCol] && cols[gradeCol].trim() !== "N/A" && cols[gradeCol].trim() !== "-") tempTags[tag].grade = cols[gradeCol].trim();
                    if (irCol !== -1 && cols[irCol] && cols[irCol].trim() !== "N/A" && cols[irCol].trim() !== "-") tempTags[tag].ir = cols[irCol].trim();
                    if (dcirCol !== -1 && cols[dcirCol] && cols[dcirCol].trim() !== "N/A" && cols[dcirCol].trim() !== "-") tempTags[tag].dcir = cols[dcirCol].trim();
                    
                    let capVal = mahCol !== -1 && cols[mahCol] ? parseFloat(cols[mahCol]) : 0;
                    
                    // استخراج ظرفیت: فقط از فازهایی که دیس‌شارژ واقعی هستند (مانند منطق Pack Builder)
                    if ((mode.includes("DISCHARGE") || mode.includes("DISCHG")) && !mode.includes("[CHG]") && capVal > 0) {
                        let currentMah = parseFloat(tempTags[tag].mah) || 0;
                        if (capVal > currentMah) {
                            tempTags[tag].mah = capVal.toFixed(0);
                            tempTags[tag].hasValidDischarge = (capVal >= 400); // تشخیص آمادگی برای ساخت پک
                        }
                    }
                }
            }
        }
    }
    
    // تبدیل شیء به آرایه و معکوس کردن برای نمایش جدیدترین‌ها در بالای لیست
    availableTags = Object.values(tempTags).reverse();

    for(let i = 0; i < 3; i++) {
      const logBox = document.getElementById(`csv-log-${i}`);
      if(!logBox) continue;
      if(txt === "Empty" || txt.trim() === "") { logBox.innerHTML = dict[currentLang]['lbl_no_data']; continue; }
      const isScrolledToBottom = logBox.scrollHeight - logBox.clientHeight <= logBox.scrollTop + 5;
      const rows = txt.trim().split('\n');
// ساخت هدرهای اختصاصی و جدید
      let tableHTML = '<table class="csv-table"><tr>';
      tableHTML += '<th>Tag</th><th>Test Mode</th><th>Grade</th><th>IR/DCIR</th><th>Capacity</th><th>Duration</th><th>V-Start</th><th>V-End</th>';
      tableHTML += '</tr>';
      
      let hasData = false;
      for(let rIdx = 1; rIdx < rows.length; rIdx++) {
        const cols = rows[rIdx].split(',');
        // بررسی میکنیم که حتما ردیف مربوط به این کانال باشد و طول کافی داشته باشد
        if(cols.length > 12 && cols[0].trim() === `CH${i+1}`) {
          hasData = true;
          
          let tag = cols[1].trim();
          let mode = cols[5].trim();
          let cap = cols[6].trim();
          let ir = cols[7].trim();
          let grade = cols[8].trim();
          let dcir = cols[9].trim();
          let duration = cols[4].trim();
          
          // فرمت کردن ولتاژها به ۲ رقم اعشار
          let vStart = parseFloat(cols[11]).toFixed(2);
          let vEnd = parseFloat(cols[12]).toFixed(2);
          
          // ترکیب هوشمند IR و DCIR
          let ir_dcir = (dcir === "N/A" || dcir === "-" || dcir === "0" || dcir === "0.0") ? ir : `${ir} / ${dcir}`;
          
          tableHTML += `<tr>
            <td><strong style="color:var(--text)">${tag}</strong></td>
            <td>${mode}</td>
            <td>${grade}</td>
            <td>${ir_dcir}</td>
            <td style="color:var(--accent2)">${cap}</td>
            <td>${duration}</td>
            <td>${vStart}V</td>
            <td>${vEnd}V</td>
          </tr>`;
        }
      }
      tableHTML += '</table>';
      if(!hasData) logBox.innerHTML = dict[currentLang]['lbl_no_data'];
      else { logBox.innerHTML = tableHTML; if (isScrolledToBottom) logBox.scrollTop = logBox.scrollHeight; }
    }
  } catch(e) {}
}

let activeDropdown = -1;

function showDropdown(ch) {
    activeDropdown = ch;
    filterDropdown(ch, document.getElementById(`name-${ch}`).value);
}

function filterDropdown(ch, text) {
    const drop = document.getElementById(`dropdown-${ch}`);
    if(!drop) return;
    drop.innerHTML = '';
    
    const filtered = availableTags.filter(t => t.id.toLowerCase().includes(text.toLowerCase()));
    
    if(filtered.length === 0) {
        drop.innerHTML = `<div style="padding:12px; color:var(--muted); font-size:12px; text-align:center; direction:inherit;">تگی یافت نشد (وارد کردن مجاز است)</div>`;
    } else {
        filtered.forEach(tagObj => {
            const item = document.createElement('div');
            item.className = 'dropdown-item';
            
            let irText = tagObj.ir;
            if(tagObj.dcir !== "-" && tagObj.dcir !== "") {
                irText += `/${tagObj.dcir}`;
            }

            // تعیین رنگ و کلاس بر اساس داشتن دیس‌شارژ معتبر
            let tagClass = tagObj.hasValidDischarge ? "ready" : "incomplete";

            item.innerHTML = `
                <span class="tag-name ${tagClass}">${tagObj.id}</span>
                <div class="triage-info">
                  <span class="stat-badge">${tagObj.mah}mAh</span>
                  <span class="stat-badge">${irText}mΩ</span>
                  <span class="grade-badge grade-${tagObj.grade}">${tagObj.grade}</span>
                </div>
            `;
            item.onclick = () => {
                document.getElementById(`name-${ch}`).value = tagObj.id;
                drop.style.display = 'none';
                activeDropdown = -1;
            };
            drop.appendChild(item);
        });
    }
    drop.style.display = 'block';
}

document.addEventListener('click', (e) => {
    if(activeDropdown !== -1) {
        const inputEl = document.getElementById(`name-${activeDropdown}`);
        const dropEl = document.getElementById(`dropdown-${activeDropdown}`);
        if(inputEl && dropEl && !inputEl.contains(e.target) && !dropEl.contains(e.target)) {
            dropEl.style.display = 'none';
            activeDropdown = -1;
        }
    }
});

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
    else if (ch.state === STATE.D500) stateColor = '#ffff00'; 
    else if (ch.state === STATE.D1000) stateColor = '#ffa500'; 
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

    let cardBorderColor = isRunning ? CH_COLORS[i] : (isResting ? '#87ceeb' : (ch.state===STATE.DONE ? '#00ff00' : 'rgba(255,255,255,0.05)'));

    let tagBadgeHtml = ch.name ? `<span style="background:rgba(255,255,255,0.08); padding:4px 8px; border-right:1px solid rgba(255,255,255,0.1); font-family:var(--mono); font-size:12px; font-weight:normal; color:var(--text);">${ch.name}</span>` : '';
    let statusBadgeHtml = `<span style="padding:4px 8px; color:${stateColor}; font-weight:700; font-size:11px; letter-spacing:0.5px; display:flex; align-items:center;" id="badge-val-${i}">${sn}</span>`;
    let combinedBadge = `<div id="combined-badge-${i}" style="display:flex; align-items:stretch; background:rgba(0,0,0,0.3); border:1px solid ${stateColor}; border-radius:8px; overflow:hidden; transition:border-color 0.3s;">${tagBadgeHtml}${statusBadgeHtml}</div>`;

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
      
      let metric4Label = isTriage ? 'IR / DCIR' : 'Resistance';
      let irDisplay = parseFloat(ch.ir).toFixed(1);
      let dcirDisplay = (ch.qt_dcir && parseFloat(ch.qt_dcir) > 0) ? parseFloat(ch.qt_dcir).toFixed(0) : '---';
      
      let metric4Value = isTriage 
          ? `<span style="color:var(--text)">${irDisplay}</span> <span style="font-size:18px; color:var(--muted); font-weight:normal; margin:0 2px;">/</span> ${dcirDisplay}` 
          : irDisplay;
      let metric4Unit  = 'mΩ';
      
      let footerHtml = '';
      if (ch.state === STATE.DONE) {
        footerHtml = `<div class="form-row" style="direction:ltr;"><a href="/global_log" class="btn" style="background:rgba(255,255,255,0.1); color:var(--text); text-decoration:none; display:flex; align-items:center; justify-content:center; flex:1; border:1px solid rgba(255,255,255,0.1);" download="Global_Log.csv">${dict[currentLang]['btn_dl']}</a><button class="btn btn-report" onclick="downloadGraph(${i})">${dict[currentLang]['btn_graph']}</button><button class="btn btn-reset" onclick="resetCh(${i})">${dict[currentLang]['btn_rst']}</button></div>`;
      } else {
        let bCharge = (ch.state === STATE.CHARGING) ? 'active-btn chg' : '';
        let b500 = (ch.state === STATE.D500) ? 'active-btn d500' : '';
        let b1000 = (ch.state === STATE.D1000) ? 'active-btn d1000' : '';
        let b1500 = (ch.state === STATE.D1500) ? 'active-btn d1500' : '';
        let bTrg = (ch.state === STATE.TRIAGE) ? 'active-btn trg' : '';

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

        let lblCharge = ch.state === STATE.CHARGING ? dict[currentLang]['btn_stop'] : (globalMode === 'auto' ? dict[currentLang]['btn_chg'] : dict[currentLang]['btn_chg_m']);
        let lbl500 = ch.state === STATE.D500 ? dict[currentLang]['btn_stop'] : (globalMode === 'auto' ? dict[currentLang]['btn_500'] : dict[currentLang]['btn_500_m']);
        let lbl1000 = ch.state === STATE.D1000 ? dict[currentLang]['btn_stop'] : (globalMode === 'auto' ? dict[currentLang]['btn_1000'] : dict[currentLang]['btn_1000_m']);
        let lbl1500 = ch.state === STATE.D1500 ? dict[currentLang]['btn_stop'] : (globalMode === 'auto' ? dict[currentLang]['btn_1500'] : dict[currentLang]['btn_1500_m']);
        let lblTrg = ch.state === STATE.TRIAGE ? dict[currentLang]['btn_stop'] : (globalMode === 'auto' ? dict[currentLang]['btn_trg'] : dict[currentLang]['btn_trg_m']);

        let tagInputHtml = '';
        if (ch.state === STATE.IDLE && !isResting) {
            if (globalMode === 'auto') {
                tagInputHtml = `
                  <div class="form-row">
                    <input class="input-name" id="name-${i}" type="text" placeholder="${dict[currentLang]['sel_tag']}" autocomplete="off" onfocus="showDropdown(${i})" oninput="filterDropdown(${i}, this.value)">
                    <div id="dropdown-${i}" class="dropdown-list"></div>
                  </div>
                `;
            } else {
                tagInputHtml = `<div class="form-row"><input class="input-name" id="name-${i}" type="text" placeholder="${dict[currentLang]['plc_id']}"></div>`;
            }
        }

        footerHtml = `
          ${tagInputHtml}
          <div class="action-grid">
            <button class="btn-big ${bCharge}" onclick="${onCharge}">${lblCharge}</button>
            <button class="btn-big ${b500}" onclick="${on500}">${lbl500}</button>
            <button class="btn-big ${b1000}" onclick="${on1000}">${lbl1000}</button>
            <button class="btn-big ${b1500}" onclick="${on1500}">${lbl1500}</button>
            <button class="btn-big full-width ${bTrg}" onclick="${onTrg}">${lblTrg}</button>
          </div>
        `;
        
        if (isResting) {
          footerHtml += `<div class="form-row" style="margin-top:10px;"><button class="btn btn-stop" onclick="stopTest(${i})">${dict[currentLang]['btn_stop_rest']}</button></div>`;
        }
      }

      card.innerHTML = `
        <div id="state-bar-${i}" style="height: 4px; width: 100%; background: ${stateColor}; box-shadow: 0 0 8px ${stateColor}; transition: background 0.3s;"></div>
        <div class="card-header">
            <div class="ch-label">
                <div class="ch-dot" id="dot-${i}" style="background: ${cardBorderColor}; box-shadow: 0 0 8px ${cardBorderColor}; ${isRunning||isResting ? 'animation: p 1.5s infinite;' : ''}"></div>
                <div class="ch-name">Channel ${i+1}</div>
                <span id="name-label-${i}" style="display:none;">${ch.name || 'Channel '+(i+1)}</span>
            </div>
            ${combinedBadge}
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
          <canvas id="graph-val-${i}" width="800" height="240" style="width:100%; height:120px; background:rgba(0,0,0,0.2); border-radius:8px; border:1px solid rgba(255,255,255,0.05);"></canvas>
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
            <span>${dict[currentLang]['lbl_prev']} (CH${i+1})</span>
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

      let cb = document.getElementById(`combined-badge-${i}`);
      if(cb) {
          cb.style.borderColor = stateColor;
          cb.innerHTML = tagBadgeHtml + statusBadgeHtml;
      }

      document.getElementById(`v-val-${i}`).innerHTML = `${ch.v}<span class="metric-unit">V</span>`;
      
      if(isTriage) {
          let recVal = ch.qt_rec && parseFloat(ch.qt_rec)>0 ? parseFloat(ch.qt_rec).toFixed(1) : '---';
          let grVal = ch.qt_grade === "Testing..." ? "Testing..." : (ch.qt_grade ? ch.qt_grade.charAt(0) : "---");
          let dcVal = ch.qt_dcir && parseFloat(ch.qt_dcir)>0 ? parseFloat(ch.qt_dcir).toFixed(0) : '---';
          let irVal = parseFloat(ch.ir).toFixed(1);
          
          document.getElementById(`i-val-${i}`).innerHTML = `${recVal}<span class="metric-unit">mV/s</span>`;
          document.getElementById(`mah-val-${i}`).innerText = grVal;
          document.getElementById(`ir-val-${i}`).innerHTML = `<span style="color:var(--text)">${irVal}</span> <span style="font-size:18px; color:var(--muted); font-weight:normal; margin:0 2px;">/</span> ${dcVal}`;
      } else {
          document.getElementById(`i-val-${i}`).innerHTML = `${ch.i}<span class="metric-unit">mA</span>`;
          document.getElementById(`mah-val-${i}`).innerText = parseFloat(ch.mah).toFixed(0);
          document.getElementById(`ir-val-${i}`).innerHTML = parseFloat(ch.ir).toFixed(1);
      }
      
      const elpEl = document.getElementById(`elapsed-val-${i}`);
      elpEl.textContent = displayElapsed;
      if (isResting) elpEl.style.color = '#87ceeb'; else elpEl.style.color = '';
      
      document.getElementById(`vstart-val-${i}`).textContent = `${ch.vstart}V`;
      document.getElementById(`vpct-val-${i}`).textContent = `${vPct.toFixed(0)}%`;
      document.getElementById(`vbar-val-${i}`).style.width = `${vPct}%`;
      
      if (!isFocused) {
          const nameEl = document.getElementById(`name-label-${i}`);
          if (nameEl) nameEl.textContent = ch.name || 'Channel ' + (i+1);
      }
    }

    fetchAndDrawGraph(i);
  });
}

function downloadGraph(i) {
  const data = window['graphData_'+i];
  if(!data || data.length === 0) {
    alert('No data for graph / دیتا برای رسم نمودار موجود نیست.');
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

async function fetchData() { 
  try { 
    const r=await fetch('/data'); 
    const d=await r.json(); 
    renderCards(d); 
    document.getElementById('sysTime').textContent=d.time; 
  } catch(e){} 
}

let isCsvFetching = false;
async function fetchGlobalLogSafe() {
  if (isCsvFetching) return;
  isCsvFetching = true;
  await fetchGlobalLogCSV();
  isCsvFetching = false;
}

fetchData();
setInterval(fetchData, 1000);

setTimeout(() => {
  fetchGlobalLogSafe();
  setInterval(fetchGlobalLogSafe, 5000);
}, 1000);

async function startAct(i, act) { 
  const el = document.getElementById(`name-${i}`); 
  let name = el ? el.value.trim() : '';

  if (globalMode === 'auto' && act !== 'autotriage') {
      if (!name) {
          if (act === 'charge') alert(dict[currentLang]['err_notag_chg']);
          else alert(dict[currentLang]['err_notag_dis']);
          return; 
      }
  }

  const r = await fetch('/action',{method:'POST',body:`ch=${i}&act=${act}&mode=${globalMode}&name=${encodeURIComponent(name)}`}); 
  if(r.ok) { showToast(dict[currentLang]['toast_cmd']); if(el) el.value = ''; }
  fetchData(); 
}

async function stopTest(i) { const r=await fetch('/stop',{method:'POST',body:String(i)}); fetchData(); }
async function resetCh(i) { await fetch('/reset',{method:'POST',body:String(i)}); fetchData(); }
function showToast(m) { const t=document.getElementById('toast'); t.textContent=m; t.classList.add('show'); setTimeout(()=>t.classList.remove('show'),3500); }

</script></body></html>
)rawhtml";

#endif