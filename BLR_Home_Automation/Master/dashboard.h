#pragma once

const char DASHBOARD[] PROGMEM = R"HOMEHTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#081526">
<title>Black Pearl · Bedroom</title>
<style>
:root{
  color-scheme:dark;
  --bg:#0b0c0f;
  --surface:#15171c;
  --surface-elev:#20232a;
  --line:#30343d;
  --text:#f1f2f4;
  --muted:#a5adba;
  --accent:#c5ccd6;
  --sea:#c5ccd6;
  --sand:#d4d8df;
  --lilac:#c4c9d2;
  --danger:#e58e8e;
}
*{box-sizing:border-box}
[hidden]{display:none!important}
body{
  margin:0;
  color:var(--text);
  font:14px/1.45 "Segoe UI",-apple-system,BlinkMacSystemFont,Arial,sans-serif;
  background:radial-gradient(ellipse at 50% -20%,rgba(145,155,175,.075),transparent 55%),var(--bg);
  min-height:100vh;
}
button,input,select{font:inherit}
button{
  cursor:pointer;
  min-width:44px;
  min-height:44px;
  color:var(--text);
  border:1px solid var(--line);
  background:var(--surface-elev);
  border-radius:7px;
  padding:8px 10px;
  touch-action:manipulation;
}
button:focus-visible,input:focus-visible,select:focus-visible{outline:2px solid var(--accent);outline-offset:3px}
button:disabled,input:disabled,select:disabled{opacity:.45;cursor:not-allowed}
main{max-width:1000px;margin:auto;padding:16px 12px 32px}
header{display:flex;align-items:center;border-bottom:1px solid var(--line);padding-bottom:12px}
.brand{display:flex;gap:10px;align-items:center;min-width:0}
.logo{
  width:40px;height:40px;flex-shrink:0;display:grid;place-items:center;
  border:1px solid #414753;
  border-radius:12px;
  background:#252932;
  color:var(--sand);font-size:24px;
}
.eyebrow,.label{
  font-size:9px;letter-spacing:.18em;font-weight:700;text-transform:uppercase;
  color:var(--sand);
}
h1{font:clamp(17px,4.6vw,23px)/1.25 Georgia,"Times New Roman",serif;letter-spacing:-.02em;margin:3px 0;overflow-wrap:anywhere}
.subtitle,.muted{color:var(--muted)}
.hero{position:relative;isolation:isolate;overflow:hidden;display:grid;justify-items:center;gap:6px;margin:16px 0 20px;padding:12px;text-align:center;border:1px solid #30343d;border-radius:14px;background:radial-gradient(ellipse at 75% 30%,rgba(145,155,175,.1),transparent 55%),linear-gradient(125deg,#20242b,#15181e)}
.hero-kicker{letter-spacing:.24em}
.voyage-lockup{position:relative;isolation:isolate;width:100%;max-width:220px;aspect-ratio:26/17}
.voyage-art{position:absolute;inset:0;display:block;width:100%;height:100%;pointer-events:none}
.voyage-name{position:absolute;inset:auto 0 0;z-index:1;display:flex;justify-content:center;gap:.16em;margin:0;font:italic clamp(26px,7vw,34px)/1 Georgia,"Times New Roman",serif;letter-spacing:-.045em;color:#edf0f5;text-shadow:0 2px 0 #12151b}
.notice{min-height:22px;color:var(--danger);font-size:13px;margin:12px 0}
.notice:empty{display:none}
.section{margin-top:22px}
.section-head{display:flex;align-items:center;justify-content:space-between;gap:8px;margin-bottom:10px}
.section-head h2{margin:0;font:20px Georgia,"Times New Roman",serif;color:var(--sand)}
.section:has(#otherDevices) .section-head h2{color:var(--sea)}
.tag{
  color:var(--muted);font-size:10px;letter-spacing:.04em;text-transform:uppercase;
  padding:4px 7px;border-radius:5px;white-space:nowrap;
  background:#1b1e24;border:1px solid var(--line);
}
.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}
.card,.master-panel{
  background:linear-gradient(145deg,#191c22,#13161b);
  border:1px solid var(--line);
  border-radius:10px;
}
.card{padding:12px;min-width:0;border-top:2px solid #555b66}
.card.is-on{border-color:#929ba9;background:linear-gradient(145deg,#242831,#171a20)}
.card-head{display:flex;justify-content:space-between;align-items:center;gap:6px}
.icon{
  width:44px;height:44px;flex-shrink:0;padding:0;border-radius:10px;
  background:#252932;border:1px solid #414753;color:#c5ccd6;
  display:grid;place-items:center;font-size:21px;
}
.icon.on{
  background:#303844;
  border-color:#a6afbe;
  color:#f4f6fa;
}
.icon svg,.logo svg{width:24px;height:24px;display:block;pointer-events:none}
.status{min-width:0;overflow-wrap:anywhere;font-size:10px;letter-spacing:.04em;text-transform:uppercase;color:var(--muted);font-weight:700;padding:4px 6px;background:#252932;border:1px solid transparent;border-radius:4px;text-align:right}
.status.on{color:#e4e9f1;background:#353e4a;border-color:#68798e}
.card h3{margin:10px 0 0;font-size:14px}
.timer{border-top:1px solid var(--line);margin-top:12px;padding-top:10px}
.timer-row{display:grid;grid-template-columns:minmax(0,1fr) 70px;gap:6px;margin-top:7px}
.timer-input{min-width:0;display:grid}
.timer-row input,.timer-row .countdown{
  min-width:0;width:100%;
  min-height:44px;border:1px solid #414753;background:#0e1015;color:var(--text);
  border-radius:6px;padding:7px;font-size:16px;
}
.timer-row .countdown{display:grid;align-items:center;padding:7px 2px;color:var(--sea);font-size:12px;font-weight:600;font-variant-numeric:tabular-nums;white-space:nowrap;text-align:center}
.timer-row button{min-width:0;padding:7px 5px;font-size:10px;font-weight:650;letter-spacing:.03em;white-space:nowrap}
.timer-row .start{
  background:#252a33;
  border-color:#505967;
  color:#d7dee8;
}
.timer-row .cancel{margin:0;background:#302329;border-color:#76515a;color:#f1b3b8}
#allCancel{white-space:normal}
.master-panel{display:grid;gap:10px;padding:12px;margin-bottom:10px;border-left:3px solid #aeb5c0}
.master-copy{display:flex;align-items:center;justify-content:space-between;gap:8px;min-width:0}
.master-copy strong{display:block;font-size:14px}
.master-copy .muted{font-size:11px;white-space:nowrap}
.receiver-link{display:inline-block;padding:2px 8px;border:1px solid var(--line);border-radius:8px;background:#0b1d2e;color:var(--muted);font-size:12px;line-height:16px;text-decoration:none}
.receiver-link.is-online{border-color:#347493;background:#14344b;color:#d7effb}
.receiver-link[aria-disabled=true]{cursor:not-allowed}
.receiver-link:focus-visible{outline:2px solid var(--accent);outline-offset:3px}
.master-controls{display:grid;grid-template-columns:minmax(0,1fr) minmax(0,1.5fr);gap:8px}
.master-controls .primary,.master-controls .timer-row button{padding:7px 4px;font-size:11px;line-height:1.2}
.primary{font-weight:650;background:#252a33;border-color:#505967;color:#d7dee8}
.primary.is-active{
  background:#2c333d;
  border-color:#8995a5;
  color:#f1f2f4;
}
.master-timer{min-width:0}
.master-timer .timer-row{grid-template-columns:minmax(0,1fr) 56px;gap:8px;margin:0}
.master-timer input{text-align:center}
@media(hover:hover) and (pointer:fine){button:hover:not(:disabled){border-color:#e0e6ed}}
@media(max-width:374px){.timer-row .countdown{font-size:10px}}
@media(max-width:350px){.card{padding:10px}.status{font-size:9px;padding:4px}.timer-row{grid-template-columns:minmax(0,1fr) 58px}.master-copy strong{font-size:13px}}
@media(min-width:760px){
  main{padding:24px 20px 40px}
  .grid{grid-template-columns:repeat(4,minmax(0,1fr))}
  .level{display:block;border-bottom:0;border-right:1px solid var(--line)}
  .level:last-child{border-right:0}
  .level-actions{margin-top:8px;width:100%}
}
:root{--bg:#081526;--surface:#0d1f31;--surface-elev:#10243a;--line:rgba(151,190,220,.13);--text:#edf6fc;--muted:#a6bbc9;--accent:#56c8f5;--sand:#56c8f5;--sea:#56c8f5;--lilac:#89d4f4;--danger:#ff8c94}
body{font:14px/1.5 "Segoe UI",-apple-system,BlinkMacSystemFont,Arial,sans-serif;background:radial-gradient(ellipse at 50% -18%,#173b5b 0,transparent 46%),var(--bg)}
main{max-width:1100px;padding:24px 20px 52px}
header{border:0;padding:0;justify-content:space-between;gap:12px}
.brand{gap:13px}
.logo{width:48px;height:48px;border-radius:16px;background:linear-gradient(145deg,#173b5b,#0d1f31);border-color:#56c8f544;color:#edf6fc;box-shadow:0 6px 24px #56c8f51c}
.hero{
  display:grid;grid-template-columns:minmax(0,1fr) minmax(100px,40%);align-items:center;
  gap:8px;min-height:0;margin:0 0 30px;padding:20px 24px;text-align:left;
  border:1px solid var(--line);border-radius:24px;
  background:radial-gradient(circle at 75% 20%,#56c8f526,transparent 36%),linear-gradient(155deg,#173650,#0a1b2b);
  box-shadow:0 20px 50px #0004;
}
.hero header{grid-column:1/-1;width:100%;padding-bottom:10px;border-bottom:1px solid #ffffff25}
.hero h1{font:600 clamp(17px,3vw,23px)/1.2 "Trebuchet MS","Segoe UI",sans-serif;letter-spacing:-.03em;color:var(--text)}
.hero-copy{position:relative;z-index:2;max-width:420px}
.hero-copy h2{margin:0;font:italic clamp(34px,6vw,52px)/1 Georgia,serif;letter-spacing:-.04em;color:var(--text)}
.voyage-lockup{width:100%;max-width:240px;align-self:center;justify-self:center}
.voyage-art{filter:saturate(1.3) brightness(1.3)}
.section{margin-top:30px}
.section-head{margin-bottom:13px}
.section-head h2{font:600 19px/1.2 "Segoe UI",sans-serif;letter-spacing:-.03em;color:var(--text)}
.section:has(#otherDevices) .section-head h2{color:var(--text)}
.section-head h2::after{content:"";display:inline-block;vertical-align:middle;width:6px;height:6px;margin-left:10px;border-radius:50%;background:var(--sand)}
.section:has(#otherDevices) .section-head h2::after{background:var(--sea)}
.tag{padding:6px 10px;border-radius:100px;background:#0b1b2b;border-color:var(--line);color:var(--muted);letter-spacing:.07em}
.grid{gap:12px}
.card,.master-panel{border:1px solid var(--line);border-radius:18px;background:linear-gradient(155deg,#142b43,#0d1f31);box-shadow:0 12px 28px #0002,inset 0 1px #ffffff08}
.card{--device-accent:#56c8f5;padding:16px;min-height:115px;border-top:1px solid #56c8f526;transition:background .2s,border-color .2s,transform .2s}
.card:nth-child(4n+2){--device-accent:#89d4f4}
.card:nth-child(4n+3){--device-accent:#6abde9}
.card:nth-child(4n+4){--device-accent:#92cfe3}
.card.is-on{border-color:var(--device-accent);background:linear-gradient(155deg,#173650,#10283c)}
@supports (color:color-mix(in srgb,red,transparent)){
  .card.is-on{border-color:color-mix(in srgb,var(--device-accent) 48%,#24445f);background:radial-gradient(circle at 12% 0%,color-mix(in srgb,var(--device-accent) 16%,transparent),transparent 56%),linear-gradient(155deg,#173650,#10283c)}
  .icon.on{background:color-mix(in srgb,var(--device-accent) 18%,#10243a);border-color:color-mix(in srgb,var(--device-accent) 65%,transparent)}
  .status.on{background:color-mix(in srgb,var(--device-accent) 10%,transparent);border-color:color-mix(in srgb,var(--device-accent) 35%,transparent)}
}
.card h3{margin:13px 0 0;font-size:15px;letter-spacing:-.01em}
.icon{width:58px;height:50px;border-radius:16px;background:#0b1d2e;border-color:var(--line);color:#a6bbc9}
.icon.on{color:var(--device-accent);box-shadow:0 0 18px color-mix(in srgb,var(--device-accent) 15%,transparent)}
.status{display:inline-flex;align-items:center;gap:6px;border-radius:100px;padding:5px 9px;background:#091a2a;color:var(--muted)}
.status::before{content:"";width:6px;height:6px;flex-shrink:0;border-radius:50%;background:#69788a}
.status.on{color:var(--device-accent)}
.status.on::before{background:var(--device-accent);box-shadow:0 0 8px var(--device-accent)}
.timer{margin-top:15px;padding-top:13px;border-color:var(--line)}
.timer-row{grid-template-columns:minmax(0,1fr) 52px;gap:8px}
.timer-row input,.timer-row .countdown{border-radius:10px;border-color:var(--line);background:#081827}
button{border-radius:11px;transition:filter .15s,transform .15s,border-color .15s}
button:active:not(:disabled){transform:scale(.97)}
.timer-row .start,.primary{background:#14344b;border-color:#347493;color:#d7effb;font-weight:700}
.timer-row .cancel{border-color:#ff8c9455;background:#402b36;color:#ffd2d4}
.master-panel{border-left:3px solid var(--accent);padding:16px;margin-bottom:12px}
.master-copy strong{font-size:15px}
.master-copy .muted{color:var(--muted)}
.primary.is-active{background:#194660;border-color:#56c8f5;color:#edf6fc}
@media(hover:hover) and (pointer:fine){.card:hover{transform:translateY(-2px)}button:hover:not(:disabled){border-color:var(--accent);filter:brightness(1.13)}}
@media(max-width:600px){
  main{padding:18px 14px 40px}
  .hero{padding:14px 18px;gap:6px;border-radius:20px}
  .hero h1{font-size:17px}
  .hero-copy h2{font-size:32px}
  .voyage-lockup{max-width:160px}
  .section{margin-top:25px}
  .card{padding:12px}
  .icon{width:58px;height:46px}
  .card h3{font-size:14px}
  .level{padding:10px 12px}
}
@media(max-width:350px){.hero-copy h2{font-size:29px}.timer-row{grid-template-columns:minmax(0,1fr) 52px}}
</style>
</head>
<body><main>
<div class="hero">
  <header><div class="brand"><div class="logo" aria-hidden="true">☠️</div><div><h1>Captain Karthik Sparroww 🏴‍☠️</h1></div></div></header>
  <div class="hero-copy"><h2>Black Pearl 🥂</h2></div>
  <div class="voyage-lockup">
  <svg class="voyage-art" viewBox="75 0 385 255" aria-hidden="true" focusable="false">
    <defs>
      <radialGradient id="moonGlow"><stop stop-color="#b9e2ef" stop-opacity=".2"/><stop offset="1" stop-color="#b9e2ef" stop-opacity="0"/></radialGradient>
      <linearGradient id="moonReflection" x2="0" y2="1"><stop stop-color="#99d5df" stop-opacity=".22"/><stop offset="1" stop-color="#99d5df" stop-opacity="0"/></linearGradient>
      <radialGradient id="moonFace" cx=".35" cy=".3"><stop stop-color="#e7ece1"/><stop offset="1" stop-color="#9fbfcb"/></radialGradient>
      <linearGradient id="nightSail" x2="1" y2=".7"><stop stop-color="#3d5969"/><stop offset=".5" stop-color="#263f50"/><stop offset="1" stop-color="#112839"/></linearGradient>
      <linearGradient id="nightHull" x2="0" y2="1"><stop stop-color="#1b3546"/><stop offset="1" stop-color="#071724"/></linearGradient>
    </defs>
    <circle cx="395" cy="73" r="95" fill="url(#moonGlow)"/>
    <circle cx="395" cy="73" r="37" fill="url(#moonFace)" opacity=".86"/>
    <path d="M370 178H420L464 310H326Z" fill="url(#moonReflection)"/>
    <g fill="none" stroke="#bad9e4" stroke-linecap="round" opacity=".12">
      <path d="M305 72Q361 58 436 72T520 68M331 89Q407 78 486 90" stroke-width="8"/>
      <path d="M0 238Q120 222 231 239T520 234M0 258Q128 241 264 258T520 250M60 276Q198 259 340 275" stroke-width="2"/>
    </g>
    <g stroke="#86b9c9" stroke-opacity=".55" stroke-width="1.5" stroke-linejoin="round">
      <path d="M120 210Q242 219 397 210L369 236Q280 250 175 238L145 229Z" fill="url(#nightHull)"/>
      <path d="M156 210V199H370V210M213 210V65M278 210V30M343 210V76" fill="none"/>
      <path d="M184 78H244Q233 103 244 122Q214 130 184 122Q196 102 184 78ZM171 132H251Q237 165 251 187Q211 196 171 187Q187 161 171 132Z" fill="url(#nightSail)"/>
      <path d="M252 48H307Q294 69 307 89Q280 96 252 89Q265 70 252 48ZM239 101H320Q305 128 320 150Q280 160 239 150Q255 125 239 101ZM232 161H324Q311 184 324 202Q278 211 232 202Q246 181 232 161Z" fill="url(#nightSail)"/>
      <path d="M318 88H369Q357 110 369 129Q344 138 318 129Q331 110 318 88ZM311 141H378Q364 164 378 188Q344 197 311 188Q325 165 311 141Z" fill="url(#nightSail)"/>
      <path d="M278 30L314 38 278 46Z" fill="#0b2233"/>
      <path d="M213 65L141 210M278 30L175 210M278 30L369 210M343 76L397 210M120 210L95 193" fill="none" stroke-opacity=".4"/>
      <path d="M171 222H366" fill="none" stroke-opacity=".4"/>
    </g>
    <g fill="#a2c6d3" opacity=".35"><circle cx="198" cy="226" r="2"/><circle cx="223" cy="226" r="2"/><circle cx="248" cy="226" r="2"/><circle cx="273" cy="226" r="2"/><circle cx="298" cy="226" r="2"/><circle cx="323" cy="226" r="2"/></g>
  </svg>
  </div>
</div>
<div id="message" class="notice" role="status" aria-live="polite"></div>
<section class="section">
  <div class="section-head"><div><h2>Entertainment</h2></div><span class="tag">4 devices</span></div>
  <div class="master-panel">
    <div class="master-copy"><strong>Entertainment power</strong><span class="muted">3 devices</span></div>
    <div class="master-controls">
      <button id="allToggle" class="primary" title="Controls TV, amplifier and LED strip" disabled>All on</button>
      <div class="master-timer">
        <div class="timer-row"><div class="timer-input"><input id="allMinutes" type="number" min="1" max="10080" inputmode="numeric" placeholder="Min" aria-label="TV, amplifier and LED off timer minutes" disabled><span id="allCountdown" class="countdown" role="timer" aria-label="TV, amplifier and LED remaining off time" hidden></span></div><button id="allStart" class="start" aria-label="Start TV, amplifier and LED off timer" disabled>START</button><button id="allCancel" class="cancel" aria-label="Cancel all TV, amplifier and LED strip timers" hidden disabled>CANCEL ALL</button></div>
      </div>
    </div>
  </div>
  <div id="livingDevices" class="grid"></div>
</section>
<section class="section">
  <div class="section-head"><div><h2>Bedroom controls</h2></div><span class="tag">4 devices</span></div>
  <div id="otherDevices" class="grid"></div>
</section>
</main>
<script>
const icons={
  tv:'<rect x="3" y="5" width="18" height="13" rx="2"/><path d="m8 2 4 3 4-3M8 21h8m-4-3v3"/>',
  amp:'<rect x="3" y="4" width="18" height="16" rx="2"/><circle cx="8" cy="12" r="2"/><path d="M14 8h4m-4 4h4m-4 4h4"/>',
  strip:'<rect x="2" y="8" width="20" height="8" rx="2"/><path d="M6 11v2m4-2v2m4-2v2m4-2v2M2 12H0m22 0h2"/>',
  socket:'<rect x="4" y="3" width="16" height="18" rx="4"/><path d="M9 9v3m6-3v3m-5 5h4"/>',
  fan:'<circle cx="12" cy="12" r="2"/><path d="M12 10C7 6 10 1 14 3c3 2 1 5-1 7"/><path d="M12 10C7 6 10 1 14 3c3 2 1 5-1 7" transform="rotate(120 12 12)"/><path d="M12 10C7 6 10 1 14 3c3 2 1 5-1 7" transform="rotate(240 12 12)"/>',
  light:'<path d="M9 17v-2a6 6 0 1 1 6 0v2Zm0 4h6"/>',
  heater:'<rect x="5" y="2" width="14" height="18" rx="3"/><circle cx="12" cy="7" r="2"/><path d="M8 20v2m8-2v2M8 14q2-3 4 0t4 0"/>'
};
const definitions=[
  {id:0,name:'TV',icon:'tv',timer:true,groupMaster:true},
  {id:1,name:'Amplifier',icon:'amp',timer:true,groupMaster:true},
  {id:2,name:'LED strip',icon:'strip',timer:true,groupMaster:true},
  {id:5,name:'Socket',icon:'socket',timer:true},
  {id:3,name:'Fan',icon:'fan'},
  {id:4,name:'Light',icon:'light'},
  {id:6,name:'Socket 2',icon:'socket'},
  {id:7,name:'Water heater',icon:'heater'}
];
const message=document.getElementById('message');
let latest=null,receivedAt=0,loading=false,refreshAgain=false,connectionError=false;
for(const d of definitions){
  const card=document.createElement('article');card.className='card';card.id='device-'+d.id;
  card.innerHTML='<div class="card-head"><button type="button" class="icon" disabled></button><span class="status">Connecting</span></div>'+
    (d.id===1?'<h3><a class="receiver-link" role="link" tabindex="0" aria-disabled="true" aria-label="AVR control, receiver offline" title="Receiver offline">AVR control</a></h3>':'<h3></h3>')+
    (d.timer?'<div class="timer"><div class="timer-row"><div class="timer-input"><input type="number" min="1" max="10080" inputmode="numeric" placeholder="Min" disabled><span class="countdown" role="timer" hidden></span></div><button class="start" disabled>START</button><button class="cancel" hidden disabled>CANCEL</button></div></div>':'');
  card.querySelector('.icon').innerHTML='<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true" focusable="false">'+icons[d.icon]+'</svg>';if(d.id!==1)card.querySelector('h3').textContent=d.name;
  const toggle=card.querySelector('.icon'),status=card.querySelector('.status');
  status.id='status-'+d.id;toggle.setAttribute('aria-label',d.name+' power');toggle.setAttribute('aria-describedby',status.id);
  toggle.onclick=()=>{const state=latest&&latest.devices.find(x=>x.id===d.id);if(state)post('/api/relay',{id:d.id,on:state.on?0:1})};
  if(d.timer){
    const input=card.querySelector('input');input.setAttribute('aria-label',d.name+' off timer minutes');
    card.querySelector('.countdown').setAttribute('aria-label',d.name+' remaining off time');
    card.querySelector('.start').setAttribute('aria-label','Start '+d.name+' off timer');
    card.querySelector('.cancel').setAttribute('aria-label','Cancel '+d.name+' off timer');
    card.querySelector('.start').onclick=()=>startTimer(String(d.id),input);
    card.querySelector('.cancel').onclick=()=>post('/api/timer',{target:String(d.id),action:'cancel'});
  }
  document.getElementById(d.timer?'livingDevices':'otherDevices').append(card);
}
function remaining(seconds){return Math.max(0,(seconds||0)-Math.floor((Date.now()-receivedAt)/1000))}
function formatTime(seconds){return [Math.floor(seconds/3600),Math.floor(seconds%3600/60),seconds%60].map(n=>String(n).padStart(2,'0')).join(':')}
function setTimerControls(row,available,running,seconds=0,canStart=false){
  const start=row.querySelector('.start'),cancel=row.querySelector('.cancel');
  const input=row.querySelector('input'),countdown=row.querySelector('.countdown');
  const showCountdown=available&&seconds>0;
  input.disabled=!available||running||!canStart;input.hidden=showCountdown;
  countdown.hidden=!showCountdown;countdown.textContent=showCountdown?formatTime(seconds):'';
  start.hidden=running;start.disabled=!available||!canStart;
  cancel.hidden=!running;cancel.disabled=!available||!running;
}
function updateCountdowns(){
  if(!latest)return;
  const allReady=definitions.filter(d=>d.groupMaster).every(d=>{const s=latest.devices.find(x=>x.id===d.id);return s&&s.online&&s.configured});
  const anyOn=definitions.some(d=>d.groupMaster&&latest.devices.some(s=>s.id===d.id&&s.on));
  const allSeconds=allReady?remaining(latest.allTimerRemaining):0;
  let any=allSeconds>0;
  for(const d of definitions.filter(x=>x.timer)){
    const state=latest.devices.find(x=>x.id===d.id), card=document.getElementById('device-'+d.id);
    const available=!!(state&&state.online&&state.configured);
    const seconds=available?remaining(state.timerRemaining):0;
    setTimerControls(card.querySelector('.timer-row'),available,seconds>0,seconds,!!(state&&state.on));
    if(d.groupMaster)any=any||seconds>0;
  }
  setTimerControls(document.querySelector('.master-timer .timer-row'),allReady,any,allSeconds,anyOn);
}
function renderReceiverLink(online,url){
  const link=document.querySelector('.receiver-link');
  link.classList.toggle('is-online',online);
  link.setAttribute('aria-disabled',String(!online));
  link.setAttribute('aria-label',online?'AVR control':'AVR control, receiver offline');
  link.title=online?'Open receiver controls (home Wi-Fi required)':'Receiver offline or amplifier power is off';
  if(online)link.setAttribute('href',url||'/receiver');else link.removeAttribute('href');
}
function render(data){
  latest=data;receivedAt=Date.now();
  for(const d of definitions){
    const state=data.devices.find(x=>x.id===d.id),card=document.getElementById('device-'+d.id);
    const available=state&&state.online&&state.configured,status=card.querySelector('.status'),toggle=card.querySelector('.icon');
    status.textContent=!state||!state.online?'Offline':!state.configured?'Not configured':state.on?'On':'Off';
    status.classList.toggle('on',!!(available&&state.on));toggle.disabled=!available;
    card.classList.toggle('is-on',!!(available&&state.on));
    toggle.classList.toggle('on',!!(available&&state.on));
    if(available)toggle.setAttribute('aria-pressed',state.on?'true':'false');else toggle.removeAttribute('aria-pressed');
  }
  const allReady=definitions.filter(d=>d.groupMaster).every(d=>{const s=data.devices.find(x=>x.id===d.id);return s&&s.online&&s.configured});
  document.getElementById('allToggle').disabled=!allReady;
  const allOn=allReady&&definitions.filter(d=>d.groupMaster).every(d=>data.devices.find(x=>x.id===d.id).on);
  document.getElementById('allToggle').textContent=allOn?'All off':'All on';
  document.getElementById('allToggle').setAttribute('aria-label',(allOn?'Turn off ':'Turn on ')+'TV, amplifier and LED strip');
  document.getElementById('allToggle').classList.toggle('is-active',allOn);
  const amp=data.devices.find(x=>x.id===1);
  renderReceiverLink(!!(data.receiverOnline&&amp&&amp.online&&amp.configured&&amp.on),data.receiverUrl);
  updateCountdowns();
}
async function refresh(){
  if(loading){refreshAgain=true;return}loading=true;
  try{const r=await fetch('/api/state',{cache:'no-store'});if(!r.ok)throw Error('Dashboard status unavailable');render(await r.json());if(connectionError){message.textContent='';connectionError=false}}
  catch(e){latest=null;connectionError=true;message.textContent=e.message;document.querySelectorAll('button,input,select').forEach(el=>el.disabled=true);document.querySelectorAll('.timer-row').forEach(row=>setTimerControls(row,false,false));document.querySelectorAll('.status').forEach(el=>{el.textContent='Offline';el.classList.remove('on')});document.querySelectorAll('.card').forEach(el=>el.classList.remove('is-on'));document.querySelectorAll('.icon').forEach(el=>{el.classList.remove('on');el.removeAttribute('aria-pressed')});document.getElementById('allToggle').classList.remove('is-active');renderReceiverLink(false)}
  finally{loading=false;if(refreshAgain){refreshAgain=false;refresh()}}
}
async function post(path,values){
  message.textContent='';
  try{
    const r=await fetch(path,{method:'POST',headers:{'X-Requested-With':'XMLHttpRequest','Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(values)});
    const text=await r.text();if(!r.ok)throw Error(text||'Request failed');
  }catch(e){message.textContent=e.message}finally{refresh()}
}
function startTimer(target,input){
  const minutes=Number(input.value);
  if(!Number.isSafeInteger(minutes)||minutes<1||minutes>10080){message.textContent='Enter whole minutes from 1 to 10080.';return}
  post('/api/timer',{target,action:'start',seconds:minutes*60});
}
document.getElementById('allToggle').onclick=()=>{
  if(!latest)return;
  const allOn=definitions.filter(d=>d.groupMaster).every(d=>{const state=latest.devices.find(x=>x.id===d.id);return state&&state.online&&state.configured&&state.on});
  post('/api/all',{on:allOn?0:1});
};
document.getElementById('allStart').onclick=()=>startTimer('all',document.getElementById('allMinutes'));
document.getElementById('allCancel').onclick=()=>post('/api/timer',{target:'all',action:'cancel'});
function connectUpdates(){
  const ws=new WebSocket('ws://'+location.hostname+':81/');
  ws.onmessage=e=>{if(e.data==='refresh')refresh()};
  ws.onclose=()=>setTimeout(connectUpdates,3000);
}
refresh();connectUpdates();setInterval(refresh,2000);setInterval(updateCountdowns,1000);
</script></body></html>
)HOMEHTML";
