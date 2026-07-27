#ifndef PAGE_INDEX_H
#define PAGE_INDEX_H

#include <Arduino.h>

String buildIndexPage() {
  return R"rawhtml(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Li-ion Battery Analyzer V6.8.2</title>
<style>
  :root { --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --ok: #00e676; --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace; }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { background: var(--bg); color: var(--text); font-family: system-ui, sans-serif; min-height: 100vh; }
  header { background: var(--surface); border-bottom: 1px solid var(--border); padding: 14px 20px; display: flex; align-items: center; justify-content: space-between; position: sticky; top: 0; z-index: 100; }
  .logo { display: flex; align-items: center; gap: 10px; }
  .logo-icon { width: 32px; height: 32px; background: linear-gradient(135deg, var(--accent), var(--accent2)); border-radius: 8px; display: flex; align-items: center; justify-content: center; font-size: 16px; }
  .logo-text { font-size: 16px; font-weight: 700; }
  .logo-sub { font-size: 11px; color: var(--muted); }
  .header-right { display: flex; align-items: center; gap: 12px; }
  .sys-time { font-family: var(--mono); font-size: 12px; color: var(--muted); background: var(--surface2); padding: 4px 10px; border-radius: 6px; border: 1px solid var(--border); }
  
  .nav-links { display: flex; gap: 5px; }
  .nav-links a { text-decoration: none; color: var(--muted); font-size: 13px; padding: 5px 12px; border-radius: 6px; border: 1px solid var(--border); transition: all .15s; display: flex; align-items: center; gap: 4px;}
  .nav-links a:hover { color: var(--text); border-color: var(--accent); }
  .nav-links a.special { color: var(--accent); font-weight: bold; border-color: var(--accent); background: rgba(0,212,170,.1); }
  .nav-links a.special:hover { background: var(--accent); color: #000; }
  
  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(340px, 1fr)); gap: 16px; padding: 20px; max-width: 1200px; margin: 0 auto; }
  .card { background: var(--surface); border: 1px solid var(--border); border-radius: 16px; overflow: hidden; display: flex; flex-direction: column; transition: border-color 0.3s;}
  
  .card-header { padding: 14px 18px 10px; display: flex; align-items: center; justify-content: space-between; border-bottom: 1px solid var(--border); }
  .ch-label { display: flex; align-items: center; gap: 8px; }
  .ch-dot { width: 10px; height: 10px; border-radius: 50%; background: var(--muted); transition: all 0.3s; }
  @keyframes p { 0%,100%{opacity:1} 50%{opacity:.4} }
  .ch-name { font-weight: 700; font-size: 16px; }
  .ch-num { font-size: 12px; color: var(--muted); font-family: var(--mono); font-weight: bold; }
  .badge { font-size: 11px; font-weight: 600; padding: 3px 9px; border-radius: 20px; transition: all 0.3s; }
  
  .metrics { display: grid; grid-template-columns: 1fr 1fr; gap: 1px; background: var(--border); }
  .metric { background: var(--surface); padding: 16px 18px; display: flex; flex-direction: column; gap: 4px; }
  .metric-label { font-size: 11px; color: var(--muted); letter-spacing: .5px; text-transform: uppercase; }
  .metric-value { font-family: var(--mono); font-size: 26px; font-weight: 700; color: var(--text); line-height: 1; }
  .metric-value.accent { color: var(--accent); }
  .metric-unit { font-size: 14px; color: var(--muted); margin-left: 2px; }
  .graph-wrap { padding: 12px 18px; background: var(--surface); border-top: 1px solid var(--border); }
  .graph-header { font-size: 11px; color: var(--muted); margin-bottom: 8px; display: flex; justify-content: space-between;}
  .voltage-bar-wrap { padding: 10px 18px; background: var(--surface); border-top: 1px solid var(--border); }
  .voltage-bar-row { display: flex; justify-content: space-between; margin-bottom: 4px; }
  .voltage-bar { height: 5px; background: var(--surface2); border-radius: 4px; overflow: hidden; }
  .voltage-bar-fill { height: 100%; background: linear-gradient(90deg, var(--warn), var(--accent), var(--ok)); transition: width .5s; }
  
  .card-footer { padding: 14px 18px; background: var(--surface2); border-top: 1px solid var(--border); }
  .form-row { display: flex; gap: 8px; margin-bottom: 8px; }
  .form-row:last-child { margin-bottom: 0; }
  
  .input-name { width: 100%; background: var(--surface); border: 1px solid var(--border); color: var(--text); padding: 10px 12px; border-radius: 8px; font-size: 13px; outline: none; transition: border-color 0.2s; }
  .input-name:focus { border-color: var(--accent); }
  
  /* گرید اختصاصی دکمه‌های کنترلی بجای دراپ‌داون */
  .action-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 6px; }
  .btn-act { background: var(--surface); border: 1px solid var(--border); color: var(--text); padding: 8px 4px; font-size: 11px; border-radius: 6px; cursor: pointer; transition: all 0.2s; font-weight: 600; display: flex; align-items: center; justify-content: center; }
  .btn-act:hover { background: #2a2a32; transform: translateY(-1px); }
  .btn-act:active { transform: translateY(1px); }
  .btn-act.chg { color: #00ffff; border-color: rgba(0,255,255,0.25); }
  .btn-act.dis { color: #ffa500; border-color: rgba(255,165,0,0.25); }
  .btn-act.dis-high { color: #ff0000; border-color: rgba(255,0,0,0.25); }
  .btn-act.auto { color: #00bfff; border-color: rgba(0,191,255,0.25); }
  .btn-act.trg { color: #ff00ff; border-color: rgba(255,0,255,0.25); }

  .btn { padding: 0 12px; height: 38px; border-radius: 8px; border: none; cursor: pointer; font-size: 13px; font-weight: 700; display: flex; align-items: center; justify-content: center; flex: 1; transition: all 0.2s; }
  .btn-stop { background: rgba(255, 107, 53, 0.1); color: var(--warn); border: 1px solid var(--warn); }
  .btn-stop:hover { background: var(--warn); color: #fff; }
  .btn-report { background: var(--accent2); color: #fff; }
  .btn-reset { background: transparent; color: var(--muted); border: 1px solid var(--border); }
  .btn-reset:hover { border-color: var(--text); color: var(--text); }
  
  .log-container { background: #0a0a0c; border-top: 1px solid var(--border); flex-grow: 1; }
  .log-header { font-size: 11px; padding: 8px 18px; color: var(--muted); border-bottom: 1px solid #1e1e23; display: flex; justify-content: space-between; align-items: center;}
  .log-btn { background: #1e1e23; border: 1px solid var(--border); color: var(--text); border-radius: 4px; padding: 2px 8px; cursor: pointer; font-size: 12px; margin-left: 4px;}
  .log-btn:hover { border-color: var(--accent); color: var(--accent); }
  
  .log-content { padding: 10px 18px; font-family: var(--mono); font-size: 13px; color: #a1a1aa; height: 160px; overflow: auto; }
  .log-content::-webkit-scrollbar { width: 6px; height: 6px; }
  .log-content::-webkit-scrollbar-thumb { background: #2a2a32; border-radius: 4px; }
  .log-content::-webkit-scrollbar-corner { background: transparent; }
  
  .csv-table { width: 100%; border-collapse: collapse; text-align: left; }
  .csv-table th, .csv-table td { padding: 4px 12px; border-bottom: 1px solid #1e1e23; white-space: nowrap; }
  .csv-table th { color: var(--muted); font-weight: bold; position: sticky; top: 0; background: #0a0a0c; z-index: 1; }
  
  #toast { position: fixed; bottom: 24px; left: 50%; transform: translateX(-50%); background: var(--surface2); border: 1px solid var(--accent); color: var(--text); padding: 10px 20px; border-radius: 10px; font-size: 13px; opacity: 0; transition: opacity .3s; pointer-events: none; z-index: 999; }
  #toast.show { opacity: 1; }

  @media (max-width: 700px) {
    .logo-sub { display: none; }
    .sys-time { display: none; }
    .nav-links a span { display: none; }
    .nav-links a { padding: 8px 10px; font-size: 16px; border: none; background: var(--surface2); }
    header { padding: 12px 15px; }
    .header-right { gap: 6px; }
    .grid { padding: 12px; }
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
    <div class="sys-time" id="sysTime">--:--:--</div>
    <nav class="nav-links">
      <a href="/triage" class="special" title="Triage Manager">⚕️ <span>Triage</span></a>
      <a href="/logs" title="CSV Manager">📋 <span>CSV</span></a>
      <a href="/settings" title="Settings">⚙️ <span>Settings</span></a>
    </nav>
  </div>
</header>

<div class="grid" id="channelGrid"></div>

<div id="toast"></div>
<script>
const STATE = {IDLE:0, CHARGING:1, D500:2, D1000:3, D1500:4, IR:5, TRIAGE:6, DONE:7};
const CH_COLORS = ['#00d4aa','#0096ff','#ff6b35'];

function zoomLog(ch, step) {
  const el = document.getElementById(`csv-log-${ch}`);
  if(el) {
    let currentSize = parseFloat(window.getComputedStyle(el).fontSize);
    let newSize = currentSize + step;
    if(newSize >= 8 && newSize <= 24) { 
      el.style.fontSize = newSize + 'px';
    }
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
    const paddingLeft = 40; // فضا برای رسم ولتاژها
    
    // رسم خطوط و اعداد ولتاژ (محور عمودی)
    ctx.fillStyle = '#6b6b80';
    ctx.font = '11px Consolas, monospace';
    ctx.textAlign = 'left';
    ctx.textBaseline = 'middle';
    
    [2.5, 3.0, 3.5, 4.0, 4.2].forEach(v => {
        let y = h - ((v - minV) / (maxV - minV)) * h;
        let textY = Math.max(6, Math.min(y, h - 6)); // جلوگیری از بیرون زدن متن
        
        ctx.beginPath();
        ctx.strokeStyle = 'rgba(42, 42, 50, 0.5)';
        ctx.lineWidth = 1;
        ctx.moveTo(paddingLeft - 5, y);
        ctx.lineTo(w, y);
        ctx.stroke();

        ctx.fillText(v.toFixed(1) + 'V', 0, textY);
    });

    if(!data || data.length < 2) return;
    
    // رسم نمودار اصلی
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
    
    // تعریف رنگ‌های وضعیت کاملا منطبق با TFT فیزیکی دستگاه
    let stateColor = '#6b6b80'; // STBY
    if (isResting) stateColor = '#87ceeb'; // TFT_SKYBLUE
    else if (ch.state === STATE.IR) stateColor = '#ffff00'; // TFT_YELLOW
    else if (ch.state === STATE.CHARGING) stateColor = '#00ffff'; // TFT_CYAN
    else if (ch.state === STATE.D500) stateColor = '#ffa500'; // TFT_ORANGE
    else if (ch.state === STATE.D1000) stateColor = '#ff0000'; // TFT_RED
    else if (ch.state === STATE.D1500) stateColor = '#800000'; // TFT_MAROON
    else if (ch.state === STATE.TRIAGE) stateColor = '#ff00ff'; // TFT_MAGENTA
    else if (ch.state === STATE.DONE) stateColor = '#00ff00'; // TFT_GREEN

    if(isResting) {
      if (ch.autophase === 2) sn = (ch.auto === 4) ? 'RESTING (45s)' : 'RESTING (5 MIN)';
      else sn = 'RESTING (1 MIN)';
    }
    else if(ch.state===STATE.CHARGING) {
        // تغییر لیبل وضعیت بر اساس متغیر topup
        sn = (ch.topup > 0 ? 'TOP-UP' : 'CHARGING') + modeLabel;
    }
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
    const forceRebuild = (currentTriageState !== String(isTriage));

    // رنگ بورد کارت کاملا اختصاصی هر کانال
    let cardBorderColor = isRunning ? CH_COLORS[i] : (isResting ? '#87ceeb' : (ch.state===STATE.DONE ? '#00ff00' : '#2a2a32'));

    if (!card || forceRebuild || (card.getAttribute('data-state') != ch.state && !isFocused) || (isResting && card.getAttribute('data-resting') !== 'true')) {
      if (!card) { card = document.createElement('div'); card.id = id; grid.appendChild(card); }
      card.className = `card`;
      card.style.borderColor = cardBorderColor;
      card.setAttribute('data-state', ch.state);
      card.setAttribute('data-resting', isResting ? 'true' : 'false');
      card.setAttribute('data-istriage', String(isTriage));
      
      let metric2Label = isTriage ? 'Recovery' : 'Current';
      let metric2Value = isTriage ? (ch.qt_rec && parseFloat(ch.qt_rec)>0 ? parseFloat(ch.qt_rec).toFixed(1) : '---') : ch.i;
      let metric2Unit  = isTriage ? 'mV/s' : 'mA';
      
      let metric3Label = isTriage ? 'Grade' : 'Capacity';
      let metric3Value = isTriage ? (ch.qt_grade === "Testing..." ? "Testing..." : (ch.qt_grade ? ch.qt_grade.charAt(0) : "---")) : parseFloat(ch.mah).toFixed(0);
      let metric3Unit  = isTriage ? '' : 'mAh';
      
      let metric4Label = isTriage ? 'DCIR' : 'Resistance';
      let metric4Value = isTriage ? (ch.qt_dcir && parseFloat(ch.qt_dcir)>0 ? parseFloat(ch.qt_dcir).toFixed(0) : '---') : parseFloat(ch.ir).toFixed(1);
      let metric4Unit  = 'mΩ';

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
          ${(ch.state===STATE.IDLE && !isResting)?`
          <div class="form-row" style="margin-bottom: 12px;">
            <input class="input-name" id="name-${i}" type="text" placeholder="Battery ID (Blank for Auto-Naming)">
          </div>
          <div class="action-grid">
            <button class="btn-act chg" onclick="startAct(${i}, 'charge')">Charge</button>
            <button class="btn-act dis" onclick="startAct(${i}, 'd500')">D-0.5A</button>
            <button class="btn-act auto" onclick="startAct(${i}, 'auto500')">Auto-0.5A</button>
            
            <button class="btn-act trg" onclick="startAct(${i}, 'triage')">Triage</button>
            <button class="btn-act dis-high" onclick="startAct(${i}, 'd1000')">D-1.0A</button>
            <button class="btn-act auto" onclick="startAct(${i}, 'auto1000')">Auto-1.0A</button>
            
            <button class="btn-act trg" onclick="startAct(${i}, 'autotriage')">A-Triage</button>
            <button class="btn-act dis-high" onclick="startAct(${i}, 'd1500')">D-1.5A</button>
            <button class="btn-act auto" onclick="startAct(${i}, 'auto1500')">Auto-1.5A</button>
          </div>
          `:''}
          ${(isRunning || isResting)?`<div class="form-row"><button class="btn btn-stop" onclick="stopTest(${i})">■ Stop Analyzer</button></div>`:''}
          ${ch.state===STATE.DONE?`<div class="form-row"><a href="/log${i+1}" class="btn" style="background:#333; color:#fff; text-decoration:none; display:flex; align-items:center; justify-content:center; flex:1;" download>📥 CSV</a><button class="btn btn-report" onclick="downloadGraph(${i})">📸 PNG Report</button><button class="btn btn-reset" onclick="resetCh(${i})">↺ Reset</button></div>`:''}
        </div>
        
        <div class="log-container">
          <div class="log-header">
            <span>Log Preview (log_ch${i+1}.csv)</span>
            <div>
              <button class="log-btn" onclick="zoomLog(${i}, 1)">A+</button>
              <button class="log-btn" onclick="zoomLog(${i}, -1)">A-</button>
            </div>
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
  const el = document.getElementById(`name-${i}`); 
  let name = el ? el.value.trim() : '';
  const r = await fetch('/action',{method:'POST',body:`ch=${i}&act=${act}&name=${encodeURIComponent(name)}`}); 
  if(r.ok) { showToast('Action initialized successfully'); if(el) el.value = ''; }
  fetchData(); 
}

async function stopTest(i) { const r=await fetch('/stop',{method:'POST',body:String(i)}); fetchData(); }
async function resetCh(i) { await fetch('/reset',{method:'POST',body:String(i)}); fetchData(); }
function showToast(m) { const t=document.getElementById('toast'); t.textContent=m; t.classList.add('show'); setTimeout(()=>t.classList.remove('show'),3000); }
fetchData(); setInterval(fetchData,1000);
</script></body></html>
)rawhtml";
}

#endif