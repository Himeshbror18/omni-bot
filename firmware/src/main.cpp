#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID = "Omni-Bot";
const char* AP_PASSWORD = "omnibot123";
WebServer server(80);

struct Motor { uint8_t pwm, in1, in2; bool invert; };
Motor FL{13,18,19,false}, FR{14,21,22,true}, RL{16,23,25,false}, RR{17,26,27,true};
constexpr uint8_t STBY_A = 32, STBY_B = 33;

const char INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<meta name="theme-color" content="#0b0d10">
<title>Omni-Bot Controller</title>
<style>
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
body{margin:0;background:#0b0d10;color:#f2f4f7;font-family:system-ui,-apple-system,sans-serif;touch-action:none}
main{width:min(680px,100%);margin:auto;padding:16px 14px 28px}
header{display:flex;align-items:center;justify-content:space-between;margin-bottom:14px}
h1{font-size:24px;margin:0}.status{font-size:12px;color:#6ee7a0}
.panel{background:#151922;border:1px solid #252b36;border-radius:20px;padding:16px;margin:12px 0}
.label{display:flex;justify-content:space-between;color:#b8bec9;font-size:14px}
input[type=range]{width:100%;accent-color:#f2f4f7}
.grid{display:grid;grid-template-columns:repeat(3,1fr);gap:9px}
button{border:1px solid #303642;background:#202631;color:#fff;border-radius:16px;min-height:68px;font-size:26px;font-weight:650;user-select:none}
button:active{transform:scale(.97);background:#343b48}
.stop{background:#a92d38;border-color:#c74450;font-size:16px}.stop:active{background:#c74450}
.rotate{display:grid;grid-template-columns:1fr 1fr;gap:9px;margin-top:9px}
.rotate button{min-height:58px;font-size:18px}
.motors{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.motor{background:#1b202a;border-radius:15px;padding:12px}.motor b{display:block;margin-bottom:5px}
.value{font-size:12px;color:#aeb5c0}
.note{color:#858d99;font-size:12px;line-height:1.5;margin:8px 2px 0}
</style>
</head>
<body>
<main>
<header><h1>🚗 Omni-Bot</h1><span class="status" id="status">● Connected</span></header>

<section class="panel">
<div class="label"><span>Drive speed</span><b id="speedValue">70%</b></div>
<input id="speed" type="range" min="0" max="100" value="70">
</section>

<section class="panel">
<div class="grid">
<button data-c="fl">↖</button><button data-c="f">↑</button><button data-c="fr">↗</button>
<button data-c="l">←</button><button class="stop" id="stop">STOP</button><button data-c="r">→</button>
<button data-c="bl">↙</button><button data-c="b">↓</button><button data-c="br">↘</button>
</div>
<div class="rotate"><button data-c="ccw">↺ Rotate left</button><button data-c="cw">Rotate right ↻</button></div>
<p class="note">Press and hold a direction to drive. Releasing the button stops the robot.</p>
</section>

<section class="panel">
<h3>Motor test</h3>
<div class="motors">
<div class="motor"><b>Front Left</b><input id="m0" type="range" min="-100" max="100" value="0"><span class="value" id="v0">0%</span></div>
<div class="motor"><b>Front Right</b><input id="m1" type="range" min="-100" max="100" value="0"><span class="value" id="v1">0%</span></div>
<div class="motor"><b>Rear Left</b><input id="m2" type="range" min="-100" max="100" value="0"><span class="value" id="v2">0%</span></div>
<div class="motor"><b>Rear Right</b><input id="m3" type="range" min="-100" max="100" value="0"><span class="value" id="v3">0%</span></div>
</div>
<p class="note">Use these controls with the wheels lifted during initial wiring tests.</p>
</section>
</main>

<script>
const $=s=>document.querySelector(s);
const speed=$('#speed');
speed.oninput=()=>$('#speedValue').textContent=speed.value+'%';

let active=false;
function api(url){
 fetch(url).catch(()=>{$('#status').textContent='● Connection lost';$('#status').style.color='#ff6875'});
}
function drive(c){active=true;api('/move?c='+encodeURIComponent(c)+'&v='+speed.value)}
function stop(){if(active){active=false;api('/move?c=s&v=0')}}

document.querySelectorAll('[data-c]').forEach(btn=>{
 const start=e=>{e.preventDefault();drive(btn.dataset.c)};
 const end=e=>{e.preventDefault();stop()};
 btn.addEventListener('pointerdown',start);
 btn.addEventListener('pointerup',end);
 btn.addEventListener('pointercancel',end);
 btn.addEventListener('pointerleave',end);
});
$('#stop').addEventListener('pointerdown',e=>{e.preventDefault();active=false;api('/move?c=s&v=0')});

for(let i=0;i<4;i++){
 const el=$('#m'+i),val=$('#v'+i);
 el.oninput=()=>{val.textContent=el.value+'%';api('/motor?i='+i+'&v='+el.value)};
}
window.addEventListener('blur',stop);
document.addEventListener('visibilitychange',()=>{if(document.hidden)stop()});
</script>
</body>
</html>
)HTML";

void setupMotor(Motor &m){
  pinMode(m.in1,OUTPUT);
  pinMode(m.in2,OUTPUT);
  ledcAttach(m.pwm,20000,8);
  ledcWrite(m.pwm,0);
}

void writeMotor(Motor &m,int value){
  value=constrain(value,-255,255);
  if(m.invert) value=-value;
  if(value==0){
    digitalWrite(m.in1,LOW); digitalWrite(m.in2,LOW); ledcWrite(m.pwm,0); return;
  }
  digitalWrite(m.in1,value>0);
  digitalWrite(m.in2,value<0);
  ledcWrite(m.pwm,abs(value));
}

void setAll(int fl,int fr,int rl,int rr){
  writeMotor(FL,fl); writeMotor(FR,fr); writeMotor(RL,rl); writeMotor(RR,rr);
}
void stopAll(){setAll(0,0,0,0);}

void moveRobot(const String &c,int percent){
  percent=constrain(percent,0,100);
  int x=0,y=0,r=0;
  if(c=="f") x=percent;
  else if(c=="b") x=-percent;
  else if(c=="l") y=-percent;
  else if(c=="r") y=percent;
  else if(c=="fl"){x=percent;y=-percent;}
  else if(c=="fr"){x=percent;y=percent;}
  else if(c=="bl"){x=-percent;y=-percent;}
  else if(c=="br"){x=-percent;y=percent;}
  else if(c=="cw") r=percent;
  else if(c=="ccw") r=-percent;
  else {stopAll();return;}

  int fl=x-y-r;
  int fr=x+y+r;
  int rl=x+y-r;
  int rr=x-y+r;
  int maxMagnitude=max(100,max(abs(fl),max(abs(fr),max(abs(rl),abs(rr)))));
  setAll(fl*255/maxMagnitude,fr*255/maxMagnitude,rl*255/maxMagnitude,rr*255/maxMagnitude);
}

void setup(){
  Serial.begin(115200);
  pinMode(STBY_A,OUTPUT); pinMode(STBY_B,OUTPUT);
  digitalWrite(STBY_A,HIGH); digitalWrite(STBY_B,HIGH);

  setupMotor(FL); setupMotor(FR); setupMotor(RL); setupMotor(RR);
  stopAll();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID,AP_PASSWORD);
  Serial.print("Controller: http://"); Serial.println(WiFi.softAPIP());

  server.on("/",[](){server.send_P(200,"text/html",INDEX_HTML);});
  server.on("/move",[](){
    moveRobot(server.arg("c"),server.arg("v").toInt());
    server.send(200,"text/plain","OK");
  });
  server.on("/motor",[](){
    int i=server.arg("i").toInt();
    int v=constrain(server.arg("v").toInt(),-100,100)*255/100;
    if(i==0) writeMotor(FL,v);
    else if(i==1) writeMotor(FR,v);
    else if(i==2) writeMotor(RL,v);
    else if(i==3) writeMotor(RR,v);
    server.send(200,"text/plain","OK");
  });
  server.begin();
}

void loop(){server.handleClient();}
