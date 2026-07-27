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
<title>AI Parallel Matcher & Pack Builder</title>
<style>
  :root { --bg: #0d0d0f; --surface: #161619; --surface2: #1e1e23; --border: #2a2a32; --accent: #00d4aa; --accent2: #0096ff; --warn: #ff6b35; --danger: #e74c3c; --text: #e8e8f0; --muted: #6b6b80; --mono: 'Consolas', monospace; }
  body { background: var(--bg); color: var(--text); font-family: system-ui, sans-serif; padding: 20px; max-width: 1100px; margin: 0 auto; direction: rtl; text-align: right; transition: all 0.3s ease; }
  .header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--border); padding-bottom: 15px; margin-bottom: 20px; direction: inherit;}
  .btn-back { color: var(--accent); text-decoration: none; font-weight: bold; border: 1px solid var(--accent); padding: 6px 14px; border-radius: 6px; background: rgba(0,212,170,.1); transition: 0.2s; direction: inherit;}
  .btn-back:hover { background: var(--accent); color: #000; }
  
  .engine-card { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 20px; position: relative; overflow: hidden; margin-bottom: 25px;}
  .engine-card::before { content: ''; position: absolute; top: 0; left: 0; width: 100%; height: 4px; background: var(--accent); }
  
  .engine-title { font-size: 18px; font-weight: bold; margin-bottom: 10px; display: flex; align-items: center; gap: 8px;}
  .engine-desc { font-size: 13px; color: var(--muted); margin-bottom: 20px; }
  
  .input-row { display: flex; gap: 15px; margin-bottom: 20px; align-items: flex-start; flex-wrap: wrap;}
  .input-group { display: flex; flex-direction: column; gap: 5px; flex: 1; min-width: 150px;}
  .input-group label { font-size: 12px; color: var(--muted); }
  .input-group input { background: var(--bg); border: 1px solid var(--border); color: var(--text); padding: 10px; border-radius: 8px; outline: none; width: 100%; box-sizing: border-box; font-family: var(--mono); text-align: center; direction: ltr;}
  
  .tolerance-guide { font-size: 11px; color: var(--muted); background: var(--bg); border: 1px solid var(--border); padding: 10px; border-radius: 8px; margin-top: 5px; line-height: 1.6; }
  
  .btn-run { color: #fff; border: none; padding: 12px 24px; border-radius: 8px; font-weight: bold; cursor: pointer; font-size: 14px; transition: 0.2s; background: linear-gradient(135deg, #008060, var(--accent)); height: 40px; display: flex; align-items: center; justify-content: center; min-width: 200px; margin-top: 22px; font-family: inherit;}
  .btn-run:hover { opacity: 0.9; transform: translateY(-2px); }
  
  .master-graph-container { background: var(--surface); border: 1px solid var(--border); border-radius: 12px; padding: 15px; margin-bottom: 20px; display: none; direction: ltr; }
  .master-title { font-size: 14px; font-weight: bold; margin-bottom: 10px; color: var(--accent2); text-align: center;}
  
  .clusters-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 15px; direction: inherit;}
  .cluster-card { background: var(--surface2); border: 1px solid var(--border); border-radius: 12px; padding: 15px; }
  .cluster-header { font-size: 15px; font-weight: bold; margin-bottom: 10px; border-bottom: 1px solid var(--border); padding-bottom: 5px; display: flex; justify-content: space-between; align-items: center; direction: ltr;}
  
  .cluster-graph { width: 100%; height: 90px; background: var(--bg); border-radius: 8px; border: 1px solid var(--border); margin-bottom: 12px; }
  
  .cell-item { display: flex; justify-content: space-between; background: var(--bg); padding: 8px 12px; border-radius: 6px; margin-bottom: 5px; border-left: 3px solid var(--muted); font-size: 12px; font-family: var(--mono); direction: ltr; text-align: left; }
  .cell-item.grade-a { border-left-color: #00e676; }
  .cell-item.grade-b { border-left-color: #00d4aa; }
  .cell-item.grade-c { border-left-color: #ff6b35; }
  .cell-item.grade-outlier { border-left-color: var(--danger); opacity: 0.8;}
  
  .btn-build { background: var(--accent2); color: #fff; border: none; padding: 6px 12px; border-radius: 6px; font-size: 12px; font-weight: bold; cursor: pointer; width: 100%; margin-top: 10px; font-family: inherit;}
  .btn-build:hover { opacity: 0.9; }
  
  /* Pack Builder Styles */
  #pack-builder-section { display: none; margin-top: 30px; background: var(--surface); border: 1px solid var(--accent2); border-radius: 12px; padding: 20px; }
  .pack-blocks { display: flex; flex-direction: column; gap: 15px; margin-top: 20px;}
  .pack-block { background: var(--bg); border: 2px solid var(--border); border-radius: 8px; padding: 15px; position: relative; direction: ltr;}
  .pack-block::after { content: 'Negative Terminals Aligned Bottom'; position: absolute; bottom: -20px; left: 50%; transform: translateX(-50%); font-size: 10px; color: var(--muted); white-space: nowrap;}
  .block-header { display: flex; justify-content: space-between; margin-bottom: 10px; font-weight: bold; border-bottom: 1px solid var(--border); padding-bottom: 5px;}
  .block-cells { display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 10px;}
  .cell-pill { background: var(--surface2); border: 1px solid var(--border); border-radius: 6px; padding: 8px; text-align: center; font-size: 11px;}
  
  #loader { display: none; color: var(--accent); margin-bottom: 15px; font-weight: bold; text-align: center; width: 100%;}
</style>
</head>
<body>
  <div class="header">
    <h2>🧠 <span data-i18n="title_main">AI Parallel Matcher & Builder</span></h2>
    <a href="/" class="btn-back" id="btnBack" data-i18n="back_dash">→ بازگشت به داشبورد</a>
  </div>

  <div class="engine-card">
    <div class="engine-title"><span style="color:var(--accent)" data-i18n="engine1_title">1. Cell Clustering Engine</span></div>
    <div class="engine-desc" data-i18n="engine1_desc">باتری‌های Triage شده بر اساس مقاومت دینامیک و گراف ولتاژ کلاستربندی می‌شوند.</div>
    <div class="input-row">
      <div class="input-group">
        <label data-i18n="lbl_parallel">تعداد کلاستر موازی (P)</label>
        <input type="number" id="parallel-p" value="4" min="1" max="20">
      </div>
      
      <div class="input-group" style="flex: 2; min-width: 250px;">
        <label data-i18n="lbl_tol">حداکثر تلورانس مقاومت (±mΩ)</label>
        <input type="number" id="tolerance-ir" value="12" min="1" max="50">
        <div class="tolerance-guide" data-i18n="tol_guide" id="tolGuideDiv">
          <span style="color: var(--warn); font-weight: bold;">±2 تا ±5:</span> High Drain (دریل، نقطه‌جوش)<br>
          <span style="color: var(--accent); font-weight: bold;">±10 تا ±15:</span> Medium Drain (پاوربانک، لپ‌تاپ)<br>
          <span style="color: var(--text); font-weight: bold;">±20 تا ±30:</span> Low Drain (پاوروال، روشنایی)
        </div>
      </div>
      
      <button class="btn-run" onclick="runParallelMatcher()" data-i18n="btn_run">اجرای موتور هوشمند 🚀</button>
    </div>
  </div>

  <div id="loader" data-i18n="loader_txt">در حال اسکن لاگ‌ها و پردازش...</div>

  <div id="master-container" class="master-graph-container">
    <div class="master-title" data-i18n="master_title">📊 Cluster vs Cluster (Average Behavior Overlay)</div>
    <canvas id="master-canvas" width="800" height="150" style="width:100%; height:150px;"></canvas>
  </div>

  <div class="clusters-grid" id="clusters-container">
    <div id="welcome-msg" style="color:var(--muted); grid-column: 1 / -1; text-align:center; padding: 40px;" data-i18n="welcome_msg">
      دیتای تمام تست‌های تریاژ به صورت خودکار خوانده می‌شود.<br>برای شروع، روی اجرای موتور هوشمند کلیک کنید.
    </div>
  </div>

  <div id="pack-builder-section">
    <div class="engine-title"><span style="color:var(--accent2)" data-i18n="engine2_title">2. Pack Assembler (Balanced Fill + Local Optimization)</span></div>
    <div class="engine-desc" data-i18n="engine2_desc">یک کلاستر تایید شده انتخاب شده است. تنظیمات پک نهایی را وارد کنید تا ظرفیت‌ها دقیقاً بالانس شوند.</div>
    
    <div class="input-row">
      <div class="input-group">
        <label data-i18n="lbl_pack_s">تعداد سری (S)</label>
        <input type="number" id="pack-s" value="4" min="1" max="20">
      </div>
      <div class="input-group">
        <label data-i18n="lbl_pack_p">تعداد موازی (P)</label>
        <input type="number" id="pack-p" value="3" min="1" max="20">
      </div>
      <button class="btn-run" style="background: var(--accent2);" onclick="generatePack()" data-i18n="btn_pack">مونتاژ پک نهایی ⚙️</button>
    </div>
    
    <div id="pack-stats" style="display:flex; justify-content:space-around; background:var(--bg); padding:15px; border-radius:8px; font-weight:bold; margin-bottom:15px; display:none; direction:ltr;"></div>
    <div id="pack-layout" class="pack-blocks"></div>
  </div>

<script>
const dict = {
  'fa': {
    'back_dash': '→ بازگشت به داشبورد',
    'title_main': 'موتور هوش مصنوعی و ساخت پک',
    'engine1_title': '۱. موتور کلاستربندی سلول‌ها',
    'engine1_desc': 'باتری‌های Triage شده بر اساس مقاومت دینامیک و گراف ولتاژ کلاستربندی می‌شوند.',
    'lbl_parallel': 'تعداد کلاستر موازی (P)',
    'lbl_tol': 'حداکثر تلورانس مقاومت (±mΩ)',
    'tol_guide': '<span style="color: var(--warn); font-weight: bold;">±2 تا ±5:</span> High Drain (دریل، نقطه‌جوش)<br><span style="color: var(--accent); font-weight: bold;">±10 تا ±15:</span> Medium Drain (پاوربانک، لپ‌تاپ)<br><span style="color: var(--text); font-weight: bold;">±20 تا ±30:</span> Low Drain (پاوروال، روشنایی)',
    'btn_run': 'اجرای موتور هوشمند 🚀',
    'loader_txt': 'در حال اسکن لاگ‌ها و پردازش...',
    'master_title': '📊 مقایسه گرافیکال کلاسترها (میانگین رفتار)',
    'welcome_msg': 'دیتای تمام تست‌های تریاژ به صورت خودکار خوانده می‌شود.<br>برای شروع، روی اجرای موتور هوشمند کلیک کنید.',
    'engine2_title': '۲. اسمبل پک (بالانس ظرفیت + بهینه‌سازی محلی)',
    'engine2_desc': 'یک کلاستر تایید شده انتخاب شده است. تنظیمات پک نهایی را وارد کنید تا ظرفیت‌ها دقیقاً بالانس شوند.',
    'lbl_pack_s': 'تعداد سری (S)',
    'lbl_pack_p': 'تعداد موازی (P)',
    'btn_pack': 'مونتاژ پک نهایی ⚙️',
    'alert_no_cell': 'هیچ سلول معتبری از نوع تست Triage یافت نشد.',
    'alert_not_enough': 'تعداد باتری کافی نیست! شما {need} باتری نیاز دارید اما این کلاستر فقط {have} باتری دارد.',
    'sim_warn': '⚠️ سلول‌های با علامت <b>~ ... *</b> ظرفیت اندازه‌گیری‌شده ندارند — عدد نمایشی صرفاً شبیه‌سازی‌شده است تا مسیر کد قابل تست باشد و نباید برای بالانس واقعی پک به آن اعتماد کرد.',
    'btn_build_cluster': '🛠️ ساخت پک از این کلاستر',
    'outlier_title': '⚠️ سلول‌های ناسازگار (Outliers)',
    'outlier_sub': 'سلول‌های خارج از تلورانس (Skipped)',
    'sim_pack_warn': '⚠️ این بالانس بر اساس ظرفیت شبیه‌سازی‌شده (~) است، نه اندازه‌گیری واقعی - فقط برای تست مسیر کد',
    'lbl_cells': 'Cells'
  },
  'en': {
    'back_dash': '← Back Dashboard',
    'title_main': 'AI Parallel Matcher & Builder',
    'engine1_title': '1. Cell Clustering Engine',
    'engine1_desc': 'Triaged batteries are clustered based on Dynamic IR and voltage graphs.',
    'lbl_parallel': 'Target Parallel Clusters (P)',
    'lbl_tol': 'Max IR Tolerance (±mΩ)',
    'tol_guide': '<span style="color: var(--warn); font-weight: bold;">±2 to ±5:</span> High Drain (Drill, Spot Welder)<br><span style="color: var(--accent); font-weight: bold;">±10 to ±15:</span> Medium Drain (Powerbank, Laptop)<br><span style="color: var(--text); font-weight: bold;">±20 to ±30:</span> Low Drain (Powerwall, Lighting)',
    'btn_run': 'Execute AI Matcher 🚀',
    'loader_txt': 'Scanning Logs & Processing ML...',
    'master_title': '📊 Cluster vs Cluster (Average Behavior Overlay)',
    'welcome_msg': 'All Triage test data is read automatically.<br>Click Execute to begin.',
    'engine2_title': '2. Pack Assembler (Balanced Fill + Local Optimization)',
    'engine2_desc': 'An approved cluster is selected. Enter the final pack settings for exact capacity balancing.',
    'lbl_pack_s': 'Series (S)',
    'lbl_pack_p': 'Parallel (P)',
    'btn_pack': 'Assemble Final Pack ⚙️',
    'alert_no_cell': 'No valid Triage test cells found.',
    'alert_not_enough': 'Not enough batteries! You need {need} but this cluster only has {have}.',
    'sim_warn': '⚠️ Cells with <b>~ ... *</b> have no measured capacity — the number is simulated to test code paths and should not be trusted for real pack balancing.',
    'btn_build_cluster': '🛠️ Build Pack from this Cluster',
    'outlier_title': '⚠️ No-Match (Outliers)',
    'outlier_sub': 'Incompatible cells (Skipped)',
    'sim_pack_warn': '⚠️ This balance uses simulated capacity (~), not real measurements - for testing only.',
    'lbl_cells': 'Cells'
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
    if (dict[currentLang][key]) {
      if (key === 'tol_guide') {
         el.innerHTML = dict[currentLang][key];
      } else {
         el.innerText = dict[currentLang][key];
      }
    }
  });
}

window.onload = applyLanguage;

let allCells = [];
let savedClusters = [];
let activeClusterForPack = -1;
const clusterColors = ['#00d4aa', '#0096ff', '#ff6b35', '#e83e8c', '#f1c40f', '#9b59b6', '#3498db', '#2ecc71'];

async function fetchDeviceLogs() {
  allCells = [];
  for (let c = 1; c <= 3; c++) {
    try {
      const res = await fetch('/log' + c);
      const text = await res.text();
      if (text !== 'Empty' && text.trim().length > 0) {
        const lines = text.trim().split('\n');
        for (let i = 1; i < lines.length; i++) {
          const cols = lines[i].split(',');
          if (cols.length < 12) continue;
          
          let name = cols[0];
          let action = cols[4] || "";
          
          if (!action.includes("Triage")) continue;
          
          let cap = parseFloat(cols[5]) || 0;
          let simulatedCap = false;
          if(cap === 0) { cap = Math.floor(Math.random() * 600) + 2000; simulatedCap = true; }

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
          
          if(ir > 0 || curve.length > 0) {
              allCells.push({ 
                  id: name, cap: cap, simulatedCap: simulatedCap, ir: ir, dcir: dcir, 
                  recRate: recRate, curve: curve, grade: grade, ch: c 
              });
          }
        }
      }
    } catch(e) { console.log("Log read error CH" + c); }
  }
}

const IR_WEIGHT = 50.0;
const DCIR_WEIGHT = 15.0; 

function weightedDistance(cell, cent) {
  let baseDist = (Math.pow(cell.nIr - cent.nIr, 2) * IR_WEIGHT) + 
                 (Math.pow(cell.nDCIR - cent.nDCIR, 2) * DCIR_WEIGHT) + 
                 Math.pow(cell.nRec - cent.nRec, 2);
  let curveDist = 0;
  let points = Math.min(cell.nCurve.length, cent.nCurve.length);
  for(let p = 0; p < points; p++) curveDist += Math.pow(cell.nCurve[p] - cent.nCurve[p], 2);
  if(points > 0) curveDist = (curveDist / points) * 5.0;
  return Math.sqrt(baseDist + curveDist);
}

function cellToCentroid(cell) {
  return { nIr: cell.nIr, nDCIR: cell.nDCIR, nRec: cell.nRec, nCurve: [...cell.nCurve] };
}

function seedCentroidsPlusPlus(data, k) {
  let centroids = [cellToCentroid(data[Math.floor(Math.random() * data.length)])];
  while (centroids.length < k) {
    let dists = data.map(d => Math.pow(Math.min(...centroids.map(c => weightedDistance(d, c))), 2));
    let sum = dists.reduce((a,b) => a+b, 0);
    if (sum === 0) { centroids.push(cellToCentroid(data[Math.floor(Math.random() * data.length)])); continue; }
    let r = Math.random() * sum, acc = 0, chosen = data[data.length - 1];
    for (let i = 0; i < data.length; i++) { acc += dists[i]; if (acc >= r) { chosen = data[i]; break; } }
    centroids.push(cellToCentroid(chosen));
  }
  return centroids;
}

function kMeansSingleRun(data, k) {
  let centroids = seedCentroidsPlusPlus(data, k);
  let clusters = new Array(k).fill(null).map(() => []);
  let maxIter = 100;

  for(let it=0; it<maxIter; it++) {
    clusters = new Array(k).fill(null).map(() => []);
    data.forEach(cell => {
      let minDist = Infinity, clusterIdx = 0;
      centroids.forEach((cent, idx) => {
        let d = weightedDistance(cell, cent);
        if(d < minDist) { minDist = d; clusterIdx = idx; }
      });
      clusters[clusterIdx].push(cell);
    });

    let hadEmpty = false;
    clusters.forEach((cluster, idx) => {
      if(cluster.length > 0) return;
      hadEmpty = true;
      let farthestPoint = data[0], farthestDist = -1;
      data.forEach(d => {
        let nearest = Math.min(...centroids.map(c => weightedDistance(d, c)));
        if(nearest > farthestDist) { farthestDist = nearest; farthestPoint = d; }
      });
      centroids[idx] = cellToCentroid(farthestPoint);
    });
    if (hadEmpty) continue;

    let oldCentroids = JSON.parse(JSON.stringify(centroids));
    clusters.forEach((cluster, idx) => {
      let sumIr = 0, sumDCIR = 0, sumRec = 0;
      let sumCurve = [], countCurve = [];
      
      cluster.forEach(c => {
          sumIr += c.nIr; sumDCIR += c.nDCIR; sumRec += c.nRec;
          for(let p = 0; p < c.nCurve.length; p++) {
              sumCurve[p] = (sumCurve[p] || 0) + c.nCurve[p];
              countCurve[p] = (countCurve[p] || 0) + 1;
          }
      });
      
      centroids[idx] = { 
          nIr: sumIr / cluster.length, 
          nDCIR: sumDCIR / cluster.length, 
          nRec: sumRec / cluster.length,
          nCurve: sumCurve.map((v, p) => v / countCurve[p])
      };
    });

    let hasChanged = false;
    for(let i=0; i<k; i++) {
      if(Math.abs(centroids[i].nIr - oldCentroids[i].nIr) > 0.0001 ||
         Math.abs(centroids[i].nDCIR - oldCentroids[i].nDCIR) > 0.0001 ||
         Math.abs(centroids[i].nRec - oldCentroids[i].nRec) > 0.0001) { hasChanged = true; break; }
    }
    if(!hasChanged) break;
  }

  let inertia = 0;
  clusters.forEach((cluster, idx) => cluster.forEach(cell => { inertia += Math.pow(weightedDistance(cell, centroids[idx]), 2); }));
  return { clusters, inertia };
}

function kMeansDynamicClustering(data, k) {
  if (data.length <= k) return data.map(d => [d]);
  
  let maxIr = Math.max(...data.map(d => d.ir)) || 1;
  let maxDCIR = Math.max(...data.map(d => Math.abs(d.dcir))) || 1;
  let maxRec = Math.max(...data.map(d => d.recRate)) || 1;
  let maxCurveV = 4.3; 

  data.forEach(d => { 
      d.nIr = d.ir / maxIr; 
      d.nDCIR = Math.abs(d.dcir) / maxDCIR; 
      d.nRec = d.recRate / maxRec; 
      d.nCurve = d.curve.map(v => v / maxCurveV);
  });

  let best = null;
  const RESTARTS = 6;
  for (let r = 0; r < RESTARTS; r++) {
    let result = kMeansSingleRun(data, k);
    if (!best || result.inertia < best.inertia) best = result;
  }
  return best.clusters;
}

function drawClusterGraph(canvasId, group, color) {
  const canvas = document.getElementById(canvasId);
  if(!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width; const h = canvas.height;
  
  let minV = 5.0, maxV = 0.0;
  group.forEach(cell => {
    cell.curve.forEach(v => {
      if(v < minV) minV = v;
      if(v > maxV) maxV = v;
    });
  });
  
  let padding = (maxV - minV) * 0.2;
  if(padding === 0) padding = 0.1;
  minV -= padding; maxV += padding;

  ctx.clearRect(0, 0, w, h);
  ctx.lineWidth = 1.5;
  ctx.lineCap = 'round';
  ctx.lineJoin = 'round';
  
  let baseColor = color.replace('rgb', 'rgba').replace(')', ', 0.35)');
  if(color.startsWith('#')) baseColor = color + '66'; 
  ctx.strokeStyle = baseColor;

  group.forEach(cell => {
    if(cell.curve.length < 2) return;
    ctx.beginPath();
    for(let i = 0; i < cell.curve.length; i++) {
      let x = (i / Math.max(1, cell.curve.length - 1)) * w;
      let y = h - ((cell.curve[i] - minV) / (maxV - minV)) * h;
      if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();
  });
}

function drawMasterGraph(clusters) {
  const canvas = document.getElementById('master-canvas');
  if(!canvas) return;
  const ctx = canvas.getContext('2d');
  const w = canvas.width; const h = canvas.height;
  
  let minV = 5.0, maxV = 0.0;
  let avgCurves = [];
  clusters.forEach(group => {
    if(group.length === 0) return;
    let avgC = [];
    let maxLength = Math.max(...group.map(c => c.curve.length));
    
    for(let i=0; i<maxLength; i++) {
      let sum = 0, count = 0;
      group.forEach(c => { if(c.curve[i] !== undefined) { sum += c.curve[i]; count++; }});
      let val = count > 0 ? sum/count : 0;
      avgC.push(val);
      if(val > 0 && val < minV) minV = val;
      if(val > 0 && val > maxV) maxV = val;
    }
    avgCurves.push(avgC);
  });
  
  let padding = (maxV - minV) * 0.15;
  if(padding === 0) padding = 0.1;
  minV -= padding; maxV += padding;
  
  ctx.clearRect(0, 0, w, h);
  
  ctx.strokeStyle = '#2a2a32';
  ctx.lineWidth = 1;
  ctx.beginPath();
  ctx.moveTo(0, h/2); ctx.lineTo(w, h/2);
  ctx.stroke();
  
  ctx.lineWidth = 2.5;
  ctx.lineCap = 'round';
  ctx.lineJoin = 'round';
  
  avgCurves.forEach((curve, idx) => {
    if(curve.length < 2) return;
    ctx.beginPath();
    ctx.strokeStyle = clusterColors[idx % clusterColors.length];
    for(let i = 0; i < curve.length; i++) {
      if(curve[i] === 0) continue;
      let x = (i / Math.max(1, curve.length - 1)) * w;
      let y = h - ((curve[i] - minV) / (maxV - minV)) * h;
      if(i === 0) ctx.moveTo(x, y); else ctx.lineTo(x, y);
    }
    ctx.stroke();
  });
}

async function runParallelMatcher() {
  document.getElementById('loader').style.display = 'block';
  document.getElementById('master-container').style.display = 'none';
  document.getElementById('pack-builder-section').style.display = 'none';
  const container = document.getElementById('clusters-container');
  container.innerHTML = '';
  
  await fetchDeviceLogs();
  
  let validCells = allCells.filter(c => c.grade !== 'D' && c.grade !== 'Unknown' && c.grade !== 'N/A');

  if(validCells.length === 0) {
    container.innerHTML = `<div style="color:var(--warn); grid-column: 1/-1; text-align:center;">${dict[currentLang]['alert_no_cell']}</div>`;
    document.getElementById('loader').style.display = 'none';
    return;
  }

  setTimeout(() => {
    const kValue = parseInt(document.getElementById('parallel-p').value);
    const toleranceIr = parseFloat(document.getElementById('tolerance-ir').value) || 12;
    
    let rawGroups = kMeansDynamicClustering(validCells, kValue);
    
    savedClusters = [];
    let outliers = [];

    rawGroups.forEach((group) => {
      if (group.length === 0) return;
      
      let avgIr = group.reduce((s,c)=>s+c.ir,0) / group.length;
      let validGroup = [];
      
      group.forEach(cell => {
         if (Math.abs(cell.ir - avgIr) > toleranceIr) {
             outliers.push(cell);
         } else {
             validGroup.push(cell);
         }
      });
      
      if(validGroup.length > 0) savedClusters.push(validGroup);
    });

    savedClusters.sort((a, b) => (a.reduce((s,c)=>s+c.ir,0)/Math.max(1, a.length)) - (b.reduce((s,c)=>s+c.ir,0)/Math.max(1, b.length)));

    let htmlOutput = "";

    if (validCells.some(c => c.simulatedCap)) {
      htmlOutput += `<div style="grid-column: 1/-1; font-size:12px; color:var(--warn); background:var(--surface2); border:1px solid var(--border); border-radius:8px; padding:10px 14px; margin-bottom:5px;">
        ${dict[currentLang]['sim_warn']}
      </div>`;
    }
    
    savedClusters.forEach((group, idx) => {
      let avgIr = (group.reduce((s,c)=>s+c.ir,0)/group.length).toFixed(1);
      let themeColor = clusterColors[idx % clusterColors.length];
      
      htmlOutput += `<div class="cluster-card">
        <div class="cluster-header">
          <span style="color: ${themeColor}">Parallel Match #${idx+1}</span>
          <span style="font-size:12px; color:var(--text); font-weight:normal; background:var(--bg); padding:2px 8px; border-radius:10px;">${group.length} ${dict[currentLang]['lbl_cells']}</span>
        </div>
        
        <canvas id="graph-cluster-${idx}" class="cluster-graph" width="300" height="90"></canvas>
        
        <div style="font-size:12px; margin-bottom:12px; display:flex; justify-content:space-between; font-weight:bold; color:var(--muted); direction: ltr;">
            <span style="color:var(--warn)">Avg IR: ${avgIr} mΩ</span>
            <span style="color:var(--accent)">Similarity Overlay</span>
        </div>
        <div style="max-height: 150px; overflow-y: auto; padding-right: 5px;">`;
      
      group.sort((a,b) => a.ir - b.ir).forEach(cell => {
        let gClass = cell.grade === 'A' ? 'grade-a' : (cell.grade === 'B' ? 'grade-b' : 'grade-c');
        let capLabel = cell.simulatedCap ? `~${cell.cap}mAh*` : `${cell.cap}mAh`;
        let details = `${cell.ir.toFixed(1)}Ω | ${capLabel}`;
            
        htmlOutput += `<div class="cell-item ${gClass}" ${cell.simulatedCap ? 'title="Simulated"' : ''}>
          <span>${cell.id}</span>
          <span>${details}</span>
        </div>`;
      });
      
      htmlOutput += `</div>
        <button class="btn-build" onclick="initPackBuilder(${idx})">${dict[currentLang]['btn_build_cluster']}</button>
      </div>`;
    });

    if(outliers.length > 0) {
      outliers.sort((a,b) => a.ir - b.ir);
      htmlOutput += `<div class="cluster-card" style="border-color: var(--danger);">
        <div class="cluster-header">
          <span style="color: var(--danger)">${dict[currentLang]['outlier_title']}</span>
          <span style="font-size:12px; color:var(--text); font-weight:normal; background:var(--bg); padding:2px 8px; border-radius:10px;">${outliers.length} ${dict[currentLang]['lbl_cells']}</span>
        </div>
        <div style="font-size:12px; margin-bottom:12px; display:flex; justify-content:space-between; font-weight:bold; color:var(--muted);">
            <span>${dict[currentLang]['outlier_sub']}</span>
            <span style="color:var(--danger)">Skipped</span>
        </div>
        <div style="max-height: 180px; overflow-y: auto; padding-right: 5px;">`;
      
      outliers.forEach(cell => {
        let capLabel = cell.simulatedCap ? `~${cell.cap}mAh*` : `${cell.cap}mAh`;
        let details = `${cell.ir.toFixed(1)}Ω | ${capLabel}`;
        htmlOutput += `<div class="cell-item grade-outlier">
          <span>${cell.id}</span>
          <span>${details}</span>
        </div>`;
      });
      htmlOutput += `</div></div>`;
    }
    
    container.innerHTML = htmlOutput;
    
    savedClusters.forEach((group, idx) => {
      if(group.length > 0) {
          drawClusterGraph(`graph-cluster-${idx}`, group, clusterColors[idx % clusterColors.length]);
      }
    });
    
    if(savedClusters.length > 0) {
        document.getElementById('master-container').style.display = 'block';
        drawMasterGraph(savedClusters);
    }
    
    document.getElementById('loader').style.display = 'none';
  }, 100);
}

function initPackBuilder(clusterIdx) {
    activeClusterForPack = clusterIdx;
    document.getElementById('pack-builder-section').style.display = 'block';
    document.getElementById('pack-builder-section').scrollIntoView({ behavior: 'smooth' });
}

function generatePack() {
    if(activeClusterForPack < 0 || !savedClusters[activeClusterForPack]) return;
    
    let s = parseInt(document.getElementById('pack-s').value);
    let p = parseInt(document.getElementById('pack-p').value);
    let totalNeeded = s * p;
    let pool = savedClusters[activeClusterForPack];
    
    if(pool.length < totalNeeded) {
        let msg = dict[currentLang]['alert_not_enough'].replace('{need}', totalNeeded).replace('{have}', pool.length);
        alert(msg);
        return;
    }
    
    let sortedPool = [...pool].sort((a, b) => b.cap - a.cap).slice(0, totalNeeded);
    let blocks = Array.from({length: s}, (_, i) => ({ id: `S${i+1}`, cells: [] }));
    
    let idx = 0;
    for (let lap = 0; lap < p; lap++) {
        let order = (lap % 2 === 0) ? blocks : [...blocks].reverse();
        order.forEach(block => { block.cells.push(sortedPool[idx++]); });
    }
    
    const blockCap = b => b.cells.reduce((sum, c) => sum + c.cap, 0);
    
    for (let iter = 0; iter < 500; iter++) {
        blocks.sort((a, b) => blockCap(b) - blockCap(a));
        let heavy = blocks[0], light = blocks[blocks.length - 1];
        let oldSpread = Math.abs(blockCap(heavy) - blockCap(light));
        let bestGain = 0, bestI = -1, bestJ = -1;
        
        for (let i = 0; i < heavy.cells.length; i++) {
            for (let j = 0; j < light.cells.length; j++) {
                let diff = heavy.cells[i].cap - light.cells[j].cap;
                let newSpread = Math.abs(oldSpread - 2 * diff);
                if (newSpread < oldSpread - bestGain) { bestGain = oldSpread - newSpread; bestI = i; bestJ = j; }
            }
        }
        if (bestI < 0) break; 
        let tmp = heavy.cells[bestI];
        heavy.cells[bestI] = light.cells[bestJ];
        light.cells[bestJ] = tmp;
    }
    
    blocks.sort((a, b) => parseInt(a.id.replace('S','')) - parseInt(b.id.replace('S','')));
    
    let maxCap = Math.max(...blocks.map(b => blockCap(b)));
    let minCap = Math.min(...blocks.map(b => blockCap(b)));
    let deviation = maxCap - minCap;
    let avgPackIr = blocks.reduce((sum, b) => sum + (b.cells.reduce((s,c)=>s+c.ir,0) / b.cells.length), 0);
    let anySimulated = sortedPool.some(c => c.simulatedCap);
    
    let statsHtml = `
      <span style="color:var(--accent2)">Total Pack IR: ${avgPackIr.toFixed(1)} mΩ</span>
      <span style="color:var(--warn)">Max Capacity Deviation: ${deviation.toFixed(0)} mAh</span>
    `;
    if (anySimulated) {
      statsHtml += `<span style="color:var(--danger); flex-basis:100%; text-align:center; margin-top:6px; font-size:12px;">${dict[currentLang]['sim_pack_warn']}</span>`;
    }
    
    let layoutHtml = "";
    blocks.forEach(block => {
        let blockTotalCap = blockCap(block);
        let blockAvgIr = (block.cells.reduce((s,c)=>s+c.ir,0) / block.cells.length).toFixed(1);
        layoutHtml += `<div class="pack-block">
            <div class="block-header">
                <span style="color:var(--accent)">Block ${block.id}</span>
                <span style="color:var(--muted)">Cap: ${blockTotalCap.toFixed(0)} mAh | Avg IR: ${blockAvgIr} mΩ | ${block.cells.length} cells</span>
            </div>
            <div class="block-cells">`;
        
        block.cells.forEach(cell => {
            let capLabel = cell.simulatedCap ? `~${cell.cap}mAh*` : `${cell.cap}mAh`;
            layoutHtml += `<div class="cell-pill">
                <div style="font-weight:bold; color:var(--text)">${cell.id}</div>
                <div style="color:var(--muted); font-family:var(--mono); font-size:10px;">${capLabel} | ${cell.ir.toFixed(1)}Ω</div>
            </div>`;
        });
        
        layoutHtml += `</div></div>`;
    });
    
    document.getElementById('pack-stats').style.display = 'flex';
    document.getElementById('pack-stats').style.flexWrap = 'wrap';
    document.getElementById('pack-stats').style.gap = '8px';
    document.getElementById('pack-stats').innerHTML = statsHtml;
    document.getElementById('pack-layout').innerHTML = layoutHtml;
}
</script>
</body>
</html>
)rawhtml";
}

#endif