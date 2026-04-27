#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "???????";
const char* password = "????????";

#define PIN 13
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

WebServer server(80);

int r=255, g=255, b=255, brilho=128, modo=0;
unsigned long ultimoMs = 0;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no' charset='UTF-8'>";
  html += "<style>";
  html += "body { margin:0; padding:0; background: #020205; color: white; font-family: 'Segoe UI', sans-serif; display: flex; align-items: center; justify-content: center; min-height: 100vh; overflow: hidden; }";
  html += ".bg { position: fixed; top: 0; left: 0; width: 100%; height: 100%; z-index: -1; background: radial-gradient(circle at center, #1b2735 0%, #090a0f 100%); }";
  html += ".stars { position: absolute; top: 0; left: 0; width: 200%; height: 200%; animation: moveStars 100s linear infinite; opacity: 0.4; }";
  html += "@keyframes moveStars { from { transform: translate(0,0); } to { transform: translate(-50%, -50%); } }";

  html += "h1 { font-size: 32px; margin: 0; color: #fff; text-transform: uppercase; letter-spacing: 5px; animation: pulse 3s ease-in-out infinite; }";
  html += "@keyframes pulse { 0%, 100% { text-shadow: 0 0 10px #7f5af0; transform: scale(1); } 50% { text-shadow: 0 0 30px #7f5af0, 0 0 40px #2cb67d; transform: scale(1.05); } }";

  html += ".card { background: rgba(255,255,255,0.07); padding: 25px; border-radius: 35px; backdrop-filter: blur(25px); border: 1px solid rgba(255,255,255,0.2); text-align: center; width: 85%; max-width: 380px; box-shadow: 0 0 60px rgba(0,0,0,0.8); }";
  html += "p { color: #94a1b2; font-size: 11px; letter-spacing: 3px; margin-bottom: 25px; text-transform: uppercase; font-weight: bold; }";

 
  html += ".color-grid { display: grid; grid-template-columns: repeat(4, 1fr); gap: 12px; margin-bottom: 15px; }";
  html += ".c-btn { width: 100%; aspect-ratio: 1; border-radius: 50%; border: 2px solid rgba(255,255,255,0.3); cursor: pointer; transition: transform 0.15s, box-shadow 0.15s, border 0.15s; }";
  html += ".c-btn:active { transform: scale(0.9); }";
  html += ".c-btn.ativo { border: 3px solid #ffffff; box-shadow: 0 0 14px 4px rgba(255,255,255,0.6); transform: scale(1.12); }";

  
  html += ".picker-row { display: flex; align-items: center; justify-content: center; gap: 12px; margin-bottom: 18px; background: rgba(255,255,255,0.05); padding: 12px 15px; border-radius: 20px; border: 1px solid rgba(255,255,255,0.1); }";
  html += "#colorPicker { width: 44px; height: 44px; border-radius: 50%; border: 2px solid rgba(255,255,255,0.3); cursor: pointer; padding: 0; background: none; transition: box-shadow 0.15s; }";
  html += "#colorPicker.ativo { border: 3px solid #ffffff; box-shadow: 0 0 14px 4px rgba(255,255,255,0.6); }";
  html += ".picker-label { color: #94a1b2; font-size: 10px; letter-spacing: 2px; text-transform: uppercase; font-weight: bold; }";

  html += ".slider-container { margin: 20px 0; background: rgba(255,255,255,0.05); padding: 15px; border-radius: 20px; border: 1px solid rgba(255,255,255,0.1); }";
  html += "input[type=range] { width: 100%; height: 10px; accent-color: #2cb67d; cursor: pointer; }";
  html += ".system-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }";

  
  html += ".s-btn { background: rgba(127,90,240,0.2); border: 1px solid rgba(127,90,240,0.4); color: white; padding: 14px; border-radius: 15px; font-size: 10px; font-weight: bold; cursor: pointer; transition: box-shadow 0.15s, border 0.15s; }";
  html += ".s-btn.ativo { border: 2px solid #7f5af0; box-shadow: 0 0 14px 4px rgba(127,90,240,0.5); }";
  html += ".btn-stop { grid-column: span 2; background: rgba(255,82,82,0.15); border-color: rgba(255,82,82,0.4); color: #ff8e8e; }";
  html += ".btn-stop.ativo { border: 2px solid #ff5252; box-shadow: 0 0 14px 4px rgba(255,82,82,0.5); }";
  html += "</style></head><body>";

  html += "<div class='bg'><div class='stars'></div></div>";
  html += "<div class='card'>";
  html += "<h1>Universo</h1><p>Controle de Galáxia</p>";

  
  html += "<div class='color-grid' id='colorGrid'>";
  String cores[] = {"#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF", "#FFFFFF", "#FF8000"};
  for(int i=0; i<8; i++) {
    html += "<div class='c-btn' id='cb" + String(i) + "' style='background:" + cores[i] + "' onclick='vib(); setCol(\"" + cores[i].substring(1) + "\", this)'></div>";
  }
  html += "</div>";

  
  html += "<div class='picker-row'>";
  html += "<input type='color' id='colorPicker' value='#ffffff' oninput='vib(); setColPicker(this.value)'>";
  html += "<span class='picker-label'>🎨 Cor Personalizada</span>";
  html += "</div>";

  
  html += "<div class='slider-container'>";
  html += "<input type='range' id='br' min='0' max='255' value='128' oninput='setB()'>";
  html += "<span style='font-size:10px; color:#2cb67d; font-weight:bold; letter-spacing:2px; margin-top:10px; display:block;'>INTENSIDADE</span>";
  html += "</div>";

  
  html += "<div class='system-grid'>";
  html += "<button class='s-btn' id='m1' onclick='vib(); setM(1)'>🌈 ARCO-ÍRIS</button>";
  html += "<button class='s-btn' id='m2' onclick='vib(); setM(2)'>🌌 NEBULOSA</button>";
  html += "<button class='s-btn' id='m3' onclick='vib(); setM(3)'>🪐 SISTEMA</button>";
  html += "<button class='s-btn btn-stop' id='m0' onclick='vib(); setM(0)'>⏹️ PARAR ANIMAÇÃO</button>";
  html += "</div></div>";

  html += "<script>";
  html += "function vib(){ if(navigator.vibrate) navigator.vibrate(50); }";

 
  html += "function clearColorAtivo(){";
  html += "  document.querySelectorAll('.c-btn').forEach(e => e.classList.remove('ativo'));";
  html += "  document.getElementById('colorPicker').classList.remove('ativo');";
  html += "}";


  html += "function clearModoAtivo(){";
  html += "  ['m0','m1','m2','m3'].forEach(id => document.getElementById(id).classList.remove('ativo'));";
  html += "}";

  
  html += "let curR='ffffff';";
  html += "function setCol(c, el){";
  html += "  curR=c;";
  html += "  clearColorAtivo(); el.classList.add('ativo');";
  html += "  clearModoAtivo(); document.getElementById('m0').classList.add('ativo');";
  html += "  fetch('/set?m=0&rgb='+c+'&b='+document.getElementById('br').value);";
  html += "}";

 
  html += "function setColPicker(hex){";
  html += "  let c = hex.replace('#','');";
  html += "  curR=c;";
  html += "  clearColorAtivo(); document.getElementById('colorPicker').classList.add('ativo');";
  html += "  clearModoAtivo(); document.getElementById('m0').classList.add('ativo');";
  html += "  fetch('/set?m=0&rgb='+c+'&b='+document.getElementById('br').value);";
  html += "}";

  
  html += "function setB(){ fetch('/set?b='+document.getElementById('br').value+'&rgb='+curR); }";

  
  html += "function setM(m){";
  html += "  clearModoAtivo(); document.getElementById('m'+m).classList.add('ativo');";
  html += "  if(m===0) clearColorAtivo();"; // parar animação limpa destaque de cor também
  html += "  fetch('/set?m='+m);";
  html += "}";

  html += "</script></body></html>";

  server.send(200, "text/html", html);
}

