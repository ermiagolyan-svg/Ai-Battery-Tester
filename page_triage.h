#ifndef PAGE_TRIAGE_H
#define PAGE_TRIAGE_H

#include <Arduino.h>

String buildTriagePage() {
  return R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>AI Smart Pack Builder</title>
<style>
  :root { --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --danger: #e74c3c; --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace; }
  body { background: var(--bg); color: var(--text); font-family: system-ui, sans-serif; padding: 20px; max-width: 1100px; margin: 0 auto; direction: rtl; text-align: right; transition: all 0.3s ease; }
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
  
  /* Pack Builder Styles */
  #pack-result-section { display: none; animation: fadeIn 0.5s;}
  @keyframes fadeIn { from { opacity: 0; transform: translateY(10px); } to { opacity: 1; transform: translateY(0); } }
  
  .pack-blocks { display: flex; flex-direction: column; gap: 15px; margin-top: 20px;}
  .pack-block { background: var(--bg); border: 2px solid var(--border); border-radius: 8px; padding: 15px; position: relative; direction: ltr;}
  .pack-block::after { content: 'Negative Terminals Aligned Bottom'; position: absolute; bottom: -20px; left: 50%; transform: translateX(-50%); font-size: 10px; color: var(--muted); white-space: nowrap;}
  .block-header { display: flex; justify-content: space-between; margin-bottom: 10px; font-weight: bold; border-bottom: 1px solid var(--border); padding-bottom: 5px;}
  .block-cells { display: grid; grid-template-columns: repeat(auto-fit, minmax(140px, 1fr)); gap: 10px;}
  .cell-pill { background: var(--surface2); border: 1px solid var(--border); border-radius: 6px; padding: 8px; text-align: center; font-size: 11px;}
  
  .master-graph-container { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 15px; margin-bottom: 20px; direction: ltr; }
  .master-title { font-size: 14px; font-weight: bold; margin-bottom: 10px; color: var(--accent); text-align: center;}
  
  #loader { display: none; color: var(--accent); margin-bottom: 15px; font-weight: bold; text-align: center; width: 100%;}
</style>
</head>
<body>
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
      <div class="master-title" data-i18n="master_title">📊 رفتار میانگین گراف تخلیه برای سلول‌های پک</div>
      <canvas id="master-canvas" width="800" height="120" style="width:100%; height:120px;"></canvas>
    </div>

    <div id="pack-stats" style="display:flex; justify-content:space-around; background:var(--surface); border:1px solid var(--border); padding:15px; border-radius:8px; font-weight:bold; margin-bottom:15px; direction:ltr;"></div>
    <div id="pack-layout" class="pack-blocks"></div>
  </div>

<script>
const dict = {
  'fa': {
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



let currentLang = localStorage.getItem('sysLang') || 'fa';
function applyLanguage() {
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

async function fetchDeviceLogs() {
  let rawData = [];
  for (let c = 1; c <= 3; c++) {
    try {
      const res = await fetch('/log' + c);
      const text = await res.text();
      if (text !== 'Empty' && text.trim().length > 0) {
        const lines = text.trim().split('\n');
        for (let i = 1; i < lines.length; i++) {
          const cols = lines[i].split(',');
          if (cols.length < 12) continue;
          
          let rawId = cols[0].trim();
          let cleanId = rawId.replace(/^#/, ''); 
          let mode = cols[4] ? cols[4].trim() : "";
          let cap = parseFloat(cols[5]) || 0;
          let ir = parseFloat(cols[6]) || 0;
          let grade = cols[7] ? cols[7].trim() : "Unknown";
          let dcir = parseFloat(cols[8]) || 0;
          
          let curve = [];
          for(let j = 12; j < cols.length; j++) {
              if(cols[j] !== undefined && cols[j].trim() !== "") curve.push(parseFloat(cols[j]));
          }
          rawData.push({ cleanId, mode, cap, ir, grade, dcir, curve, ch: c });
        }
      }
    } catch(e) {}
  }

  let grouped = {};
  rawData.forEach(row => {
      if (!grouped[row.cleanId]) grouped[row.cleanId] = { triage: null, capTests: [] };
      if (row.mode.includes("Triage")) grouped[row.cleanId].triage = row;
      else if (row.cap > 0) grouped[row.cleanId].capTests.push(row);
  });

  allCells = [];
  for (let id in grouped) {
      let g = grouped[id];
      if (!g.triage || g.capTests.length === 0) continue;
      g.capTests.sort((a, b) => b.cap - a.cap);
      let bestCapTest = g.capTests[0];
      if (bestCapTest.cap < 400) continue;

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

    // ۰. بررسی پیش‌دستانه موجودی دیتابیس (Inventory Check)
    if (validCells.length < (reqS * origP)) {
        inventoryFallbackTriggered = true;
        currentP = Math.floor(validCells.length / reqS);
        inventoryMaxP = currentP; // ثبت بالاترین P ممکن از نظر فیزیکی
    }

    let bestFinalBlocks = null;
    let bestFinalStats = null;

    while (currentP >= 1) {
        let totalNeeded = reqS * currentP;

        // ۱. حذف ضعیف‌ترین سلول‌ها برای رسیدن دقیق به تعداد مورد نیاز
        let pool = [...validCells].sort((a, b) => {
            if (a.cap !== b.cap) return a.cap - b.cap; // کمترین ظرفیت‌ها بالا
            return b.dcir - a.dcir; // بیشترین مقاومت‌ها بالا
        });
        let selectedCells = pool.slice(pool.length - totalNeeded); 

        // ۲. چیدمان اولیه ماری (Snake Draft) برای ظرفیت
        selectedCells.sort((a, b) => b.cap - a.cap);
        let blocks = Array.from({length: reqS}, (_, i) => ({ id: `S${i+1}`, cells: [] }));
        let idx = 0;
        for (let lap = 0; lap < currentP; lap++) {
            let order = (lap % 2 === 0) ? blocks : [...blocks].reverse();
            order.forEach(block => { block.cells.push(selectedCells[idx++]); });
        }

        // ۳. تابع ارزیابی هزینه (Cost Function) مبتنی بر قانون اهم
        function evaluate(testBlocks) {
            let caps = testBlocks.map(b => b.cells.reduce((sum, c) => sum + c.cap, 0));
            let capDiff = Math.max(...caps) - Math.min(...caps);
            
            let maxIOverall = 0;
            testBlocks.forEach(b => {
                let dcirs = b.cells.map(c => Math.abs(c.dcir));
                let rp = 1 / dcirs.reduce((sum, r) => sum + (1/r), 0);
                let vDrop_mV = totalAmps * rp; // محاسبه افت ولتاژ گروه
                let maxICell = vDrop_mV / Math.min(...dcirs); // جریان بلعیده شده توسط قوی‌ترین سلول
                if (maxICell > maxIOverall) maxIOverall = maxICell;
            });
            
            // جریمه سنگین برای عبور از جریان ایمن
            let penalty = maxIOverall > safeCurrent ? (maxIOverall - safeCurrent) * 10000 : 0;
            let cost = (capDiff * 100) + (maxIOverall * 500) + penalty;
            
            return { cost, capDiff, maxIOverall };
        }

        let currentEval = evaluate(blocks);
        let bestCost = currentEval.cost;
        
        // ۴. الگوریتم Hill Climbing (انجام ۱۰۰,۰۰۰ جابجایی تصادفی)
        const ITERATIONS = 100000;
        for (let i = 0; i < ITERATIONS; i++) {
            let b1 = Math.floor(Math.random() * reqS);
            let b2 = Math.floor(Math.random() * reqS);
            if (b1 === b2) continue;
            
            let c1 = Math.floor(Math.random() * currentP);
            let c2 = Math.floor(Math.random() * currentP);
            
            // Swap (جابجایی موقت)
            let temp = blocks[b1].cells[c1];
            blocks[b1].cells[c1] = blocks[b2].cells[c2];
            blocks[b2].cells[c2] = temp;
            
            let ev = evaluate(blocks);
            if (ev.cost < bestCost) {
                bestCost = ev.cost;
                currentEval = ev; // حفظ جابجایی موفق
            } else {
                // Revert (بازگردانی جابجایی ناموفق)
                temp = blocks[b1].cells[c1];
                blocks[b1].cells[c1] = blocks[b2].cells[c2];
                blocks[b2].cells[c2] = temp;
            }
        }

        if (currentEval.maxIOverall <= safeCurrent) {
            bestFinalBlocks = blocks;
            bestFinalStats = currentEval;
            break; 
        } else {
            currentP--;
            thermalFallbackTriggered = true;
        }
    }

    // ۵. مدیریت هوشمند خروجی و پیام‌های خطا
    if (!bestFinalBlocks || currentP === 0) {
        alertBox.innerHTML = dict[currentLang]['alert_impossible']
            .replace(/\{S\}/g, reqS)
            .replace(/\{P_NEW\}/g, inventoryMaxP) // استفاده از مقداری که کاربر واقعاً داشته
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
</script>
</body>
</html>
)rawhtml";
}

#endif