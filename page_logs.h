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
  :root{--bg:#0d0d0f;--surface:#161619;--border:#2a2a32;--accent:#00d4aa;--text:#e8e8f0;--muted:#6b6b80;--mono:monospace}
  body{background:var(--bg);color:var(--text);font-family:system-ui,sans-serif;padding:20px; max-width:800px; margin:0 auto; direction: rtl; text-align: right; transition: all 0.3s ease;}
  .header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px; direction: inherit;}
  .btn-back { color: var(--accent); text-decoration: none; font-weight: bold; direction: inherit;}
  
  .ch-card { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 20px; margin-bottom: 20px; }
  .ch-header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); padding-bottom: 15px; margin-bottom: 15px; direction: ltr;}
  .ch-title { font-size: 18px; font-weight: bold; }
  
  .btn-pdf { background: linear-gradient(135deg, #ff3366, #ff6b35); color: white; border: none; padding: 8px 16px; border-radius: 8px; font-weight: bold; cursor: pointer; transition: 0.2s; font-family: inherit;}
  .btn-pdf:hover { opacity: 0.9; transform: translateY(-1px); }
  .btn-csv { background: #1e1e23; color: var(--text); border: 1px solid var(--border); padding: 8px 16px; border-radius: 8px; cursor: pointer; text-decoration: none; font-size: 13.3px; font-family: inherit;}
  
  pre { background: #0a0a0c; border: 1px solid var(--border); border-radius: 8px; padding: 12px; font-family: var(--mono); font-size: 12px; overflow-x: auto; height: 120px; color: #a1a1aa; white-space: pre-wrap; direction: ltr; text-align: left;}
  #toast { position: fixed; bottom: 20px; left: 50%; transform: translateX(-50%); background: var(--accent); color: #000; padding: 10px 20px; border-radius: 8px; font-weight: bold; opacity: 0; transition: 0.3s; pointer-events: none; }
</style>
</head>
<body>
  <div class="header">
    <a href="/" class="btn-back" id="btnBack" data-i18n="back_dash">→ بازگشت به داشبورد</a>
    <h2 data-i18n="page_title">📋 گزارش‌های انبوه و لاگ‌ها</h2>
  </div>

  <div id="channels-container"></div>
  <div id="toast" data-i18n="toast_pdf">در حال ساخت فایل PDF، لطفاً صبر کنید...</div>

<script>
  const dict = {
    'fa': {
      'back_dash': '→ بازگشت به داشبورد',
      'page_title': '📋 گزارش‌های انبوه و لاگ‌ها',
      'loading': 'در حال بارگذاری...',
      'no_data': 'هیچ رکوردی یافت نشد.',
      'btn_csv': 'دانلود CSV',
      'btn_pdf': '📄 خروجی PDF انبوه',
      'toast_pdf': 'در حال ساخت فایل PDF، لطفاً صبر کنید...',
      'trunc': '\n... [کوتاه شده برای پیش‌نمایش]'
    },
    'en': {
      'back_dash': '← Back Dashboard',
      'page_title': '📋 Mass Reports & Logs',
      'loading': 'Loading...',
      'no_data': 'No records discovered.',
      'btn_csv': 'Download CSV',
      'btn_pdf': '📄 Export Mass PDF',
      'toast_pdf': 'Generating PDF, Please wait...',
      'trunc': '\n... [Truncated for preview]'
    }
  };

  let currentLang = localStorage.getItem('sysLang') || 'fa';

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

  async function loadLogs() {
    const container = document.getElementById('channels-container');
    for (let c = 1; c <= 3; c++) {
      let previewText = dict[currentLang]['loading'];
      let hasData = false;
      try {
        const res = await fetch('/log' + c);
        const text = await res.text();
        if (text !== 'Empty' && text.trim().length > 0) {
          const lines = text.trim().split('\n');
          previewText = lines.slice(0, 3).map(l => l.split(',').slice(0,12).join(',')).join('\n');
          if(lines.length > 3) previewText += dict[currentLang]['trunc'];
          hasData = true;
        } else {
          previewText = dict[currentLang]['no_data'];
        }
      } catch (e) { previewText = "Error reading log."; }

      container.innerHTML += `
        <div class="ch-card">
          <div class="ch-header">
            <div class="ch-title">⚡ Channel ${c}</div>
            <div style="display:flex; gap:10px;">
              ${hasData ? `<a href="/log${c}" class="btn-csv" download>${dict[currentLang]['btn_csv']}</a>` : ''}
              ${hasData ? `<button class="btn-pdf" onclick="generateMassPDF(${c})">${dict[currentLang]['btn_pdf']}</button>` : ''}
            </div>
          </div>
          <pre>${previewText}</pre>
        </div>
      `;
    }
  }

  function renderPrintGraph(pts, chIndex) {
    const canvas = document.createElement('canvas');
    canvas.width = 1600; 
    canvas.height = 600; 
    const ctx = canvas.getContext('2d');
    
    ctx.fillStyle = '#ffffff';
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    
    const minV = 2.5, maxV = 4.3;
    
    ctx.lineWidth = 2;
    ctx.strokeStyle = '#f0f0f0';
    ctx.font = '26px sans-serif';
    ctx.fillStyle = '#888888';
    
    [2.5, 3.0, 3.5, 4.0, 4.3].forEach(v => {
      let y = 550 - ((v - minV) / (maxV - minV)) * 500;
      ctx.beginPath(); ctx.moveTo(80, y); ctx.lineTo(1580, y); ctx.stroke();
      ctx.fillText(v.toFixed(1) + 'V', 10, y + 10);
    });

    const colors = ['#00d4aa', '#0096ff', '#ff6b35'];
    const themeColor = colors[(chIndex - 1) % 3];

    ctx.beginPath();
    ctx.strokeStyle = themeColor;
    ctx.lineWidth = 8;
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';
    
    for(let i = 0; i < pts.length; i++) {
      let x = 80 + (i / Math.max(1, pts.length - 1)) * 1500;
      let y = 550 - ((pts[i] - minV) / (maxV - minV)) * 500;
      if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();

    ctx.lineTo(1580, 550);
    ctx.lineTo(80, 550);
    ctx.closePath();
    ctx.fillStyle = themeColor + '25'; 
    ctx.fill();
    
    return canvas.toDataURL('image/png');
  }

  async function generateMassPDF(c) {
    const toast = document.getElementById('toast');
    toast.style.opacity = 1;
    
    try {
      const res = await fetch('/log' + c);
      const csv = await res.text();
      const lines = csv.trim().split('\n');
      
      const { jsPDF } = window.jspdf;
      const doc = new jsPDF();
      
      const pageWidth = doc.internal.pageSize.getWidth();
      const pageHeight = doc.internal.pageSize.getHeight();
      
      let y = 15;
      
      doc.setFontSize(16);
      doc.setFont("helvetica", "bold");
      doc.text(`Li-ion Mass Test Report - Channel ${c}`, 15, y);
      y += 8;
      doc.setFontSize(10);
      doc.setFont("helvetica", "normal");
      doc.setTextColor(100);
      doc.text(`Generated automatically by ESP32 Battery Analyzer`, 15, y);
      y += 12;

      for(let i = 1; i < lines.length; i++) {
        const cols = lines[i].split(',');
        if(cols.length < 12) continue; 
        
        if (y > pageHeight - 68) {
          doc.addPage();
          y = 15;
        }
        
        const name = cols[0];
        const date = cols[2]; 
        const action = cols[4];  
        const cap = cols[5];
        const ir = cols[6];
        const grade = cols[7] ? cols[7].trim() : "N/A";
        const dcir = cols[8] ? cols[8].trim() : "N/A";
        const vStart = cols[10];  
        const vEnd = cols[11];    
        
        doc.setDrawColor(200);
        doc.setFillColor(248, 249, 250);
        doc.roundedRect(15, y, pageWidth - 30, 64, 2, 2, 'FD');
        
        doc.setTextColor(0);
        doc.setFontSize(11);
        doc.setFont("helvetica", "bold");
        doc.text(`ID: ${name}`, 20, y + 7);
        
        doc.setFontSize(10);
        doc.setTextColor(50);
        doc.text(`Test: ${action}`, 75, y + 7);
        
        doc.setFontSize(9);
        doc.setFont("helvetica", "normal");
        doc.setTextColor(100);
        doc.text(`Date: ${date}`, pageWidth - 55, y + 7);
        
        doc.setDrawColor(220);
        doc.line(20, y + 11, pageWidth - 20, y + 11);
        
        doc.setTextColor(0);
        doc.setFontSize(9);
        doc.setFont("helvetica", "bold");
        doc.text(`Cap: ${cap} mAh`, 20, y + 17);
        doc.text(`IR / DCIR: ${ir} / ${dcir} mΩ`, 65, y + 17);
        
        if(grade !== "N/A" && grade !== "Unknown") {
            doc.text(`Grade: ${grade}`, 125, y + 17);
        }
        doc.text(`V-End: ${vEnd} V`, 165, y + 17);
        
        const pts = cols.slice(12).map(Number).filter(n => !isNaN(n));
        if (pts.length > 0) {
          const graphImg = renderPrintGraph(pts, c);
          doc.addImage(graphImg, 'PNG', 20, y + 21, pageWidth - 40, 40);
        }
        
        y += 68; 
      }
      
      doc.save(`CH${c}_Mass_Report_${Date.now()}.pdf`);
      
    } catch (err) {
      alert("Error generating PDF: " + err);
    }
    toast.style.opacity = 0;
  }

  loadLogs();
</script>
</body>
</html>
)rawhtml";
}

#endif