void handleUpdate() {
  if (server.hasArg("m")) modo = server.arg("m").toInt();
  if (server.hasArg("b")) brilho = server.arg("b").toInt();
  if (server.hasArg("rgb")) {
    long n = strtol(server.arg("rgb").c_str(), NULL, 16);
    r = n >> 16; g = (n >> 8) & 0xFF; b = n & 0xFF;
  }
  if (modo == 0) {
    pixels.setBrightness(brilho);
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.show();
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  pixels.begin();
  pixels.setBrightness(128);
  pixels.show();
  WiFi.softAP(ssid, password);
  IPAddress local_IP(192, 168, 1, 4);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  server.on("/", handleRoot);
  server.on("/set", handleUpdate);
  server.begin();
}

void loop() {
  server.handleClient();
  unsigned long atual = millis();
  if (atual - ultimoMs > 30) {
    ultimoMs = atual;
    static byte j = 0;
    if (modo > 0) {
      pixels.setBrightness(brilho);
      if (modo == 1) pixels.setPixelColor(0, pixels.ColorHSV(j * 256));
      else if (modo == 2) { int f = sinf(j*0.1f)*127+128; pixels.setPixelColor(0, pixels.Color(f/2, 0, 255-f/2)); }
      else if (modo == 3) { int f = cosf(j*0.1f)*127+128; pixels.setPixelColor(0, pixels.Color(255-f/4, f/2, f/10)); }
      pixels.show();
      j++;
    }
  }
}
