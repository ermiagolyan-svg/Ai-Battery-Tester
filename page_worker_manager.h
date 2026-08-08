#ifndef PAGE_WORKER_MANAGER_H
#define PAGE_WORKER_MANAGER_H

#include <Arduino.h>

const char PAGE_WORKER_MANAGER_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, viewport-fit=cover">
<title>Worker Manager - Under Construction</title>
<style>
  :root { 
    --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; 
    --accent: #00d4aa; --warn: #ff6b35; --text: #e8e8f0; --muted: #6b6b80; 
    --dir-sign: 1; 
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  
  body { background: var(--bg); color: var(--text); font-family: system-ui, Tahoma, sans-serif; min-height: 100vh; min-height: 100dvh; direction: rtl; text-align: right; transition: all 0.3s ease; padding-inline-start: max(70px, env(safe-area-inset-left)); overflow-x: hidden; padding-top: env(safe-area-inset-top); padding-bottom: max(20px, env(safe-area-inset-bottom)); margin: 0; }
  
  .main-content { padding: 20px; max-width: 900px; margin: 0 auto; width: 100%; box-sizing: border-box; display: flex; flex-direction: column; align-items: center; justify-content: center; min-height: 80vh;}

  /* سایدبار و نوار موبایل */
  .sidebar-overlay { position: fixed; inset: 0; background: rgba(0,0,0,0.6); z-index: 95; opacity: 0; pointer-events: none; transition: opacity 0.3s ease; backdrop-filter: blur(2px); }
  .sidebar-overlay.active { opacity: 1; pointer-events: auto; }
  .mobile-top-bar { display: none; }

  header#mainSidebar { position: fixed; top: 0; bottom: 0; inset-inline-start: 0; width: 70px; background: var(--surface); border-inline-end: 1px solid var(--border); display: flex; flex-direction: column; padding-top: max(16px, env(safe-area-inset-top)); padding-bottom: max(20px, env(safe-area-inset-bottom)); z-index: 100; transition: width 0.3s cubic-bezier(0.4, 0, 0.2, 1); overflow-x: hidden; white-space: nowrap; }
  header#mainSidebar.open { width: 260px; box-shadow: 0 0 24px rgba(0,0,0,0.7); }
  
  .top-bar-sidebar { display: flex; align-items: center; padding: 0 13px 16px; gap: 16px; min-height: 50px; }
  .menu-toggle { width: 44px; height: 44px; display: flex; align-items: center; justify-content: center; background: transparent; border: none; color: var(--text); cursor: pointer; border-radius: 50%; flex-shrink: 0; transition: background 0.2s; }
  .menu-toggle:hover { background: var(--surface2); }
  .logo-icon { font-size: 20px; width: 24px; height: 24px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  
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
  
  /* محتوای صفحه در حال ساخت */
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 16px; padding: 40px; text-align: center; box-shadow: 0 10px 30px rgba(0,0,0,0.5); width: 100%; max-width: 600px; }
  .card h1 { color: #9b59b6; margin-bottom: 15px; font-size: 24px; }
  .card p { color: var(--muted); font-size: 15px; margin-bottom: 30px; line-height: 1.6; }
  
  .btn-back { color: var(--accent); text-decoration: none; font-weight: bold; border: 1px solid var(--accent); padding: 12px 24px; border-radius: 8px; transition: 0.2s; background: rgba(0,212,170,.1); display: inline-block; font-family: inherit; }
  .btn-back:hover { background: var(--accent); color: #000; transform: translateY(-2px); box-shadow: 0 4px 15px rgba(0,212,170,0.3); }

  @media (max-width: 700px) {
    body { padding-inline-start: env(safe-area-inset-left); padding-top: calc(64px + env(safe-area-inset-top)); padding-bottom: max(40px, env(safe-area-inset-bottom)); }
    .main-content { padding: 15px; min-height: 70vh; }
    
    .mobile-top-bar { display: flex; position: fixed; top: 0; left: 0; right: 0; height: calc(64px + env(safe-area-inset-top)); padding: env(safe-area-inset-top) 16px 0; background: var(--bg); align-items: center; justify-content: space-between; z-index: 90; }
    .mobile-top-bar .title-group { display: flex; align-items: center; gap: 12px; font-size: 18px; font-weight: 500; }
    
    header#mainSidebar { width: 280px; transform: translateX(calc(100% * var(--dir-sign))); transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1); border-inline-end: none; background: var(--surface); }
    header#mainSidebar.open { transform: translateX(0); box-shadow: 0 0 40px rgba(0,0,0,0.8); }
    
    .top-bar-sidebar { padding: 0 16px 16px; }
    .nav-links a, .bottom-section a { margin: 0 12px; }
    .text, .logo-text { opacity: 1; }
    .card h1 { font-size: 20px; }
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
    <a href="/" title="Dashboard"><span class="icon">🏠</span> <span class="text" data-i18n="nav_home">داشبورد</span></a>
    <a href="/pack_builder"  title="AI Pack Builder"><span class="icon">🧠</span> <span class="text" data-i18n="nav_pack">ساخت پک</span></a>
    <a href="/triage" title="Triage Manager"><span class="icon">⚕️</span> <span class="text" data-i18n="nav_triage">تریاژ</span></a>
    <a href="/worker_manager" title="Worker Manager" style="background:var(--surface2)"><span class="icon">🧑‍🔧</span> <span class="text" data-i18n="nav_worker">مدیریت ورکر</span></a>
    <a href="/logs" title="CSV Manager"><span class="icon">📋</span> <span class="text" data-i18n="nav_logs">فایل‌ها</span></a>
  </nav>

  <div class="bottom-section">
    <a href="/settings" title="Settings"><span class="icon">⚙️</span> <span class="text" data-i18n="nav_settings">تنظیمات</span></a>
  </div>
</header>

<div class="main-content">
  <div class="card">
    <h1 style="direction:inherit;">🚧 <span data-i18n="wip_title">بخش مدیریت ورکر در حال ساخت است</span> 🚧</h1>
    <p data-i18n="wip_desc">این صفحه به زودی با امکانات هماهنگ‌سازی چند دستگاه آماده خواهد شد.</p>
    <a href="/" class="btn-back" data-i18n="back_dash">→ بازگشت به داشبورد</a>
  </div>
</div>

<script>
const dict = {
  'fa': {
    'nav_home': 'داشبورد', 'nav_pack': 'ساخت پک', 'nav_triage': 'تریاژ', 'nav_worker': 'مدیریت ورکر', 'nav_logs': 'فایل‌ها', 'nav_settings': 'تنظیمات',
    'back_dash': '→ بازگشت به داشبورد',
    'wip_title': 'بخش مدیریت ورکر در حال ساخت است',
    'wip_desc': 'این صفحه به زودی با امکانات هماهنگ‌سازی چند دستگاه آماده خواهد شد.'
  },
  'en': {
    'nav_home': 'Dashboard', 'nav_pack': 'Pack Builder', 'nav_triage': 'Triage', 'nav_worker': 'Worker Manager', 'nav_logs': 'Logs', 'nav_settings': 'Settings',
    'back_dash': '← Back to Dashboard',
    'wip_title': 'Worker Manager Under Construction',
    'wip_desc': 'This page will be updated with multi-device orchestration soon.'
  }
};

function toggleSidebar() {
  const sidebar = document.getElementById('mainSidebar');
  const overlay = document.getElementById('sidebarOverlay');
  sidebar.classList.toggle('open');
  overlay.classList.toggle('active');
}

window.onload = () => {
  let currentLang = localStorage.getItem('sysLang') || 'fa';
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
};
</script>
</body>
</html>
)rawhtml";

#endif