#pragma once
#include <Arduino.h>

const char kWebPage[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en"><head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#081526">
<title>AV Receiver · Control</title>
<style>
:root{color-scheme:dark;--bg:#081526;--line:rgba(151,190,220,.13);--text:#edf6fc;--muted:#a6bbc9;--blue:#56c8f5;--blue-soft:rgba(86,200,245,.13);--good:#62d6a5;--danger:#ff8c94;font-family:Inter,ui-sans-serif,system-ui,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif}
*{box-sizing:border-box}
html{scroll-behavior:smooth;background:var(--bg)}
body{margin:0;background:radial-gradient(ellipse at 50% -18%,#173b5b 0,transparent 46%),var(--bg);color:var(--text);min-height:100vh;padding:0 0 calc(92px + env(safe-area-inset-bottom))}
button,input,select{font:inherit}
button{cursor:pointer;touch-action:manipulation}
button:disabled,select:disabled{opacity:.45;cursor:not-allowed}
button:focus-visible,input:focus-visible,select:focus-visible,summary:focus-visible{outline:2px solid var(--blue);outline-offset:3px}
.app{width:min(100%,760px);margin:0 auto;padding:0 16px}
.topbar{display:flex;align-items:center;justify-content:space-between;padding:18px 2px 20px;gap:12px}
.brand{display:flex;align-items:center;gap:11px;min-width:0;color:inherit;text-decoration:none;border-radius:14px}
.brand:focus-visible{outline:2px solid var(--blue);outline-offset:3px}
.brandmark{width:40px;height:40px;flex:none;border-radius:14px;background:linear-gradient(145deg,#65d6fb,#258fc9);display:grid;place-items:center;color:#061827;font-size:19px;font-weight:900;transition:transform .15s,box-shadow .15s}
.brand:hover .brandmark,.brand:focus-visible .brandmark{transform:scale(1.06);box-shadow:0 0 0 3px #56c8f540}
.brandname{font-size:14px;font-weight:850;letter-spacing:.16em}.brandmeta{font-size:10px;color:var(--muted);letter-spacing:.14em;margin-top:3px}
.connection{font-size:10px;font-weight:750;letter-spacing:.08em;color:var(--muted);border:1px solid var(--line);border-radius:999px;padding:8px 10px;background:#0b1b2b;white-space:nowrap}
.connection.status{color:var(--good);border-color:#62d6a544}.connection.offline{color:var(--danger);border-color:#ff727c66}
.eyebrow{font-size:10px;letter-spacing:.18em;color:var(--blue);font-weight:800;text-transform:uppercase}
h1{font-size:clamp(25px,7vw,34px);letter-spacing:-.045em;margin:4px 0 5px}
.sub{font-size:12px;color:var(--muted);line-height:1.5}
.section{margin-top:18px;scroll-margin-top:16px}
.sectionhead{display:flex;align-items:center;justify-content:space-between;gap:12px;margin:0 2px 11px}.sectionhead h2{font-size:14px;letter-spacing:.01em;margin:0}.sectionhead span{font-size:10px;color:var(--muted)}
.card{background:linear-gradient(155deg,#142b43,#0d1f31);border:1px solid var(--line);border-radius:22px;box-shadow:0 18px 48px #0004;padding:17px}
.hero{position:relative;overflow:hidden;padding:20px 18px 18px}.hero:before{content:"";position:absolute;width:260px;height:260px;border-radius:50%;right:-115px;top:-150px;background:#56c8f51a;pointer-events:none}
.herohead{display:flex;justify-content:space-between;align-items:center;gap:10px;position:relative}
.pill{border:1px solid var(--line);border-radius:999px;padding:7px 10px;font-size:10px;color:var(--muted);background:#091a2a}
.volume-wrap{display:grid;place-items:center;padding:12px 0 10px;position:relative}
.gauge{--accent:var(--blue);position:relative;width:min(56vw,210px);aspect-ratio:1;margin:0 auto;user-select:none;-webkit-user-select:none;-webkit-tap-highlight-color:transparent}
.gauge .g-track,.gauge .g-fill,.g-knob{cursor:pointer}.gauge.dragging,.gauge.dragging *{cursor:grabbing}
.gauge-input{position:absolute;width:1px;height:1px;opacity:0;pointer-events:none}
.gauge:focus-within .g-track{stroke:#3f7099}
.gauge svg{position:absolute;inset:0;width:100%;height:100%;overflow:visible}
.gauge circle{fill:none;stroke-linecap:round}
.g-track{stroke:#24445f;stroke-width:9}
.g-fill{stroke:var(--accent);stroke-width:9;transition:stroke-dasharray .12s,stroke .12s;filter:drop-shadow(0 0 6px color-mix(in srgb,var(--accent) 45%,transparent))}
.g-mark{fill:var(--text);stroke:#0a1b2b;stroke-width:2}
.gauge circle.g-knob{fill:var(--accent);stroke:#0a1b2b;stroke-width:2.5;transition:r .12s,fill .12s}.gauge.dragging circle.g-knob{r:9}
.g-center{position:absolute;inset:0;display:grid;place-content:center;text-align:center;padding-top:6%}
.g-num{font-size:clamp(42px,11vw,56px);font-weight:760;letter-spacing:-.075em;line-height:1;font-variant-numeric:tabular-nums;color:var(--text);transition:color .12s}
.g-unit{font-size:11px;color:var(--accent);font-weight:800;letter-spacing:.15em;margin-top:5px;text-transform:uppercase}
.g-cap{font-size:11px;color:var(--muted);margin-top:6px}.g-cap:empty{display:none}
.g-ends{position:absolute;left:14%;right:14%;bottom:4%;display:flex;justify-content:space-between;font-size:11px;color:var(--muted);font-weight:700;font-variant-numeric:tabular-nums}
.gauge.preview{--accent:#ffb454}.gauge.preview .g-num{color:#ffb454}
.gauge.match{--accent:var(--good)}.gauge.match .g-num{color:var(--good)}
.gauge.muted{--accent:#5c7387}.gauge.muted .g-num{color:var(--muted)}
.gauge.small{width:min(100%,132px)}.gauge.small .g-num{font-size:30px}.gauge.small .g-unit{font-size:10px;letter-spacing:.1em;margin-top:3px}.gauge.small .g-cap{font-size:9px;margin-top:3px}.gauge.small .g-ends{font-size:9px;bottom:2%}
.gauge.small .g-track,.gauge.small .g-fill{stroke-width:10}
.icon-btn{flex:none;width:38px;height:38px;display:grid;place-items:center;padding:0;border:1px solid var(--line);border-radius:12px;background:#0b1c2c;color:var(--muted);transition:background .12s,color .12s,border-color .12s}
.icon-btn svg{width:19px;height:19px}.icon-btn:hover{color:var(--blue);border-color:#56c8f566}
.icon-btn .ic-off{display:none}.icon-btn.active{background:#ff8c941f;border-color:var(--danger);color:var(--danger)}
.icon-btn.active .ic-on{display:none}.icon-btn.active .ic-off{display:inline}
.head-actions{display:flex;align-items:center;gap:6px}
input[type=range]{width:100%;accent-color:var(--blue);cursor:pointer}
.vol-actions{display:grid;grid-template-columns:1fr 1fr;gap:9px;margin-top:15px}
.control-btn{min-height:48px;border-radius:15px;border:1px solid var(--line);background:#0b1c2c;color:var(--text);font-weight:800;font-size:17px}
#mute{width:42px;height:42px}#mute svg{width:21px;height:21px}
.channel-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}
.channel{background:#0b1d2e;border:1px solid var(--line);border-radius:17px;padding:13px;min-width:0}
.channel-head{display:flex;align-items:center;justify-content:space-between;gap:8px}.channel-head strong{font-size:14px}.channel-head output{color:var(--blue);font-size:13px;font-variant-numeric:tabular-nums}
.channel-status{display:block;color:var(--muted);font-size:10px;min-height:14px;margin-top:3px}
.channel input[type=range]{margin:8px 0}
.stepper{display:flex;align-items:center;gap:8px}.stepper input{flex:1;min-width:0}
.step-btn{flex:none;width:44px;min-height:44px;border:1px solid var(--line);border-radius:12px;background:#10243a;color:var(--text);font-size:19px}
.channel .step-btn{width:40px}.channel .row-actions{margin-top:8px}.channel .action-btn{min-width:0;min-height:40px;padding:8px 4px;font-size:10px}
[hidden]{display:none!important}
.pair-note{color:var(--muted);font-size:10px;line-height:1.4;margin:8px 0 0}.pair-note:empty{display:none}
.dual{display:grid;grid-template-columns:1fr 1fr;gap:12px}.sliderbox{background:#0b1d2e;border:1px solid var(--line);border-radius:16px;padding:13px;min-width:0}
.sliderhead{display:flex;justify-content:space-between;align-items:center;font-size:11px;margin-bottom:12px}.sliderhead output{color:var(--blue);font-weight:700}
.sliderbox .stepper{margin-top:8px}.sliderbox .step-btn{width:44px}.sliderbox .stepper span{flex:1}
.field{margin-top:12px}.field label{display:block;color:var(--muted);font-size:10px;font-weight:750;letter-spacing:.1em;text-transform:uppercase;margin-bottom:9px}
.field input:not([type=range]),.field select{width:100%;min-height:44px;border:1px solid var(--line);border-radius:12px;background:#081827;color:var(--text);padding:10px 11px}
.field .stepper input{width:0;text-align:center;font-variant-numeric:tabular-nums}
.field input[type=range]{margin:3px 0}
.gain-value{float:right;color:var(--blue);font-size:12px}
.row-actions{display:flex;flex-wrap:wrap;gap:8px;margin-top:12px}.action-btn{flex:1;min-width:110px;min-height:44px;border:1px solid var(--line);background:#0b1d2e;color:var(--text);border-radius:14px;padding:11px 10px;font-size:12px;font-weight:750;text-align:center}
.action-btn.active{background:var(--blue-soft);border-color:var(--blue);color:var(--blue)}
details.advanced{border:1px solid var(--line);border-radius:18px;background:#0c1e30;overflow:hidden;margin-top:10px}
details.advanced summary{padding:16px;cursor:pointer;list-style:none;display:flex;justify-content:space-between;align-items:center;font-size:12px;font-weight:800}
details.advanced summary::-webkit-details-marker{display:none}details.advanced summary:after{content:"＋";color:var(--blue);font-size:17px}details.advanced[open] summary:after{content:"−"}
.advancedbody{padding:0 14px 15px;border-top:1px solid var(--line)}
.preset-summary{display:flex;align-items:center;justify-content:space-between;gap:12px}
.preset-summary span{font-size:11px;color:var(--blue);text-align:right;overflow-wrap:anywhere}
.notice{font-size:11px;color:var(--muted);line-height:1.55;padding:12px;border-radius:13px;background:#091a2a;border:1px solid var(--line);margin-top:10px}
.notice:empty{display:none}.error{color:var(--danger);white-space:pre-wrap}
.light-grid{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.light-grid .field{margin-top:0}.light-power{display:flex;align-items:center;gap:10px;color:var(--text);font-size:13px}
.light-power input{width:20px;height:20px;accent-color:var(--blue)}
.light-grid .wide{grid-column:1/-1}
.bottomnav{position:fixed;z-index:20;bottom:0;left:0;right:0;padding:9px 12px calc(9px + env(safe-area-inset-bottom));background:rgba(6,19,32,.95);backdrop-filter:blur(18px);border-top:1px solid var(--line)}
.navinner{width:min(100%,600px);margin:auto;display:grid;grid-template-columns:repeat(4,1fr);gap:5px}
.navitem{display:grid;justify-items:center;gap:4px;padding:7px 3px;border:0;background:transparent;color:#a6bbc9;font-size:9px;font-weight:750;text-decoration:none}
.navitem b{font-size:17px;font-weight:500}.navitem:focus,.navitem:hover{color:var(--blue)}
@media(min-width:680px){body{padding-bottom:28px}.app{padding:0 24px}.bottomnav{display:none}.hero{padding:22px}}
@media(max-width:360px){.app{padding:0 12px}.card{padding:14px}.channel{padding:11px}.topbar{flex-wrap:wrap}}
@media(prefers-reduced-motion:reduce){html{scroll-behavior:auto}}
</style></head>
<body><div class="app">
<header class="topbar"><a class="brand" id="masterLink" href="/master" title="Open master dashboard (requires home Wi-Fi)" aria-label="Open master dashboard"><div class="brandmark" aria-hidden="true">A</div><div><div class="brandname">AUDIO / CONTROL</div><div class="brandmeta">NETWORK AV RECEIVER</div></div></a><div id="connection" class="connection">CONNECTING</div></header>
<section class="section"><div class="eyebrow">Personal listening space</div><h1>Receiver control</h1><div class="sub">Precision control for your home theatre system.</div></section>
<div id="warning" class="notice error" role="alert"></div>
<section class="section" id="volumeSection"><div class="card hero">
<div class="herohead"><div class="eyebrow"></div><div class="head-actions"><div class="pill" id="rampStatus" aria-live="polite">CONNECTING</div><button id="mute" class="icon-btn" type="button" aria-label="Mute master" title="Mute master"></button></div></div>
<div class="volume-wrap" id="masterGauge"></div>
<div class="vol-actions"><button id="down" class="control-btn" type="button" aria-label="Decrease master volume">−</button><button id="up" class="control-btn" type="button" aria-label="Increase master volume">+</button></div>
</div></section>
<section class="section" id="speakersSection"><div class="sectionhead"><h2>Speaker levels</h2><span>6 CHANNELS</span></div><div class="card"><div id="channels" class="channel-grid"></div><div class="row-actions"><button id="channelView" class="action-btn" type="button" aria-pressed="false">Individual view</button><button id="balance" class="action-btn" type="button">Restore balance</button></div></div></section>
<section class="section" id="toneSection"><div class="sectionhead"><h2>Tone shaping</h2><span>LEFT / RIGHT</span></div><div class="card">
<div class="dual">
<div class="sliderbox"><div class="sliderhead"><label for="bass">Bass</label><output id="bassValue"></output></div><input id="bass" type="range" min="0" max="14" step="1"><div class="stepper"><button id="bassDown" class="step-btn" type="button" aria-label="Decrease bass">−</button><span></span><button id="bassUp" class="step-btn" type="button" aria-label="Increase bass">+</button></div></div>
<div class="sliderbox"><div class="sliderhead"><label for="treble">Treble</label><output id="trebleValue"></output></div><input id="treble" type="range" min="0" max="14" step="1"><div class="stepper"><button id="trebleDown" class="step-btn" type="button" aria-label="Decrease treble">−</button><span></span><button id="trebleUp" class="step-btn" type="button" aria-label="Increase treble">+</button></div></div>
</div><div class="row-actions"><button id="bypass" class="action-btn" type="button">Tone bypass</button><button id="flat" class="action-btn" type="button">Reset flat</button></div></div></section>
<section class="section" id="lightsSection"><div class="sectionhead"><h2>Knob lighting</h2><span>BOTH RINGS</span></div><div class="card">
<fieldset id="lightFields" disabled style="border:0;padding:0;margin:0;min-width:0"><div class="light-grid">
<label class="light-power wide"><input id="lightOn" type="checkbox" role="switch"> Ring power</label>
<div class="field"><label for="lightPattern">Pattern</label><select id="lightPattern"><option value="solid">Solid</option><option value="breathing">Breathing</option><option value="rainbow">Rainbow</option><option value="chase">Chase</option><option value="comet">Comet</option><option value="color_cycle">Colour cycle</option><option value="aurora">Aurora</option><option value="ocean">Ocean waves</option><option value="fire">Fire glow</option><option value="candle">Candlelight</option><option value="twinkle">Starlight twinkle</option><option value="pendulum">Pendulum</option><option value="heartbeat">Heartbeat</option><option value="lighthouse">Lighthouse</option><option value="nebula">Nebula</option><option value="dual_breathing">Dual breathing</option><option value="color_segments">Colour segments</option></select></div>
<div class="field"><label for="lightColor">Colour</label><input id="lightColor" type="color" value="#8be0d5"></div>
<div class="field wide"><label for="lightBrightness">Brightness · <output id="lightBrightnessValue">40%</output></label><input id="lightBrightness" type="range" min="0" max="100" value="40"></div>
</div></fieldset><div id="lightStatus" class="notice" role="status" aria-live="polite">Connecting to knob lights...</div>
</div></section>
<section class="section" id="presetsSection"><details class="advanced" id="presetsPanel"><summary><span class="preset-summary"><strong>Listening presets</strong><span id="selectedPreset">No preset selected</span></span></summary><div class="advancedbody">
<div class="field"><label for="preset">Select and apply preset</label><select id="preset" disabled><option value="">No preset selected</option></select></div>
<div class="field"><label for="presetName">Preset name</label><input id="presetName" type="text" maxlength="32" autocomplete="off" placeholder="e.g. Movie night"></div>
<div id="presetStatus" class="notice" aria-live="polite">Loading presets...</div>
<div class="row-actions"><button id="presetCreate" class="action-btn" type="button">Create</button><button id="presetApply" class="action-btn" type="button" disabled>Reapply</button><button id="presetUpdate" class="action-btn" type="button" disabled>Update</button></div>
<div class="row-actions"><button id="presetRename" class="action-btn" type="button" disabled>Rename</button><button id="presetDelete" class="action-btn" type="button" disabled>Delete</button></div></div></details></section>
<section class="section" id="moreSection"><div class="sectionhead"><h2>Fine tuning</h2><span>ADVANCED</span></div>
<details class="advanced"><summary>Input and channel gain</summary><div class="advancedbody">
<div class="field"><label for="inputGain">Input gain <output id="inputGainValue" class="gain-value"></output></label><div class="stepper"><button id="inputGainDown" class="step-btn" type="button" aria-label="Decrease input gain">−</button><input id="inputGain" type="range" min="0" max="14" step="2"><button id="inputGainUp" class="step-btn" type="button" aria-label="Increase input gain">+</button></div></div>
<div id="channelGains"></div>
<div class="notice">Gain is separate from the volume limit. Nonzero ADC attenuation is disabled because the datasheet tables conflict; the presumed zero code is not hardware-verified.</div>
</div></details>
<details class="advanced"><summary>Volume safety and ramp</summary><div class="advancedbody">
<div class="field"><label for="limit">Maximum volume limit (0–60)</label><div class="stepper"><button id="limitDown" class="step-btn" type="button" aria-label="Decrease maximum volume">−</button><input id="limit" type="number" inputmode="numeric" min="0" max="60" step="1"><button id="limitUp" class="step-btn" type="button" aria-label="Increase maximum volume">+</button></div></div>
<div class="field"><label for="startup">Startup volume (0–60)</label><div class="stepper"><button id="startupDown" class="step-btn" type="button" aria-label="Decrease startup volume">−</button><input id="startup" type="number" inputmode="numeric" min="0" max="60" step="1"><button id="startupUp" class="step-btn" type="button" aria-label="Increase startup volume">+</button></div></div>
<div class="field"><label for="step">Volume button increment</label><div class="stepper"><button id="stepDown" class="step-btn" type="button" aria-label="Decrease volume button increment">−</button><input id="step" type="number" inputmode="numeric" min="1" max="10" step="1"><button id="stepUp" class="step-btn" type="button" aria-label="Increase volume button increment">+</button></div></div>
<div class="field"><label for="rampMs">Ramp interval (ms per 1 dB)</label><div class="stepper"><button id="rampMsDown" class="step-btn" type="button" aria-label="Decrease ramp interval">−</button><input id="rampMs" type="number" inputmode="numeric" min="5" max="1000" step="1"><button id="rampMsUp" class="step-btn" type="button" aria-label="Increase ramp interval">+</button></div></div>
<div class="row-actions"><button id="restore" class="action-btn" type="button">Restore last volume</button></div>
<div class="notice">Startup volume applies when last-volume restore is off. Volume limits do not limit input or output gain.</div>
</div></details>
<details class="advanced"><summary>Backup and restore</summary><div class="advancedbody"><div class="row-actions"><button id="backup" class="action-btn" type="button">Download backup</button><button id="restoreFile" class="action-btn" type="button">Restore backup</button></div><div class="notice">Backups contain audio settings, not presets or Wi-Fi credentials.</div><input id="file" type="file" accept=".json,application/json" hidden></div></details>
<details class="advanced"><summary>Home Wi-Fi connection</summary><div class="advancedbody">
<div id="homeStatus" class="notice"></div>
<div class="notice">Home Wi-Fi is configured in the receiver firmware. Reserve the fixed address in your router. Home-network control uses unencrypted HTTP.</div>
</div></details></section></div>
<nav class="bottomnav" aria-label="Quick navigation"><div class="navinner"><a class="navitem" href="#volumeSection"><b aria-hidden="true">◉</b>Volume</a><a class="navitem" href="#speakersSection"><b aria-hidden="true">◫</b>Speakers</a><a class="navitem" href="#toneSection"><b aria-hidden="true">〰</b>Tone</a><a class="navitem" href="#moreSection"><b aria-hidden="true">⋯</b>More</a></div></nav>
<script>
const $=id=>document.getElementById(id);
let snapshot=null, busy=false, polling=false, nextPoll=0, queue=[], selectedPreset='', individualView=false;
let lightDirty=false, lightBusy=false, lightRevision=0, lightTimer=null, lightAvailable=false;
const automaticLightColors=['rainbow','color_cycle','aurora','ocean','fire','candle','nebula','dual_breathing','color_segments'];
function lightPreview(){
 $('lightBrightnessValue').textContent=$('lightBrightness').value+'%';
 $('lightColor').disabled=!lightAvailable||automaticLightColors.includes($('lightPattern').value);
 $('lightColor').title=automaticLightColors.includes($('lightPattern').value)?'This pattern chooses its own colours':'Shared ring colour';
}
function renderLights(settings){
 lightAvailable=!!(settings&&settings.available);
 $('lightFields').disabled=!lightAvailable;
 if(!lightAvailable){$('lightStatus').textContent='Knob lights unavailable';}
 else if(!lightDirty&&!lightBusy){
  $('lightOn').checked=settings.on;$('lightColor').value=settings.color;
  $('lightPattern').value=settings.pattern;$('lightBrightness').value=settings.brightness;
  $('lightStatus').textContent='';
 }
 lightPreview();
}
$('lightFields').addEventListener('input',event=>{
 lightDirty=true;lightRevision++;clearTimeout(lightTimer);
 $('lightStatus').textContent='';lightPreview();
 lightTimer=setTimeout(()=>{lightTimer=null;sendLights()},event.target.id==='lightOn'||event.target.id==='lightPattern'?0:300);
});
async function sendLights(){
 if(lightBusy||!lightDirty)return;
 if(!lightAvailable){$('lightStatus').textContent='Knob lights unavailable; change a setting to retry';return}
 const revision=lightRevision;
 const body=new URLSearchParams({on:$('lightOn').checked?'1':'0',color:$('lightColor').value.slice(1),brightness:$('lightBrightness').value,pattern:$('lightPattern').value});
 lightBusy=true;
 try{
  const current=await get();
  const r=await request('/api/knob-lights',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded','X-Receiver-Token':current.token},body});
  if(!r.ok)throw Error(await r.text());
  if(revision===lightRevision){lightDirty=false;$('lightStatus').textContent=''}
 }catch(e){$('lightStatus').textContent='Not confirmed: '+e.message+'. Change a setting to retry.'}
 finally{lightBusy=false;if(lightDirty&&lightRevision!==revision&&lightTimer===null)sendLights()}
}
const names=['FL','FR','C','SW','SL','SR'];
const pairs=[{id:'front',name:'FL / FR',indices:[0,1]},{id:'surround',name:'SL / SR',indices:[4,5]}];
// 270-degree arc on a pathLength=100 circle: starts bottom-left (min), ends bottom-right (max).
const GAUGE_SPAN=75, gaugeState={};
function gaugeMarkup(id,small,inputId,label){return `<div class="gauge${small?' small':''}" id="gauge${id}"><input id="${inputId}" class="gauge-input" type="range" min="0" max="60" step="1" aria-label="${label} volume"><svg viewBox="0 0 120 120" aria-hidden="true"><circle class="g-track" cx="60" cy="60" r="50" pathLength="100" stroke-dasharray="${GAUGE_SPAN} 100" transform="rotate(135 60 60)"/><circle class="g-fill" id="gaugeFill${id}" cx="60" cy="60" r="50" pathLength="100" stroke-dasharray="0 100" transform="rotate(135 60 60)"/><circle class="g-mark" id="gaugeMark${id}" r="5" cx="60" cy="10" hidden/><circle class="g-knob" id="gaugeKnob${id}" r="7" cx="60" cy="10"/></svg><div class="g-center"><div class="g-num" id="gaugeNum${id}">--</div><div class="g-unit">${label}</div><div class="g-cap" id="gaugeCap${id}"></div></div><div class="g-ends" aria-hidden="true"><span>0</span><span>60</span></div></div>`}
const MUTE_ICON='<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M3 9v6h4l5 4V5L7 9H3z" fill="currentColor"/><path class="ic-on" d="M15.5 8.5a5 5 0 0 1 0 7M18 6a8.5 8.5 0 0 1 0 12" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/><path class="ic-off" d="M16 9.5l5 5M21 9.5l-5 5" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round"/></svg>';
const RESET_ICON='<svg viewBox="0 0 24 24" aria-hidden="true"><path d="M4.5 12a7.5 7.5 0 1 0 2.2-5.3M4.5 4v4h4" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/></svg>';
function tileHead(label,muteId,resetId){return `<div class="channel-head"><button id="${resetId}" class="icon-btn" type="button" aria-label="Reset ${label}" title="Reset ${label}">${RESET_ICON}</button><button id="${muteId}" class="icon-btn" type="button" aria-label="Mute ${label}" title="Mute ${label}">${MUTE_ICON}</button></div>`}
$('mute').innerHTML=MUTE_ICON;
function paintGauge(id){const g=gaugeState[id];if(!g)return;
 const previewing=g.preview!==null,shown=previewing?g.preview:g.current,frac=clamp(shown,0,60)/60;
 $('gaugeFill'+id).setAttribute('stroke-dasharray',(GAUGE_SPAN*frac).toFixed(2)+' 100');
 $('gauge'+id).classList.toggle('preview',previewing&&g.preview!==g.current);
 $('gauge'+id).classList.toggle('match',previewing&&g.preview===g.current);
 $('gauge'+id).classList.toggle('muted',!previewing&&g.muted);
 $('gaugeNum'+id).textContent=shown;
 $('gaugeCap'+id).textContent=previewing?(g.preview===g.current?'CURRENT LEVEL':'NOW '+g.current):g.caption;
 const point=level=>{const a=(135+270*clamp(level,0,60)/60)*Math.PI/180;return [(60+50*Math.cos(a)).toFixed(2),(60+50*Math.sin(a)).toFixed(2)]};
 const mark=$('gaugeMark'+id),[mx,my]=point(g.current),knob=$('gaugeKnob'+id),[kx,ky]=point(shown);
 mark.setAttribute('cx',mx);mark.setAttribute('cy',my);knob.setAttribute('cx',kx);knob.setAttribute('cy',ky);
 mark.toggleAttribute('hidden',!previewing||g.preview===g.current)}
function setGauge(id,current,muted,caption){const g=gaugeState[id]||(gaugeState[id]={preview:null,committed:false});
 if(g.committed){g.preview=null;g.committed=false}
 Object.assign(g,{current,muted,caption});paintGauge(id)}
function bindPreview(sliderId,gaugeId){const slider=$(sliderId);
 slider.addEventListener('input',()=>{const g=gaugeState[gaugeId];if(!g)return;g.preview=Number(slider.value);g.committed=false;paintGauge(gaugeId)});
 const end=()=>{const g=gaugeState[gaugeId];if(!g||g.preview===null)return;g.committed=true;if(g.preview===g.current){g.preview=null;g.committed=false;paintGauge(gaugeId)}};
 ['change','pointerup','touchend','blur','knobend'].forEach(type=>slider.addEventListener(type,end))}
// The gauge ring is the volume control. The centre never reacts, so the page can be scrolled over it.
// Touch drags must start on the ring near the knob and never jump on a tap; a mouse can click anywhere on the ring.
function bindKnob(gaugeId,inputId){const gauge=$('gauge'+gaugeId),input=$(inputId);let dragging=false,start=null;
 const levelAt=e=>{const r=gauge.getBoundingClientRect(),deg=Math.atan2(e.clientY-r.top-r.height/2,e.clientX-r.left-r.width/2)*180/Math.PI;let rel=(deg-135+720)%360;if(rel>270)rel=rel>315?0:270;return Math.round(rel/270*60)};
 const onRing=e=>{const r=gauge.getBoundingClientRect(),R=r.width*50/120,d=Math.hypot(e.clientX-r.left-r.width/2,e.clientY-r.top-r.height/2);return d>R*.7&&d<R*1.3};
 const nearKnob=e=>Math.abs(levelAt(e)-Number(input.value))<=7;
 const grabbable=(e,touch)=>onRing(e)&&(!touch||nearKnob(e));
 gauge.addEventListener('touchstart',e=>{if(e.touches.length===1&&grabbable(e.touches[0],true))e.preventDefault()},{passive:false});
 const apply=e=>{const prev=Number(input.value);let level=levelAt(e);
  if(dragging&&Math.abs(level-prev)>30)level=prev>30?60:0;// don't jump across the gap at the bottom
  input.value=level;if(!dragging||input.value!==String(prev))input.dispatchEvent(new Event('input',{bubbles:true}))};
 gauge.addEventListener('pointerdown',e=>{const touch=e.pointerType!=='mouse';if(e.button>0||!grabbable(e,touch))return;e.preventDefault();gauge.setPointerCapture(e.pointerId);start=input.value;dragging=false;if(!touch)apply(e);dragging=true;gauge.classList.add('dragging')});
 gauge.addEventListener('pointermove',e=>{if(dragging)apply(e)});
 const end=()=>{if(!dragging)return;dragging=false;gauge.classList.remove('dragging');input.dispatchEvent(new Event(input.value!==start?'change':'knobend',{bubbles:true}))};
 gauge.addEventListener('pointerup',end);gauge.addEventListener('pointercancel',end)}
$('masterGauge').innerHTML=gaugeMarkup('master',false,'master','Master');
const channels=$('channels');
names.forEach((name,i)=>{
 const pair=pairs.find(p=>p.indices[0]===i);
 if(pair){const combined=document.createElement('div');combined.className='channel';combined.id='pairCard'+pair.id;
 combined.innerHTML=`${tileHead(pair.name,'pairMute'+pair.id,'pairReset'+pair.id)}${gaugeMarkup('pair'+pair.id,true,'pair'+pair.id,pair.name)}<div class="stepper"><button id="pairDown${pair.id}" class="step-btn" type="button" aria-label="Decrease ${pair.name} level">−</button><span style="flex:1"></span><button id="pairUp${pair.id}" class="step-btn" type="button" aria-label="Increase ${pair.name} level">+</button></div><p id="pairNote${pair.id}" class="pair-note"></p>`;
 channels.appendChild(combined)}
 const div=document.createElement('div');div.className='channel';
 div.id='channelCard'+i;div.hidden=i===0||i===1||i===4||i===5;
 div.innerHTML=`${tileHead(name,'cmute'+i,'reset'+i)}${gaugeMarkup('ch'+i,true,'channel'+i,name)}<div class="stepper"><button id="channelDown${i}" class="step-btn" type="button" aria-label="Decrease ${name} level">−</button><span style="flex:1"></span><button id="channelUp${i}" class="step-btn" type="button" aria-label="Increase ${name} level">+</button></div>`;
 channels.appendChild(div);
 const gain=document.createElement('div');gain.className='field';
 gain.innerHTML=`<label for="gain${i}">${name} output gain <output id="gainValue${i}" class="gain-value"></output></label><div class="stepper"><button id="gainDown${i}" class="step-btn" type="button" aria-label="Decrease ${name} gain">−</button><input id="gain${i}" type="range" min="0" max="14" step="2"><button id="gainUp${i}" class="step-btn" type="button" aria-label="Increase ${name} gain">+</button></div>`;
 $('channelGains').appendChild(gain);
});
function db(n){return (n>0?'+':'')+n+' dB'}
function clamp(n,lo,hi){return Math.min(hi,Math.max(lo,n))}
// Must match volumeLevel() in AmpRemoteControl.ino: levels 20-60 use 1 dB steps
// (-40..0 dB), levels 0-20 use 3 dB steps down to -99 dB.
function volumeLevel(attenuation){return attenuation<=40?60-attenuation:Math.max(0,20-Math.floor((attenuation-39)/3))}
function volumeAttenuation(level){return level>=20?60-level:Math.min(99,40+(20-level)*3)}
function toneLevel(dbValue){return (dbValue+14)/2}
function toneDb(level){return level*2-14}
function channelAttenuation(s,i){return clamp(s.master+s.trims[i],s.limit,99)}
function setChannels(s,indices,level){const n=Math.max(s.limit,volumeAttenuation(clamp(level,0,60)));indices.forEach(i=>s.trims[i]=n-s.master)}
function field(id,value){if(document.activeElement!==$(id))$(id).value=value}
function button(id,on){$(id).classList.toggle('active',on);$(id).setAttribute('aria-pressed',String(on))}
function channelMuteButton(id,on,masterMuted){button(id,on);$(id).classList.toggle('active',masterMuted||on);$(id).title=masterMuted?'Master mute is on; this button controls the channel mute setting.':$(id).getAttribute('aria-label')}
function render(data){snapshot=data;const s=data.state;
 if(data.masterUrl)$('masterLink').href=data.masterUrl;
 renderLights(data.knobLights);
 $('connection').textContent='ESP32 CONNECTED';$('connection').className='connection status';
 $('warning').textContent=data.warning+(data.pendingSave?' Settings save pending.':'');
 $('homeStatus').textContent=(data.homeStatus||'Home Wi-Fi status unavailable')+(data.homeAddress?' · '+data.homeAddress:'');
 const namesList=data.presets||[];
 if(JSON.stringify(Array.from($('preset').options,option=>option.value))!==JSON.stringify(['',...namesList])){
   const placeholder=document.createElement('option');placeholder.value='';placeholder.textContent='No preset selected';
   $('preset').replaceChildren(placeholder,...namesList.map(name=>{const option=document.createElement('option');option.value=name;option.textContent=name;return option}));
 }
 if(!namesList.includes(selectedPreset))selectedPreset='';
 $('preset').value=selectedPreset;$('selectedPreset').textContent=selectedPreset||'No preset selected';
 $('presetStatus').textContent=namesList.length?'':'No saved presets yet.';
 $('preset').disabled=!namesList.length;
 ['presetApply','presetUpdate','presetRename','presetDelete'].forEach(id=>$(id).disabled=!selectedPreset);
 $('rampStatus').textContent=s.mute?'Muted':data.ramping?'Adjusting':'Ready';
 setGauge('master',volumeLevel(s.master),s.mute,s.mute?'MUTED':'');$('master').max=volumeLevel(s.limit);field('master',volumeLevel(s.master));button('mute',s.mute);
 ['bass','treble'].forEach(k=>{field(k,toneLevel(s[k]));$(k+'Value').textContent=toneLevel(s[k])});
 field('inputGain',s.inputGain);$('inputGainValue').textContent=db(s.inputGain);
 ['limit','startup'].forEach(k=>field(k,volumeLevel(s[k])));field('step',s.step);field('rampMs',s.rampMs);
 ['bypass','restore'].forEach(k=>button(k,s[k]));
 names.forEach((_,i)=>{const target=channelAttenuation(s,i),muted=s.mute||s.mutes[i];$('channel'+i).max=volumeLevel(s.limit);field('channel'+i,volumeLevel(target));field('gain'+i,s.gains[i]);setGauge('ch'+i,volumeLevel(target),muted,muted?'MUTED':'');$('gainValue'+i).textContent=db(s.gains[i]);channelMuteButton('cmute'+i,s.mutes[i],s.mute)});
 pairs.forEach(p=>{const [a,b]=p.indices,first=channelAttenuation(s,a),second=channelAttenuation(s,b),mixed=first!==second,level=Math.round((volumeLevel(first)+volumeLevel(second))/2);const muteA=s.mutes[a],muteB=s.mutes[b],muted=s.mute||muteA&&muteB;setGauge('pair'+p.id,level,muted,mixed?'MIXED':muted?'MUTED':muteA||muteB?'PART MUTED':'');$('pair'+p.id).max=volumeLevel(s.limit);field('pair'+p.id,level);$('pair'+p.id).setAttribute('aria-valuetext',mixed?'Mixed levels; adjusting matches both':String(level));$('pairNote'+p.id).textContent=mixed?'Mixed levels; adjusting sets both equal.':'';channelMuteButton('pairMute'+p.id,muteA&&muteB,s.mute);if(muteA!==muteB)$('pairMute'+p.id).setAttribute('aria-pressed','mixed')})}
// Per-boot token changes when the receiver restarts; any reconnect reloads the page for a clean state.
let bootToken=null,offline=false,reloading=false;
async function request(path,options={}){const ctrl=new AbortController(),timer=setTimeout(()=>ctrl.abort(),4000);
 try{return await fetch(path,{...options,cache:'no-store',signal:ctrl.signal})}
 catch(e){offline=true;throw Error('Receiver not reachable')}
 finally{clearTimeout(timer)}}
async function get(){const r=await request('/api/state');if(!r.ok)throw Error('Connection failed');const data=await r.json();
 if(offline||(bootToken&&data.token!==bootToken)){if(!reloading){reloading=true;$('connection').textContent='RECONNECTED · RELOADING';location.reload()}throw Error('Reconnected; reloading page')}
 bootToken=data.token;return data}
async function refresh(){if(busy||polling)return;polling=true;try{const data=await get();if(!busy&&(!snapshot||data.revision>=snapshot.revision))render(data);nextPoll=Date.now()+(data.ramping?250:2000)}catch(e){if(reloading)return;$('connection').textContent='ESP32 OFFLINE';$('connection').className='connection offline';renderLights(null);nextPoll=Date.now()+2000}finally{polling=false}}
function enqueue(build,key='',priority=false,onSuccess=null){if(key)queue=queue.filter(job=>job.key!==key);const job={build,key,priority,onSuccess};if(priority){const i=queue.findIndex(item=>!item.priority);if(i<0)queue.push(job);else queue.splice(i,0,job)}else queue.push(job);drain()}
function change(edit,key='',priority=false){enqueue(current=>{edit(current.state);return {path:'/api/state',body:{revision:current.revision,state:current.state}}},key,priority)}
async function drain(){if(busy)return;busy=true;while(queue.length){const job=queue.shift();try{const current=await get(),jobRequest=job.build(current);const r=await request(jobRequest.path,{method:'POST',headers:{'Content-Type':'application/json','X-Receiver-Token':current.token},body:JSON.stringify(jobRequest.body)});if(!r.ok)throw Error(await r.text());if(job.onSuccess)job.onSuccess();render(await get())}catch(e){$('warning').textContent=e.message;try{render(await get());$('warning').textContent=e.message}catch(_){}}}busy=false;nextPoll=0}
['inputGain','step','rampMs'].forEach(k=>$(k).onchange=e=>{const n=Number(e.target.value);change(s=>s[k]=n)});
['bass','treble'].forEach(k=>{$(k).onchange=e=>{const n=toneDb(Number(e.target.value));change(s=>s[k]=n)};$(k+'Down').onclick=()=>change(s=>s[k]=toneDb(clamp(toneLevel(s[k])-1,0,14)));$(k+'Up').onclick=()=>change(s=>s[k]=toneDb(clamp(toneLevel(s[k])+1,0,14)))});
function setLimit(s,level){const n=volumeAttenuation(clamp(level,0,60));s.limit=n;s.master=Math.max(s.master,n);s.startup=Math.max(s.startup,n)}
function setStartup(s,level){s.startup=Math.max(s.limit,volumeAttenuation(clamp(level,0,60)))}
$('limit').onchange=e=>{const n=Number(e.target.value);change(s=>setLimit(s,n),'limit',true)};
$('startup').onchange=e=>{const n=Number(e.target.value);change(s=>setStartup(s,n),'startup')};
['Down','Up'].forEach((direction,i)=>{const delta=i?1:-1;
 $('limit'+direction).onclick=()=>change(s=>setLimit(s,volumeLevel(s.limit)+delta),'',delta<0);
 $('startup'+direction).onclick=()=>change(s=>setStartup(s,volumeLevel(s.startup)+delta));
 $('step'+direction).onclick=()=>change(s=>s.step=clamp(s.step+delta,1,10));
 $('rampMs'+direction).onclick=()=>change(s=>s.rampMs=clamp(s.rampMs+delta*5,5,1000));
 $('inputGain'+direction).onclick=()=>change(s=>s.inputGain=clamp(s.inputGain+delta*2,0,14));
});
$('master').onchange=e=>{const n=volumeAttenuation(Number(e.target.value));change(s=>s.master=Math.max(s.limit,n),'master')};
$('up').onclick=()=>change(s=>s.master=Math.max(s.limit,volumeAttenuation(Math.min(60,volumeLevel(s.master)+s.step))));
$('down').onclick=()=>change(s=>s.master=Math.max(s.limit,volumeAttenuation(Math.max(0,volumeLevel(s.master)-s.step))));
['mute','bypass','restore'].forEach(k=>$(k).onclick=()=>change(s=>s[k]=!s[k],'',k==='mute'));
$('flat').onclick=()=>change(s=>{s.bass=0;s.treble=0;s.bypass=false});
$('balance').onclick=()=>change(s=>s.trims.fill(0));
$('channelView').onclick=()=>{individualView=!individualView;button('channelView',individualView);$('channelView').textContent=individualView?'Combined view':'Individual view';pairs.forEach(p=>{$('pairCard'+p.id).hidden=individualView;p.indices.forEach(i=>$('channelCard'+i).hidden=!individualView)})};
pairs.forEach(p=>{
 $('pair'+p.id).onchange=e=>{const level=Number(e.target.value);change(s=>setChannels(s,p.indices,level),'pair'+p.id)};
 ['Down','Up'].forEach((direction,j)=>{$('pair'+direction+p.id).onclick=()=>change(s=>{const level=Math.round(p.indices.reduce((sum,i)=>sum+volumeLevel(channelAttenuation(s,i)),0)/p.indices.length);setChannels(s,p.indices,level+(j?1:-1)*s.step)})});
 $('pairMute'+p.id).onclick=()=>change(s=>{const mute=!p.indices.every(i=>s.mutes[i]);p.indices.forEach(i=>s.mutes[i]=mute)},'',true);
 $('pairReset'+p.id).onclick=()=>change(s=>p.indices.forEach(i=>{s.trims[i]=0;s.mutes[i]=false}));
});
names.forEach((_,i)=>{
 $('channel'+i).onchange=e=>{const n=volumeAttenuation(Number(e.target.value));change(s=>s.trims[i]=Math.max(s.limit,n)-s.master,'channel'+i)};
 ['Down','Up'].forEach((direction,j)=>{const delta=j?1:-1;$('channel'+direction+i).onclick=()=>change(s=>{const current=clamp(s.master+s.trims[i],s.limit,99);const n=volumeAttenuation(clamp(volumeLevel(current)+delta*s.step,0,60));s.trims[i]=Math.max(s.limit,n)-s.master});$('gain'+direction+i).onclick=()=>change(s=>s.gains[i]=clamp(s.gains[i]+delta*2,0,14))});
 $('gain'+i).onchange=e=>{const n=Number(e.target.value);change(s=>s.gains[i]=n)};
 $('cmute'+i).onclick=()=>change(s=>s.mutes[i]=!s.mutes[i],'',true);
 $('reset'+i).onclick=()=>change(s=>{s.trims[i]=0;s.mutes[i]=false});
});
function presetAction(action,name,newName='',onSuccess=null){enqueue(current=>({path:'/api/presets',body:{revision:current.revision,action,name,newName}}),'',false,onSuccess)}
function applyPreset(name){if(name&&confirm('Applying a preset can change channel gain, tone and loudness. Continue?'))presetAction('apply',name,'',()=>{selectedPreset=name})}
$('preset').onchange=e=>{const name=e.target.value;e.target.value=selectedPreset;applyPreset(name)};
$('presetApply').onclick=()=>applyPreset(selectedPreset);
$('presetUpdate').onclick=()=>{const name=$('preset').value;if(name)presetAction('update',name)};
$('presetCreate').onclick=()=>presetAction('create',$('presetName').value.trim());
$('presetRename').onclick=()=>{const name=$('preset').value;if(name)presetAction('rename',name,$('presetName').value.trim())};
$('presetDelete').onclick=()=>{const name=$('preset').value;if(name&&confirm('Delete the selected preset?'))presetAction('delete',name)};
$('backup').onclick=async()=>{try{const data=await get();const url=URL.createObjectURL(new Blob([JSON.stringify(data.state,null,2)],{type:'application/json'}));const a=document.createElement('a');a.href=url;a.download='receiver-settings.json';a.click();URL.revokeObjectURL(url)}catch(e){$('warning').textContent=e.message}};
$('restoreFile').onclick=()=>$('file').click();
$('file').onchange=async e=>{try{const f=e.target.files[0];if(!f)return;if(f.size>4096)throw Error('Backup is too large');const restored=JSON.parse(await f.text());restored.mute=true;change(s=>{Object.assign(s,restored);s.lrMulti=true;s.surroundMulti=true;s.rec=false})}catch(err){$('warning').textContent='Backup error: '+err.message}finally{e.target.value=''}};
bindPreview('master','master');names.forEach((_,i)=>bindPreview('channel'+i,'ch'+i));pairs.forEach(p=>bindPreview('pair'+p.id,'pair'+p.id));
bindKnob('master','master');names.forEach((_,i)=>bindKnob('ch'+i,'channel'+i));pairs.forEach(p=>bindKnob('pair'+p.id,'pair'+p.id));
refresh();setInterval(()=>{if(Date.now()>=nextPoll)refresh()},250);
</script></body></html>
)HTML";
