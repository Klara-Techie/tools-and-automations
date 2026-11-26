#pragma once

const char DASHBOARD[] PROGMEM = R"HOMEHTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta name="theme-color" content="#0b0c0f">
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
.section:has(.remote) .section-head h2{color:var(--lilac)}
.section:has(#otherDevices) .section-head h2{color:var(--sea)}
.tag{
  color:var(--muted);font-size:10px;letter-spacing:.04em;text-transform:uppercase;
  padding:4px 7px;border-radius:5px;white-space:nowrap;
  background:#1b1e24;border:1px solid var(--line);
}
.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px}
.card,.master-panel,.remote,.knob-panel{
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
.master-controls{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:8px}
.master-controls .primary,.master-controls .timer-row button{padding:7px 4px;font-size:11px;line-height:1.2}
.primary{font-weight:650;background:#252a33;border-color:#505967;color:#d7dee8}
.primary.is-active{
  background:#2c333d;
  border-color:#8995a5;
  color:#f1f2f4;
}
.master-timer{min-width:0;grid-column:span 2}
.master-timer .timer-row{grid-template-columns:repeat(2,minmax(0,1fr));gap:8px;margin:0}
.master-timer input{text-align:center}
.remote{display:grid;grid-template-columns:1fr;overflow:hidden;border-top:2px solid #929ba9}
.level{
  display:flex;align-items:center;justify-content:space-between;gap:12px;
  padding:8px 12px;border-bottom:1px solid var(--line);min-width:0;
}
.level:last-child{border-bottom:0}
.level-name{display:flex;align-items:center;flex-wrap:wrap;gap:4px 8px;min-width:0;font-weight:600;font-size:13px}
.level-feedback{min-width:0;border-left:1px solid #505967;padding-left:8px;font-size:11px;font-weight:400;color:#b9c9de;overflow-wrap:anywhere}
.level-feedback:empty{display:none}
.level-actions{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));width:104px;flex-shrink:0;border:1px solid #505967;border-radius:7px;overflow:hidden;background:#20232a}
.level-actions button{border:0;border-radius:0;background:transparent;font-size:23px;font-weight:500;line-height:1;padding:6px;color:var(--text)}
.level-actions button+button{border-left:1px solid var(--line)}
.level-actions button:last-child{background:#242a34;color:#edf2f8}
.level-actions button:focus-visible{outline-offset:-4px}
.level-actions button:active:not(:disabled){
  background:#394350;
  color:#f4f6fa;
}
.ir-feedback{min-height:18px;margin-top:7px;color:var(--sea);font-size:12px}
.knob-panel{padding:12px;border-top:2px solid #929ba9}
.knob-head{display:flex;align-items:center;justify-content:space-between;gap:10px;margin-bottom:12px}
.knob-head h2{margin:0;font:20px Georgia,"Times New Roman",serif;color:var(--text)}
.knob-head p{margin:4px 0 0;font-size:12px;color:var(--muted)}
.knob-layout{display:grid;grid-template-columns:56px minmax(0,1fr) 52px;align-items:center;gap:10px}
.knob-toggle{position:relative;display:grid;justify-items:center;justify-self:center;gap:6px;font-size:11px;color:var(--muted);cursor:pointer;touch-action:manipulation}
.knob-switch{position:absolute;width:1px;height:1px;padding:0;margin:-1px;overflow:hidden;clip-path:inset(50%);white-space:nowrap;border:0}
.knob-switch:focus-visible+.knob-ring{outline:2px solid var(--accent);outline-offset:4px}
.knob-toggle:has(.knob-switch:disabled){opacity:.45;cursor:not-allowed}
.knob-ring{position:relative;display:block;width:56px;height:56px;--ring-color:#8be0d5}
.knob-ring::before{content:"";position:absolute;inset:12px;border:1px solid #697583;border-radius:50%;background:#292e37}
.knob-ring svg{position:absolute;inset:0;margin:auto;width:22px;height:22px;color:var(--muted)}
.knob-ring.is-on svg{color:var(--text)}
.knob-pixel{position:absolute;left:50%;top:50%;width:6px;height:6px;border-radius:50%;background:#476173}
.knob-ring.is-on .knob-pixel{background:var(--ring-color)}
.knob-panel fieldset{border:0;padding:0;margin:0;min-width:0}
.knob-fields{display:contents}
.knob-fields label{display:flex;flex-direction:column;gap:7px;min-width:0;font-size:12px;color:var(--muted)}
.knob-fields input[type=color],.knob-fields select{width:100%;height:44px;min-width:0;padding:6px;border:1px solid #414753;border-radius:7px;background:#0e1015;color:var(--text)}
.knob-fields select{font-size:16px}
.knob-brightness{grid-column:1/-1}
.knob-brightness span{display:flex;justify-content:space-between;gap:10px}
.knob-brightness output{color:var(--accent);font-variant-numeric:tabular-nums}
.knob-brightness input{width:100%;height:44px;margin:0;accent-color:var(--accent)}
@media(hover:hover) and (pointer:fine){button:hover:not(:disabled){border-color:#e0e6ed}}
@media(max-width:374px){.timer-row .countdown{font-size:10px}}
@media(max-width:350px){.card{padding:10px}.status{font-size:9px;padding:4px}.timer-row{grid-template-columns:minmax(0,1fr) 58px}.master-copy strong{font-size:13px}}
@media(min-width:760px){
  main{padding:24px 20px 40px}
  .grid{grid-template-columns:repeat(4,minmax(0,1fr))}
  .remote{grid-template-columns:repeat(5,minmax(0,1fr))}
  .level{display:block;border-bottom:0;border-right:1px solid var(--line)}
  .level:last-child{border-right:0}
  .level-actions{margin-top:8px;width:100%}
}
</style>
</head>
<body><main>
<header><div class="brand"><div class="logo" aria-hidden="true"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" stroke-linejoin="round"><circle cx="12" cy="12" r="9"/><path d="m16 8-2 6-6 2 2-6Zm-4-7v3m0 16v3M1 12h3m16 0h3"/></svg></div><div><div class="eyebrow">Black Pearl</div><h1>Captain Karthik Sparroww 🏴‍☠️</h1></div></div></header>
<div class="hero">
  <div class="eyebrow hero-kicker">Aboard the</div>
  <div class="voyage-lockup">
  <h2 class="voyage-name"><span>Black</span> <span>Pearl</span></h2>
  <svg class="voyage-art" viewBox="0 0 520 340" aria-hidden="true" focusable="false">
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
        <div class="timer-row"><div class="timer-input"><input id="allMinutes" type="number" min="1" max="10080" inputmode="numeric" placeholder="Min" aria-label="TV, amplifier and LED off timer minutes" disabled><span id="allCountdown" class="countdown" role="timer" aria-label="TV, amplifier and LED remaining off time" hidden></span></div><button id="allStart" class="start" disabled>START</button><button id="allCancel" class="cancel" aria-label="Cancel all TV, amplifier and LED strip timers" hidden disabled>CANCEL ALL</button></div>
      </div>
    </div>
  </div>
  <div id="livingDevices" class="grid"></div>
</section>
<section class="section">
  <div class="section-head"><div><h2>Sound system</h2></div><span id="irStatus" class="tag">IR offline</span></div>
  <div class="remote">
    <div class="level"><div class="level-name"><span>Master</span><span class="level-feedback" role="status" aria-live="polite" aria-label="Master command status"></span></div><div class="level-actions"><button data-ir="master_minus" aria-label="Master volume down" disabled>−</button><button data-ir="master_plus" aria-label="Master volume up" disabled>+</button></div></div>
    <div class="level"><div class="level-name"><span>Front</span><span class="level-feedback" role="status" aria-live="polite" aria-label="Front command status"></span></div><div class="level-actions"><button data-ir="front_minus" aria-label="Front volume down" disabled>−</button><button data-ir="front_plus" aria-label="Front volume up" disabled>+</button></div></div>
    <div class="level"><div class="level-name"><span>Surround</span><span class="level-feedback" role="status" aria-live="polite" aria-label="Surround command status"></span></div><div class="level-actions"><button data-ir="surround_minus" aria-label="Surround volume down" disabled>−</button><button data-ir="surround_plus" aria-label="Surround volume up" disabled>+</button></div></div>
    <div class="level"><div class="level-name"><span>Centre</span><span class="level-feedback" role="status" aria-live="polite" aria-label="Centre command status"></span></div><div class="level-actions"><button data-ir="centre_minus" aria-label="Centre volume down" disabled>−</button><button data-ir="centre_plus" aria-label="Centre volume up" disabled>+</button></div></div>
    <div class="level"><div class="level-name"><span>Subwoofer</span><span class="level-feedback" role="status" aria-live="polite" aria-label="Subwoofer command status"></span></div><div class="level-actions"><button data-ir="sub_minus" aria-label="Subwoofer volume down" disabled>−</button><button data-ir="sub_plus" aria-label="Subwoofer volume up" disabled>+</button></div></div>
  </div>
</section>
<section class="section">
  <div class="section-head"><div><h2>Bedroom controls</h2></div><span class="tag">4 devices</span></div>
  <div id="otherDevices" class="grid"></div>
</section>
<section class="section" aria-labelledby="knobHeading">
  <div id="knobPanel" class="knob-panel">
    <div class="knob-head"><div><h2 id="knobHeading">Knob lighting</h2><p>Both knobs · 10 LEDs each</p></div><span id="knobStatus" class="tag">Unavailable</span></div>
    <fieldset id="knobFields" disabled aria-label="Shared knob lighting settings">
      <div class="knob-layout">
        <label class="knob-toggle">
          <input id="knobOn" class="knob-switch" type="checkbox" role="switch" aria-label="Power for both knob rings">
          <span class="knob-ring" aria-hidden="true"><svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round"><path d="M12 3v9m-5-7a8 8 0 1 0 10 0"/></svg></span>
        </label>
        <div class="knob-fields">
          <label class="knob-pattern"><select id="knobPattern" aria-label="Pattern"><option value="solid">Solid</option><option value="breathing">Breathing</option><option value="rainbow">Rainbow</option><option value="chase">Chase</option><option value="comet">Comet</option><option value="color_cycle">Colour cycle</option><option value="aurora">Aurora</option><option value="ocean">Ocean waves</option><option value="fire">Fire glow</option><option value="candle">Candlelight</option><option value="twinkle">Starlight twinkle</option><option value="pendulum">Pendulum</option></select></label>
          <label><input id="knobColor" type="color" value="#8be0d5" aria-label="Colour"></label>
          <label class="knob-brightness"><span>Brightness<output id="knobBrightnessValue" for="knobBrightness">40%</output></span><input id="knobBrightness" type="range" min="0" max="100" value="40" title="Percent of the configured LED brightness ceiling"></label>
        </div>
      </div>
    </fieldset>
    <div id="knobFeedback" class="ir-feedback" role="status" aria-live="polite"></div>
  </div>
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
  card.innerHTML='<div class="card-head"><button type="button" class="icon" disabled></button><span class="status">Connecting</span></div><h3></h3>'+
    (d.timer?'<div class="timer"><div class="timer-row"><div class="timer-input"><input type="number" min="1" max="10080" inputmode="numeric" placeholder="Min" disabled><span class="countdown" role="timer" hidden></span></div><button class="start" disabled>START</button><button class="cancel" hidden disabled>CANCEL</button></div></div>':'');
  card.querySelector('.icon').innerHTML='<svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true" focusable="false">'+icons[d.icon]+'</svg>';card.querySelector('h3').textContent=d.name;
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
  document.getElementById('irStatus').textContent=data.irOnline?'IR ready':'IR offline';
  document.querySelectorAll('[data-ir]').forEach(b=>b.disabled=!data.irOnline);
  renderKnobLights(data.knobLights);
  updateCountdowns();
}
async function refresh(){
  if(loading){refreshAgain=true;return}loading=true;
  try{const r=await fetch('/api/state',{cache:'no-store'});if(!r.ok)throw Error('Dashboard status unavailable');render(await r.json());if(connectionError){message.textContent='';connectionError=false}}
  catch(e){latest=null;connectionError=true;message.textContent=e.message;document.querySelectorAll('button,input,select').forEach(el=>el.disabled=true);document.querySelectorAll('.timer-row').forEach(row=>setTimerControls(row,false,false));document.querySelectorAll('.status').forEach(el=>{el.textContent='Offline';el.classList.remove('on')});document.querySelectorAll('.card').forEach(el=>el.classList.remove('is-on'));document.querySelectorAll('.icon').forEach(el=>{el.classList.remove('on');el.removeAttribute('aria-pressed')});document.getElementById('allToggle').classList.remove('is-active');document.getElementById('irStatus').textContent='IR offline';renderKnobLights(null)}
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
let irQueue=Promise.resolve(),irPending=0,irFeedbackRevision=0;
function sendIR(button){
  const operation=button.getAttribute('aria-label');
  const feedback=button.closest('.level').querySelector('.level-feedback');
  const action=button.dataset.ir.endsWith('_plus')?'Volume up':'Volume down';
  const revision=++irFeedbackRevision;
  const setFeedback=text=>{if(revision===irFeedbackRevision)feedback.textContent=text};
  document.querySelectorAll('.level-feedback').forEach(el=>el.textContent='');
  irPending++;
  setFeedback(action+(irPending>1?' · queued':' · sending...'));
  irQueue=irQueue.then(async()=>{
    setFeedback(action+' · sending...');
    const r=await fetch('/api/ir',{method:'POST',headers:{'X-Requested-With':'XMLHttpRequest','Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams({command:button.dataset.ir})});
    const text=await r.text();if(!r.ok)throw Error(text||'IR request failed');
    setFeedback(action+' · sent');
  }).catch(e=>{
    message.textContent=operation+' not confirmed: '+e.message;
    setFeedback(action+' · failed');
  }).finally(()=>{irPending--});
}
document.querySelectorAll('[data-ir]').forEach(b=>b.onclick=()=>sendIR(b));
const knobFields=document.getElementById('knobFields');
const knobOn=document.getElementById('knobOn'),knobColor=document.getElementById('knobColor');
const knobPattern=document.getElementById('knobPattern'),knobBrightness=document.getElementById('knobBrightness');
const knobFeedback=document.getElementById('knobFeedback');
let knobDirty=false,knobBusy=false,knobRevision=0,knobTimer=null;
const automaticKnobColors=['rainbow','color_cycle','aurora','ocean','fire','candle'];
for(const ring of document.querySelectorAll('.knob-ring')){
  for(let i=0;i<10;i++){const pixel=document.createElement('span');pixel.className='knob-pixel';pixel.style.transform='translate(-50%,-50%) rotate('+i*36+'deg) translateY(-22px)';ring.append(pixel)}
}
function updateKnobPreview(){
  document.getElementById('knobBrightnessValue').textContent=knobBrightness.value+'%';
  document.querySelectorAll('.knob-ring').forEach(ring=>{
    ring.style.setProperty('--ring-color',knobColor.value);
    ring.classList.toggle('is-on',!knobFields.disabled&&knobOn.checked&&Number(knobBrightness.value)>0);
  });
  knobColor.disabled=knobFields.disabled||automaticKnobColors.includes(knobPattern.value);
  knobColor.title=automaticKnobColors.includes(knobPattern.value)?'This pattern chooses its own colours':'Shared ring colour';
}
function renderKnobLights(settings){
  const available=!!(settings&&settings.available);
  knobFields.disabled=!available;
  knobFields.querySelectorAll('button,input,select').forEach(el=>el.disabled=knobFields.disabled);
  document.getElementById('knobStatus').textContent=!available?'Unavailable':settings.on?'On':'Off';
  if(available&&!knobDirty&&!knobBusy){
    knobOn.checked=settings.on;knobColor.value=settings.color;
    knobPattern.value=settings.pattern;knobBrightness.value=settings.brightness;
  }
  updateKnobPreview();
}
knobFields.addEventListener('input',event=>{
  knobDirty=true;knobRevision++;
  clearTimeout(knobTimer);
  knobFeedback.textContent='Updating...';updateKnobPreview();
  knobTimer=setTimeout(()=>{
    knobTimer=null;sendKnobLights();
  },event.target===knobOn||event.target===knobPattern?0:300);
});
async function sendKnobLights(){
  if(knobBusy||!knobDirty)return;
  if(!latest||!latest.knobLights||!latest.knobLights.available){knobFeedback.textContent='Knob LEDs unavailable; change a setting to retry when online';return}
  const brightness=Number(knobBrightness.value),pattern=knobPattern.value;
  if(!/^#[0-9a-f]{6}$/i.test(knobColor.value)||!Number.isInteger(brightness)||brightness<0||brightness>100||
     ![...knobPattern.options].some(option=>option.value===pattern)){knobFeedback.textContent='Invalid knob lighting settings';return}
  const values={on:knobOn.checked?1:0,color:knobColor.value.slice(1),brightness,pattern};
  const revision=knobRevision;
  knobBusy=true;knobFeedback.textContent='Updating...';message.textContent='';
  try{
    const r=await fetch('/api/knob-lights',{method:'POST',headers:{'X-Requested-With':'XMLHttpRequest','Content-Type':'application/x-www-form-urlencoded'},body:new URLSearchParams(values)});
    const text=await r.text();if(!r.ok)throw Error(text||'Knob lighting request failed');
    if(revision===knobRevision){knobDirty=false;knobFeedback.textContent='Both knobs updated'}
  }catch(e){knobFeedback.textContent='Not confirmed: '+e.message+'. Change a setting to retry.';message.textContent=e.message}
  finally{
    knobBusy=false;await refresh();renderKnobLights(latest&&latest.knobLights);
    if(knobDirty&&knobRevision!==revision&&knobTimer===null)sendKnobLights();
  }
}
function connectUpdates(){
  const ws=new WebSocket('ws://'+location.hostname+':81/');
  ws.onmessage=e=>{if(e.data==='refresh')refresh()};
  ws.onclose=()=>setTimeout(connectUpdates,3000);
}
refresh();connectUpdates();setInterval(refresh,10000);setInterval(updateCountdowns,1000);
</script></body></html>
)HOMEHTML";
