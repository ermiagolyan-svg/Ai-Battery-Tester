#ifndef PAGE_LOGS_H
#define PAGE_LOGS_H

#include <Arduino.h>

String buildLogPage() {
  return R"rawhtml(
<!DOCTYPE html>
<html lang="fa">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Mass Reports & Logs</title>
<script src="https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js"></script>
<style>
  :root{--bg:#0d0d0f;--surface:#161619;--border:#2a2a32;--accent:#00d4aa;--text:#e8e8f0;--muted:#6b6b80;--mono:monospace; --accent2:#0096ff;}
  body{background:var(--bg);color:var(--text);font-family:system-ui,sans-serif;padding:20px; max-width:900px; margin:0 auto; direction: rtl; text-align: right; transition: all 0.3s ease;}
  .header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; direction: inherit;}
  .btn-back { color: var(--accent); text-decoration: none; font-weight: bold; direction: inherit;}
  
  .section-title { font-size: 16px; font-weight: bold; margin: 30px 0 15px 0; color: var(--muted); border-bottom: 1px solid var(--border); padding-bottom: 10px; }
  
  .ch-card { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 20px; margin-bottom: 20px; position: relative;}
  .ch-card.unified { border-color: var(--accent2); background: #12161a; }
  
  .ch-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); padding-bottom: 15px; margin-bottom: 15px; direction: ltr;}
  .ch-title { font-size: 18px; font-weight: bold; }
  .unified .ch-title { color: var(--accent2); }
  
  .btn-pdf { background: linear-gradient(135deg, #ff3366, #ff6b35); color: white; border: none; padding: 8px 16px; border-radius: 8px; font-weight: bold; cursor: pointer; transition: 0.2s; font-family: inherit;}
  .btn-pdf:hover { opacity: 0.9; transform: translateY(-1px); }
  .btn-pdf.btn-uni { background: linear-gradient(135deg, #008060, var(--accent)); }
  
  .btn-csv { background: #1e1e23; color: var(--text); border: 1px solid var(--border); padding: 8px 16px; border-radius: 8px; cursor: pointer; text-decoration: none; font-size: 13.3px; font-family: inherit;}
  .btn-csv:hover { background: #2a2a32; }
  
  .btn-ai { background: linear-gradient(135deg, #8e44ad, #9b59b6); color: white; border: none; padding: 8px 16px; border-radius: 8px; font-weight: bold; cursor: pointer; transition: 0.2s; font-family: inherit; font-size: 13px;}
  .btn-ai:hover { opacity: 0.9; box-shadow: 0 0 10px rgba(155, 89, 182, 0.5); }
  
  pre { background: #0a0a0c; border: 1px solid var(--border); border-radius: 8px; padding: 12px; font-family: var(--mono); font-size: 12px; overflow-x: auto; height: 140px; color: #a1a1aa; white-space: pre; direction: ltr; text-align: left;}
  .unified pre { color: #e8e8f0; height: 220px;}
  
  #toast { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%); background: var(--accent); color: #000; padding: 10px 20px; border-radius: 8px; font-weight: bold; opacity: 0; transition: 0.3s; pointer-events: none; z-index: 9999;}
</style>
</head>
<body>
  <div class="header">
    <a href="/" class="btn-back" id="btnBack" data-i18n="back_dash">→ بازگشت به داشبورد</a>
    <h2 data-i18n="page_title">📋 گزارش‌ها و لاگ‌ها</h2>
  </div>

  <div class="section-title" data-i18n="sec_unified">🌍 پایگاه داده یکپارچه (ادغام هوشمند Triage و ظرفیت)</div>
  <div id="unified-container">
    <div class="ch-card unified">
      <div class="ch-header">
        <div class="ch-title">Global Unified Database</div>
        <div style="display:flex; gap:10px; align-items:center;">
          <button id="btn-ai-regrade" class="btn-ai" style="display:none;" onclick="runAiRegrade()">🤖 ارزیابی مجدد با AI</button>
          <a id="btn-unified-csv" class="btn-csv" style="display:none;" download="Unified_Battery_Report.csv" data-i18n="btn_csv_uni">دانلود CSV نهایی</a>
          <button id="btn-unified-pdf" class="btn-pdf btn-uni" style="display:none;" onclick="generateUnifiedPDF()" data-i18n="btn_pdf_uni">📄 خروجی PDF جامع</button>
        </div>
      </div>
      <pre id="unified-preview" data-i18n="loading">در حال اسکن کانال‌ها و پردازش ادغام اطلاعات...</pre>
    </div>
  </div>

  <div class="section-title" data-i18n="sec_raw">📂 لاگ‌های خام مجزا (Raw Data)</div>
  <div id="channels-container"></div>
  
  <div id="toast" data-i18n="toast_pdf">در حال ساخت فایل PDF، لطفاً صبر کنید...</div>

<script>
  const dict = {
    'fa': {
      'back_dash': '→ بازگشت به داشبورد',
      'page_title': '📋 گزارش‌ها و لاگ‌ها',
      'sec_unified': '🌍 پایگاه داده یکپارچه (ادغام هوشمند Triage و ظرفیت)',
      'sec_raw': '📂 لاگ‌های خام مجزا (بدون فیلتر)',
      'loading': 'در حال اسکن کانال‌ها...',
      'no_data': 'هیچ رکوردی یافت نشد.',
      'no_uni_data': 'هیچ باتری معتبری (دارای تست Triage + ظرفیت کامل > 400mAh) یافت نشد.',
      'btn_csv_uni': 'دانلود CSV نهایی',
      'btn_pdf_uni': '📄 PDF یکپارچه',
      'btn_csv_raw': 'دانلود CSV',
      'btn_pdf_raw': '📄 PDF خام',
      'toast_pdf': 'در حال پردازش، لطفاً صبر کنید...',
      'trunc': '\n... [لیست برای پیش‌نمایش کوتاه شده است]'
    },
    'en': {
      'back_dash': '← Dashboard',
      'page_title': '📋 Mass Reports & Logs',
      'sec_unified': '🌍 Global Unified Database (Smart Merge)',
      'sec_raw': '📂 Raw Channel Logs (Unfiltered)',
      'loading': 'Scanning channels...',
      'no_data': 'No records discovered.',
      'no_uni_data': 'No valid matched batteries (> 400mAh) found.',
      'btn_csv_uni': 'Unified CSV',
      'btn_pdf_uni': '📄 Unified PDF',
      'btn_csv_raw': 'Download CSV',
      'btn_pdf_raw': '📄 Raw PDF',
      'toast_pdf': 'Processing, Please wait...',
      'trunc': '\n... [List truncated for preview]'
    }
  };

  let currentLang = localStorage.getItem('sysLang') || 'fa';
  let rawLogsData = { 1: '', 2: '', 3: '' };
  let unifiedCells = [];
  let isAiRegraded = false; 

  function applyLanguage() {
    if (currentLang === 'fa') {
      document.body.style.direction = 'rtl';
      document.body.style.textAlign = 'right';
      document.getElementById('btnBack').style.direction = 'rtl';
    } else {
      document.body.style.direction = 'ltr';
      document.body.style.textAlign = 'left';
      document.getElementById('btnBack').style.direction = 'ltr';
    }
    document.querySelectorAll('[data-i18n]').forEach(el => {
      let key = el.getAttribute('data-i18n');
      if (dict[currentLang][key]) el.innerText = dict[currentLang][key];
    });
  }

  window.onload = applyLanguage;

  async function fetchAllLogs() {
    for (let c = 1; c <= 3; c++) {
      try {
        const res = await fetch('/log' + c);
        const text = await res.text();
        if (text !== 'Empty' && text.trim().length > 0) {
          rawLogsData[c] = text.trim();
        }
      } catch (e) { console.log("Error reading CH" + c); }
    }
    renderRawLogs();
    processUnifiedLogs();
  }

  function renderRawLogs() {
    const container = document.getElementById('channels-container');
    container.innerHTML = '';
    
    for (let c = 1; c <= 3; c++) {
      let text = rawLogsData[c];
      let hasData = text.length > 0;
      let previewText = dict[currentLang]['no_data'];
      
      if (hasData) {
        const lines = text.split('\n');
        previewText = lines.slice(0, 4).map(l => l.split(',').slice(0,12).join(',')).join('\n');
        if(lines.length > 4) previewText += dict[currentLang]['trunc'];
      }

      container.innerHTML += `
        <div class="ch-card">
          <div class="ch-header">
            <div class="ch-title">⚡ Channel ${c} RAW</div>
            <div style="display:flex; gap:10px;">
              ${hasData ? `<a href="/log${c}" class="btn-csv" download="CH${c}_RawLog.csv">${dict[currentLang]['btn_csv_raw']}</a>` : ''}
              ${hasData ? `<button class="btn-pdf" onclick="generateMassPDF(${c})">${dict[currentLang]['btn_pdf_raw']}</button>` : ''}
            </div>
          </div>
          <pre>${previewText}</pre>
        </div>
      `;
    }
  }

  function processUnifiedLogs() {
    let rawData = [];
    
    for (let c = 1; c <= 3; c++) {
      if(!rawLogsData[c]) continue;
      const lines = rawLogsData[c].split('\n');
      
      for (let i = 1; i < lines.length; i++) {
        const cols = lines[i].split(',');
        if (cols.length < 12) continue;
        
        let rawId = cols[0].trim();
        let cleanId = rawId.replace(/^#/, '').trim(); 
        if (cleanId === "") continue; 
        
        let start = cols[1] ? cols[1].trim() : "";
        let end = cols[2] ? cols[2].trim() : "";
        let duration = cols[3] ? cols[3].trim() : "";
        
        let mode = cols[4] ? cols[4].trim() : "";
        let cap = parseFloat(cols[5]) || 0;
        let ir = parseFloat(cols[6]) || 0;
        let grade = cols[7] ? cols[7].trim() : "Unknown";
        let dcir = parseFloat(cols[8]) || 0;
        let recRate = parseFloat(cols[9]) || 0;
        let vStart = parseFloat(cols[10]) || 0;
        let vEnd = parseFloat(cols[11]) || 0;
        
        let curve = [];
        for(let j = 12; j < cols.length; j++) {
            if(cols[j] !== undefined && cols[j].trim() !== "") {
                curve.push(parseFloat(cols[j]));
            }
        }
        
        let timeVal = new Date(start).getTime();
        if (isNaN(timeVal)) timeVal = 0; 
        
        rawData.push({ cleanId, start, end, duration, mode, cap, ir, grade, dcir, recRate, vStart, vEnd, curve, ch: c, timeVal });
      }
    }

    rawData.sort((a, b) => a.timeVal - b.timeVal);

    let allSessions = [];
    let currentSessionForId = {}; 

    rawData.forEach(row => {
        let id = row.cleanId;
        if (row.mode.includes("Triage")) {
            let newSession = { triage: row, capTests: [] };
            allSessions.push(newSession);
            currentSessionForId[id] = newSession; 
        } else if (row.cap > 0) {
            if (currentSessionForId[id]) {
                currentSessionForId[id].capTests.push(row);
            }
        }
    });

    unifiedCells = [];
    isAiRegraded = false;
    
    allSessions.forEach(session => {
        if (!session.triage) return;
        if (session.capTests.length === 0) return;

        session.capTests.sort((a, b) => b.cap - a.cap);
        let bestCapTest = session.capTests[0];

        if (bestCapTest.cap < 400) return; 

        unifiedCells.push({ 
            id: session.triage.cleanId, 
            start: bestCapTest.start,
            end: bestCapTest.end,
            duration: bestCapTest.duration,
            cap: bestCapTest.cap, 
            ir: bestCapTest.ir, 
            grade: session.triage.grade, 
            dcir: session.triage.dcir, 
            recRate: session.triage.recRate, 
            vStart: bestCapTest.vStart,
            vEnd: bestCapTest.vEnd,
            triageCurve: session.triage.curve,       
            dischargeCurve: bestCapTest.curve, 
            ch: bestCapTest.ch 
        });
    });

    updateUnifiedUI();
  }

  function updateUnifiedUI() {
    const previewEl = document.getElementById('unified-preview');
    if(unifiedCells.length === 0) {
        previewEl.innerText = dict[currentLang]['no_uni_data'];
        return;
    }

    let previewText = "ID        Cap(mAh)  IR(mΩ)  DCIR    Grade                 V-End\n";
    previewText += "-------------------------------------------------------------------\n";
    let limit = Math.min(unifiedCells.length, 8);
    for(let i=0; i<limit; i++) {
        let c = unifiedCells[i];
        let idPad = c.id.padEnd(10, ' ');
        let capPad = c.cap.toString().padEnd(10, ' ');
        let irPad = c.ir.toString().padEnd(8, ' ');
        let dcirPad = c.dcir.toString().padEnd(8, ' ');
        let grPad = c.grade.padEnd(22, ' ');
        previewText += `${idPad}${capPad}${irPad}${dcirPad}${grPad}${c.vEnd}\n`;
    }
    if(unifiedCells.length > limit) previewText += dict[currentLang]['trunc'];
    previewText += `\n\nTotal Matched Batteries Ready for K-Means: ${unifiedCells.length}`;
    if (isAiRegraded) previewText += `\n✅ (Dataset Updated via AI MLP Model)`;
    
    previewEl.innerText = previewText;

    document.getElementById('btn-unified-csv').style.display = 'inline-block';
    document.getElementById('btn-unified-pdf').style.display = 'inline-block';
    
    if(!isAiRegraded) {
        document.getElementById('btn-ai-regrade').style.display = 'inline-block';
    } else {
        document.getElementById('btn-ai-regrade').style.display = 'none';
    }

    let csvContent = "Battery ID,Start Time,End Time,Duration,Test Mode,Capacity (mAh),IR (mOhm),Triage Grade,DCIR (mOhm),Recovery (mV/s),V_Start,V_End,Triage_Graph_Points... ,|, Discharge_Graph_Points...\n";
    unifiedCells.forEach(c => {
        let row = `${c.id},${c.start},${c.end},${c.duration},Unified Match,${c.cap},${c.ir},${c.grade},${c.dcir},${c.recRate},${c.vStart},${c.vEnd}`;
        
        if (c.triageCurve && c.triageCurve.length > 0) {
            c.triageCurve.forEach(pt => row += `,${pt}`);
        } else { row += `,0`; }
        row += `,|`;
        if (c.dischargeCurve && c.dischargeCurve.length > 0) {
            c.dischargeCurve.forEach(pt => row += `,${pt}`);
        }
        
        csvContent += row + `\n`;
    });
    
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const url = URL.createObjectURL(blob);
    document.getElementById('btn-unified-csv').href = url;
  }

  // =========================================================
  // 🧠 موتور هوش مصنوعی جاوااسکریپت (رفع باگ مقیاس‌بندی)
  // =========================================================
  function runAiRegrade() {
    const ai_input_mean = [173.5880, 219.9374, 1.2423, 0.4123];
    const ai_input_scale = [153.8867, 184.9004, 0.9352, 0.2976];
    
    const ai_W1 = [
        [1.2494, 1.2423, 0.4054, 0.8977, 1.0386, -1.3105, -2.2269, -0.2796],
        [0.9774, 0.6648, -0.4494, 1.0569, 1.2002, -1.1553, -1.8880, -0.5959],
        [-1.4156, 0.0225, -0.1108, -1.0251, -1.2015, 0.1093, 0.9053, -0.5753],
        [0.6062, 0.3953, -0.3037, 0.0040, 0.9054, -0.8111, -1.0623, 0.0448]
    ];
    const ai_b1 = [-0.9863, 2.3486, 0.9161, 0.4711, 1.6583, -1.8947, -0.1112, 1.6201];
    
    const ai_W2 = [
        [-1.0577, -0.6978, -2.9391, 2.4081],
        [-2.9198, 0.4057, 0.7282, 0.2029],
        [0.1835, 0.1213, 0.8910, -0.0787],
        [0.2873, 0.1396, 0.4838, 1.0104],
        [-1.7485, -2.3899, 1.0196, 0.2499],
        [2.1276, -2.2884, -0.0097, -0.4578],
        [0.8960, -0.1261, -2.1067, -0.3912],
        [-0.8978, 2.0714, 0.8680, -1.5481]
    ];
    const ai_b2 = [-0.5272, 0.9210, 1.1192, -0.3202];

    function predictGradeJS(ir, dcir, rec, vDrop) {
        if (ir <= 0 || dcir <= 0) return "N/A";
        
        // 💡 رفع باگ مقیاس‌بندی: تبدیل به مقیاس نرمال‌شده مدل
        let scaled_rec = rec / 18.0;
        
        let x = [
            (ir - ai_input_mean[0]) / ai_input_scale[0],
            (dcir - ai_input_mean[1]) / ai_input_scale[1],
            (scaled_rec - ai_input_mean[2]) / ai_input_scale[2],
            (vDrop - ai_input_mean[3]) / ai_input_scale[3]
        ];
        
        let h = [0,0,0,0,0,0,0,0];
        for (let j = 0; j < 8; j++) {
            h[j] = ai_b1[j];
            for (let i = 0; i < 4; i++) {
                h[j] += x[i] * ai_W1[i][j];
            }
            if (h[j] < 0) h[j] = 0; 
        }
        
        let out = [0,0,0,0];
        for (let j = 0; j < 4; j++) {
            out[j] = ai_b2[j];
            for (let i = 0; i < 8; i++) {
                out[j] += h[i] * ai_W2[i][j];
            }
        }
        
        let bestClass = 0;
        let maxVal = out[0];
        for (let j = 1; j < 4; j++) {
            if (out[j] > maxVal) { maxVal = out[j]; bestClass = j; }
        }
        
        if (bestClass === 0) return "A (High Drain)";
        if (bestClass === 1) return "B (Std/Laptop)";
        if (bestClass === 2) return "C (Weak)";
        return "D (Dead/Heater)";
    }

    let previewEl = document.getElementById('unified-preview');
    let text = "ID        Old Legacy Grade    ➔   New AI Grade\n";
    text += "-------------------------------------------------------------------\n";
    
    let count = 0;
    unifiedCells.forEach(c => {
        // 💡 بازسازی دقیق ولتاژ دراپ بر اساس فرمول فیزیک (مقاومت * جریان 1.5 آمپر)
        let triageVdrop = (c.dcir / 1000.0) * 1.5;
        
        let aiGrade = predictGradeJS(c.ir, c.dcir, c.recRate, triageVdrop);
        
        let idPad = c.id.padEnd(10, ' ');
        let oldPad = c.grade.padEnd(20, ' ');
        text += `${idPad}${oldPad}➔   ${aiGrade}\n`;
        
        c.grade = aiGrade;
        count++;
    });
    
    text += `\n✅ ${count} Batteries Re-Graded Using Triage MLP Neural Network!\n(دیتابیس در مرورگر آپدیت شد. در صورت دانلود PDF یا CSV، گریدهای جدید ذخیره خواهند شد)`;
    previewEl.innerText = text;
    
    isAiRegraded = true; 
    updateUnifiedUI(); 
  }
  // =========================================================

  function renderRawPrintGraph(pts, chIndex) {
    const canvas = document.createElement('canvas');
    canvas.width = 1600; canvas.height = 600; 
    const ctx = canvas.getContext('2d');
    
    ctx.fillStyle = '#ffffff'; ctx.fillRect(0, 0, canvas.width, canvas.height);
    const minV = 2.5, maxV = 4.3;
    ctx.lineWidth = 2; ctx.strokeStyle = '#f0f0f0'; ctx.font = '26px sans-serif'; ctx.fillStyle = '#888888';
    
    [2.5, 3.0, 3.5, 4.0, 4.3].forEach(v => {
      let y = 550 - ((v - minV) / (maxV - minV)) * 500;
      ctx.beginPath(); ctx.moveTo(80, y); ctx.lineTo(1580, y); ctx.stroke();
      ctx.fillText(v.toFixed(1) + 'V', 10, y + 10);
    });

    const colors = ['#00d4aa', '#0096ff', '#ff6b35'];
    const themeColor = colors[(chIndex - 1) % 3];

    ctx.beginPath(); ctx.strokeStyle = themeColor; ctx.lineWidth = 8;
    ctx.lineCap = 'round'; ctx.lineJoin = 'round';
    
    for(let i = 0; i < pts.length; i++) {
      let x = 80 + (i / Math.max(1, pts.length - 1)) * 1500;
      let y = 550 - ((pts[i] - minV) / (maxV - minV)) * 500;
      if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();
    
    ctx.lineTo(1580, 550); ctx.lineTo(80, 550); ctx.closePath();
    ctx.fillStyle = themeColor + '25'; ctx.fill();
    return canvas.toDataURL('image/png');
  }

  function renderUnifiedGraph(triagePts, dischargePts, chIndex) {
    const canvas = document.createElement('canvas');
    canvas.width = 1600; canvas.height = 600; 
    const ctx = canvas.getContext('2d');
    
    ctx.fillStyle = '#ffffff'; ctx.fillRect(0, 0, canvas.width, canvas.height);
    const minV = 2.5, maxV = 4.3;
    ctx.lineWidth = 2; ctx.strokeStyle = '#f0f0f0'; ctx.font = '26px sans-serif'; ctx.fillStyle = '#888888';
    
    [2.5, 3.0, 3.5, 4.0, 4.3].forEach(v => {
      let y = 550 - ((v - minV) / (maxV - minV)) * 500;
      ctx.beginPath(); ctx.moveTo(80, y); ctx.lineTo(1580, y); ctx.stroke();
      ctx.fillText(v.toFixed(1) + 'V', 10, y + 10);
    });

    const colors = ['#00d4aa', '#0096ff', '#ff6b35'];
    const themeColor = colors[(chIndex - 1) % 3];

    if(triagePts && triagePts.length > 0) {
      ctx.beginPath();
      ctx.strokeStyle = '#ff6b35'; 
      ctx.lineWidth = 6; ctx.lineCap = 'round'; ctx.lineJoin = 'round';
      for(let i = 0; i < triagePts.length; i++) {
        let x = 100 + (i / Math.max(1, triagePts.length - 1)) * 650;
        let y = 550 - ((triagePts[i] - minV) / (maxV - minV)) * 500;
        if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      }
      ctx.stroke();
      ctx.fillStyle = '#ff6b35'; ctx.font = 'bold 28px sans-serif';
      ctx.fillText("Triage Profile", 100, 60);
    }

    ctx.beginPath(); ctx.moveTo(800, 40); ctx.lineTo(800, 560); 
    ctx.strokeStyle = '#dddddd'; ctx.lineWidth = 3; ctx.stroke();

    if(dischargePts && dischargePts.length > 0) {
      ctx.beginPath();
      ctx.strokeStyle = themeColor;
      ctx.lineWidth = 6; ctx.lineCap = 'round'; ctx.lineJoin = 'round';
      for(let i = 0; i < dischargePts.length; i++) {
        let x = 850 + (i / Math.max(1, dischargePts.length - 1)) * 650;
        let y = 550 - ((dischargePts[i] - minV) / (maxV - minV)) * 500;
        if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
      }
      ctx.stroke();
      
      ctx.lineTo(1500, 550); ctx.lineTo(850, 550); ctx.closePath();
      ctx.fillStyle = themeColor + '25'; ctx.fill();
      
      ctx.fillStyle = themeColor; ctx.font = 'bold 28px sans-serif';
      ctx.fillText("Discharge Cycle", 850, 60);
    }
    
    return canvas.toDataURL('image/png');
  }

  async function generateMassPDF(c) {
    if(!rawLogsData[c]) return;
    const toast = document.getElementById('toast');
    toast.style.opacity = 1;
    
    setTimeout(() => {
      try {
        const lines = rawLogsData[c].split('\n');
        const { jsPDF } = window.jspdf;
        const doc = new jsPDF();
        
        const pageWidth = doc.internal.pageSize.getWidth();
        const pageHeight = doc.internal.pageSize.getHeight();
        let y = 15;
        
        doc.setFontSize(16); doc.setFont("helvetica", "bold");
        doc.text(`Li-ion RAW Test Report - Channel ${c}`, 15, y);
        y += 8;
        doc.setFontSize(10); doc.setFont("helvetica", "normal"); doc.setTextColor(100);
        doc.text(`Generated automatically by ESP32 Battery Analyzer`, 15, y);
        y += 12;

        for(let i = 1; i < lines.length; i++) {
          const cols = lines[i].split(',');
          if(cols.length < 12) continue; 
          
          if (y > pageHeight - 68) { doc.addPage(); y = 15; }
          
          const name = cols[0];
          const date = cols[2]; 
          const action = cols[4];  
          const cap = cols[5];
          const ir = cols[6];
          const grade = cols[7] ? cols[7].trim() : "N/A";
          const dcir = cols[8] ? cols[8].trim() : "N/A";
          const vEnd = cols[11];    
          
          doc.setDrawColor(200); doc.setFillColor(248, 249, 250);
          doc.roundedRect(15, y, pageWidth - 30, 64, 2, 2, 'FD');
          
          doc.setTextColor(0); doc.setFontSize(11); doc.setFont("helvetica", "bold");
          doc.text(`ID: ${name}`, 20, y + 7);
          
          doc.setFontSize(10); doc.setTextColor(50);
          doc.text(`Test: ${action}`, 75, y + 7);
          
          doc.setFontSize(9); doc.setFont("helvetica", "normal"); doc.setTextColor(100);
          doc.text(`Date: ${date}`, pageWidth - 55, y + 7);
          
          doc.setDrawColor(220); doc.line(20, y + 11, pageWidth - 20, y + 11);
          
          doc.setTextColor(0); doc.setFontSize(9); doc.setFont("helvetica", "bold");
          
          doc.text(`Cap: ${cap} mAh`, 18, y + 17);
          doc.text(`IR/DCIR: ${ir}/${dcir} mΩ`, 55, y + 17);
          
          if(grade !== "N/A" && grade !== "Unknown" && grade !== "") {
              doc.text(`Grade: ${grade}`, 115, y + 17);
          }
          doc.text(`V-End: ${vEnd} V`, 160, y + 17);
          
          const pts = cols.slice(12).map(Number).filter(n => !isNaN(n));
          if (pts.length > 0) {
            const graphImg = renderRawPrintGraph(pts, c);
            doc.addImage(graphImg, 'PNG', 20, y + 21, pageWidth - 40, 40);
          }
          y += 68; 
        }
        
        doc.save(`CH${c}_RAW_Report_${Date.now()}.pdf`);
      } catch (err) { alert("Error generating PDF: " + err); }
      toast.style.opacity = 0;
    }, 100);
  }

  async function generateUnifiedPDF() {
    if(unifiedCells.length === 0) return;
    const toast = document.getElementById('toast');
    toast.style.opacity = 1;
    
    setTimeout(() => {
      try {
        const { jsPDF } = window.jspdf;
        const doc = new jsPDF();
        
        const pageWidth = doc.internal.pageSize.getWidth();
        const pageHeight = doc.internal.pageSize.getHeight();
        let y = 15;
        
        doc.setFontSize(16); doc.setFont("helvetica", "bold");
        doc.text(`Li-ion Unified Mass Test Report`, 15, y);
        y += 8;
        doc.setFontSize(10); doc.setFont("helvetica", "normal"); doc.setTextColor(100);
        let aiNote = isAiRegraded ? " (AI ML Re-Graded)" : "";
        doc.text(`Generated by ESP32 AI Analyzer - Valid Matched Batteries: ${unifiedCells.length}${aiNote}`, 15, y);
        y += 12;

        for(let i = 0; i < unifiedCells.length; i++) {
          const cell = unifiedCells[i];
          
          if (y > pageHeight - 68) { doc.addPage(); y = 15; }
          
          doc.setDrawColor(200); doc.setFillColor(248, 249, 250);
          doc.roundedRect(15, y, pageWidth - 30, 64, 2, 2, 'FD');
          
          doc.setTextColor(0); doc.setFontSize(11); doc.setFont("helvetica", "bold");
          doc.text(`ID: ${cell.id}`, 20, y + 7);
          
          doc.setFontSize(10); doc.setTextColor(50);
          doc.text(`Test: Chronological Match`, 75, y + 7);
          
          doc.setFontSize(9); doc.setFont("helvetica", "normal"); doc.setTextColor(100);
          doc.text(`Date: ${cell.end}`, pageWidth - 60, y + 7);
          
          doc.setDrawColor(220); doc.line(20, y + 11, pageWidth - 20, y + 11);
          
          doc.setTextColor(0); doc.setFontSize(9); doc.setFont("helvetica", "bold");
          
          doc.text(`Cap: ${cell.cap} mAh`, 18, y + 17);
          doc.text(`IR/DCIR: ${cell.ir}/${cell.dcir} mΩ`, 55, y + 17);
          
          if(cell.grade !== "N/A" && cell.grade !== "Unknown") {
              doc.text(`Grade: ${cell.grade}`, 115, y + 17);
          }
          doc.text(`V-End: ${cell.vEnd} V`, 160, y + 17);
          
          if ((cell.dischargeCurve && cell.dischargeCurve.length > 0) || (cell.triageCurve && cell.triageCurve.length > 0)) {
            const graphImg = renderUnifiedGraph(cell.triageCurve, cell.dischargeCurve, cell.ch);
            doc.addImage(graphImg, 'PNG', 20, y + 21, pageWidth - 40, 40);
          }
          y += 68; 
        }
        
        doc.save(`Unified_Battery_Report_${Date.now()}.pdf`);
      } catch (err) { alert("Error generating PDF: " + err); }
      toast.style.opacity = 0;
    }, 100);
  }

  fetchAllLogs();
</script>
</body>
</html>
)rawhtml";
}

#endif