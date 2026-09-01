#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* AP_SSID="Omni-Bot";
const char* AP_PASSWORD="omnibot123";
WebServer server(80);

struct Motor{uint8_t pwm,in1,in2;bool invert;};
Motor FL{13,18,19,false},FR{14,21,22,true},RL{16,23,25,false},RR{17,26,27,true};
constexpr uint8_t STBY_A=32,STBY_B=33;

const char INDEX_HTML[] PROGMEM=R"HTML(
<!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Omni-Bot</title><style>
body{font-family:system-ui;margin:0;background:#111;color:#eee;text-align:center}main{max-width:620px;margin:auto;padding:18px}
.pad{display:grid;grid-template-columns:repeat(3,1fr);gap:10px}button{font-size:20px;padding:18px;border:0;border-radius:14px;background:#292929;color:#fff}
button:active{background:#555}.stop{background:#a22}.slider{width:90%}.motors{display:grid;grid-template-columns:1fr 1fr;gap:12px}
.card{background:#1d1d1d;padding:12px;border-radius:14px}small{color:#aaa}
</style></head><body><main><h1>🚗 Omni-Bot</h1><p>ESP32 Web Controller</p>
<label>Speed: <b id="sv">70</b>%</label><br><input class="slider" id="speed" type="range" min="0" max="100" value="70"><br><br>
<div class="pad"><button data-c="fl">↖</button><button data-c="f">↑</button><button data-c="fr">↗</button>
<button data-c="l">←</button><button class="stop" data-c="s">STOP</button><button data-c="r">→</button>
<button data-c="bl">↙</button><button data-c="b">↓</button><button data-c="br">↘</button></div><br>
<div class="pad"><button data-c="ccw">↺</button><button data-c="s">STOP</button><button data-c="cw">↻</button></div>
<h2>Individual motors</h2><div class="motors">
<div class="card">FL<br><input id="m0" type="range" min="-100" max="100" value="0"><br><small id="v0">0%</small></div>
<div class="card">FR<br><input id="m1" type="range" min="-100" max="100" value="0"><br><small id="v1">0%</small></div>
<div class="card">RL<br><input id="m2" type="range" min="-100" max="100" value="0"><br><small id="v2">0%</small></div>
<div class="card">RR<br><input id="m3" type="range" min="-100" max="100" value="0"><br><small id="v3">0%</small></div>
</div></main><script>
const q=x=>document.querySelector(x),speed=q('#speed');speed.oninput=()=>q('#sv').textContent=speed.value;
function go(c){fetch('/move?c='+c+'&v='+speed.value)}
document.querySelectorAll('[data-c]').forEach(b=>{b.onpointerdown=()=>go(b.dataset.c);b.onpointerup=()=>go('s');b.onpointerleave=()=>go('s')});
for(let i=0;i<4;i++){let e=q('#m'+i),v=q('#v'+i);e.oninput=()=>{v.textContent=e.value+'%';fetch('/motor?i='+i+'&v='+e.value)}}
</script></body></html>)HTML";

void setupMotor(Motor&m){pinMode(m.in1,OUTPUT);pinMode(m.in2,OUTPUT);ledcAttach(m.pwm,20000,8);ledcWrite(m.pwm,0);}
void writeMotor(Motor&m,int v){
 v=constrain(v,-255,255);if(m.invert)v=-v;
 if(v==0){digitalWrite(m.in1,LOW);digitalWrite(m.in2,LOW);ledcWrite(m.pwm,0);return;}
 digitalWrite(m.in1,v>0);digitalWrite(m.in2,v<0);ledcWrite(m.pwm,abs(v));
}
void setAll(int a,int b,int c,int d){writeMotor(FL,a);writeMotor(FR,b);writeMotor(RL,c);writeMotor(RR,d);}
void stopAll(){setAll(0,0,0,0);}
void moveRobot(String c,int v){
 v=constrain(v,0,100);int x=0,y=0,r=0;
 if(c=="f")x=v;else if(c=="b")x=-v;else if(c=="l")y=-v;else if(c=="r")y=v;
 else if(c=="fl"){x=v;y=-v;}else if(c=="fr"){x=v;y=v;}else if(c=="bl"){x=-v;y=-v;}else if(c=="br"){x=-v;y=v;}
 else if(c=="cw")r=v;else if(c=="ccw")r=-v;else{stopAll();return;}
 int a=x-y-r,b=x+y+r,d=x+y-r,e=x-y+r;
 int mx=max(100,max(abs(a),max(abs(b),max(abs(d),abs(e)))));
 setAll(a*255/mx,b*255/mx,d*255/mx,e*255/mx);
}
void setup(){
 Serial.begin(115200);pinMode(STBY_A,OUTPUT);pinMode(STBY_B,OUTPUT);digitalWrite(STBY_A,HIGH);digitalWrite(STBY_B,HIGH);
 setupMotor(FL);setupMotor(FR);setupMotor(RL);setupMotor(RR);stopAll();
 WiFi.mode(WIFI_AP);WiFi.softAP(AP_SSID,AP_PASSWORD);
 Serial.print("Controller: http://");Serial.println(WiFi.softAPIP());
 server.on("/",[](){server.send_P(200,"text/html",INDEX_HTML);});
 server.on("/move",[](){moveRobot(server.arg("c"),server.arg("v").toInt());server.send(200,"text/plain","OK");});
 server.on("/motor",[](){int i=server.arg("i").toInt(),v=server.arg("v").toInt()*255/100;
   if(i==0)writeMotor(FL,v);else if(i==1)writeMotor(FR,v);else if(i==2)writeMotor(RL,v);else if(i==3)writeMotor(RR,v);
   server.send(200,"text/plain","OK");});
 server.begin();
}
void loop(){server.handleClient();}
