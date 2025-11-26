#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// 电机控制引脚定义（原有代码，无修改）
#define LEFT_MOTOR_IN1 0
#define LEFT_MOTOR_IN2 1
#define RIGHT_MOTOR_IN1 4
#define RIGHT_MOTOR_IN2 8

// LED11-13定义（原有代码，无修改）
#define LED11 2    // 对应LEDC通道0
#define LED12 3    // 对应LEDC通道1
#define LED13 10   // 对应LEDC通道2
#define LED_PWM_CH0 0  
#define LED_PWM_CH1 1  
#define LED_PWM_CH2 2  
#define PWM_FREQ 5000  
#define PWM_RESOLUTION 8 

// WS2812配置（原有代码，无修改）
#define WS2812_PIN     6
#define WS2812_COUNT   8
#define WS2812_BRIGHTNESS  128

// 创建WS2812对象（原有代码，无修改）
Adafruit_NeoPixel ws2812Strip(WS2812_COUNT, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// WiFi配置（原有代码，无修改）
const char* ssid = "bingbing";
const char* password = "20051002";

// Web服务器（原有代码，无修改）
WebServer server(80);

// ---------------------- 函数声明（原有+新增灯效函数声明，仅新增3个灯效声明） ----------------------
void stopMotors();
void forward();
void backward();
void left();
void right();
void turnOffWS2812();
void updateWS2812Animations();
// 原有LED11-13快速流水函数（保留）
void updateLEDWaterFlow();
// 新增灯效函数声明（仅新增）
void breathLightEffect();
void blinkEffect();
void gradientSwitchEffect();
// 其他原有函数声明（保留）
void updateAutoCruise();
void handleRoot();
void handleMotorControl();
void handleLED11_13Control();
void handleWS2812Control();
void handleNotFound();

// ---------------------- 全局变量（原有+新增灯效变量，仅新增变量，不修改原有） ----------------------
// LED11-13原有变量（完全保留，无修改）
int lightSpeed = 5;          
uint8_t brightness11 = 0;   
uint8_t brightness12 = 0;   
uint8_t brightness13 = 0;   
uint8_t lightState = 0;      
unsigned long lightTimer = 0;

// 新增：灯效模式变量（不修改原有led11_13_Mode，新增用于区分灯效）
String led11_13_Effect = "快速流水"; // 新增：灯效选择：快速流水/呼吸灯/闪烁/渐变切换
// 新增：各灯效专用变量（仅新增，不影响原有）
uint8_t breathBright = 0;    // 呼吸灯亮度
int8_t breathStep = 5;       // 呼吸灯亮度增量
bool blinkState = false;     // 闪烁状态
uint8_t gradIndex = 0;       // 渐变切换索引

// 自动巡航变量（仅修改cruiseDurations数组值，解决抖动；其他无修改）
bool autoCruiseActive = false; 
String motorStatus = "停止";  // 补充原有遗漏的变量定义
uint8_t cruiseState = 0;       
unsigned long cruiseTimer = 0; 
// 【修改1】延长巡航状态时间，避免电机频繁切换导致抖动（原100/50ms→3000/1000ms）
const unsigned long cruiseDurations[] = {3000, 1000, 3000, 1000}; 

// WS2812相关变量（原有代码，无修改）
String led11_13_Mode = "关闭";  
String ws2812_Mode = "关闭";    
uint8_t currentColorIndex = 0;
unsigned long previousMillis = 0;
unsigned long animationInterval = 100;
int animationPosition = 0;

// 预设颜色（原有代码，无修改）
uint32_t colors[] = {
  ws2812Strip.Color(255, 0, 0),ws2812Strip.Color(255, 127, 0),ws2812Strip.Color(255, 255, 0),ws2812Strip.Color(127, 255, 0),
  ws2812Strip.Color(0, 255, 0),ws2812Strip.Color(0, 255, 127),ws2812Strip.Color(0, 255, 255),ws2812Strip.Color(0, 127, 255),
  ws2812Strip.Color(0, 0, 255),ws2812Strip.Color(127, 0, 255),ws2812Strip.Color(255, 0, 255),ws2812Strip.Color(255, 0, 127),
  ws2812Strip.Color(255, 100, 100),ws2812Strip.Color(100, 255, 100),ws2812Strip.Color(100, 100, 255),ws2812Strip.Color(255, 255, 100),
  ws2812Strip.Color(255, 100, 255),ws2812Strip.Color(100, 255, 255),ws2812Strip.Color(200, 100, 150),ws2812Strip.Color(100, 200, 150),
  ws2812Strip.Color(100, 150, 200),ws2812Strip.Color(200, 150, 100),ws2812Strip.Color(150, 100, 200),ws2812Strip.Color(150, 200, 100)
};
const uint8_t colorCount = sizeof(colors) / sizeof(colors[0]);

// ---------------------- 电机控制函数（仅修改left()和right()，解决转圈问题；其他无修改） ----------------------
void stopMotors() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "停止";
  autoCruiseActive = false; 
}

void forward() {
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "前进";
}

void backward() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, HIGH);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  motorStatus = "后退";
}

// 【修改2】修复左转：左电机停止，右电机正转（原逻辑为左正转、右反转，导致转圈）
void right() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW); // 左电机停止
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW); // 右电机正转（前进方向）
  motorStatus = "右转";
}

// 【修改3】修复右转：右电机停止，左电机正转（原逻辑为左反转、右正转，导致转圈）
void left() {
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW); // 左电机正转（前进方向）
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW); // 右电机停止
  motorStatus = "左转";
}

// ---------------------- LED11-13 灯效函数（原有保留+新增，不修改原有函数） ----------------------
// 原有：LED11-13快速流水灯效（完全保留，无任何修改）
void updateLEDWaterFlow() {
  if (led11_13_Mode != "灯条效果") return; 
  
  unsigned long currentMillis = millis();
  unsigned long lightInterval = map(lightSpeed, 1, 10, 50, 500);
  
  if (currentMillis - lightTimer >= lightInterval) {
    lightTimer = currentMillis;
    
    switch(lightState) {
      case 0: // LED11快速亮（直接满亮）
        brightness11 = 255;
        ledcWrite(LED_PWM_CH0, brightness11);
        lightState = 1;
        break;
      case 1: // LED12快速亮（直接满亮）
        brightness12 = 255;
        ledcWrite(LED_PWM_CH1, brightness12);
        lightState = 2;
        break;
      case 2: // LED13快速亮（直接满亮）
        brightness13 = 255;
        ledcWrite(LED_PWM_CH2, brightness13);
        lightState = 3;
        break;
      case 3: // LED11快速灭（直接熄灭）
        brightness11 = 0;
        ledcWrite(LED_PWM_CH0, brightness11);
        lightState = 4;
        break;
      case 4: // LED12快速灭（直接熄灭）
        brightness12 = 0;
        ledcWrite(LED_PWM_CH1, brightness12);
        lightState = 5;
        break;
      case 5: // LED13快速灭（直接熄灭），循环重置
        brightness13 = 0;
        ledcWrite(LED_PWM_CH2, brightness13);
        lightState = 0;
        break;
      default:
        lightState = 0;
        break;
    }
  }
}

// 新增：1. 呼吸灯效（3个LED同步渐变）
void breathLightEffect() {
  if (led11_13_Mode != "灯条效果") return; 
  
  unsigned long currentMillis = millis();
  unsigned long interval = map(lightSpeed, 1, 10, 10, 100); // 速度适配
  
  if (currentMillis - lightTimer >= interval) {
    lightTimer = currentMillis;
    // 亮度0-255循环，到达边界反转方向
    breathBright += breathStep;
    if (breathBright >= 255 || breathBright <= 0) {
      breathStep = -breathStep;
    }
    ledcWrite(LED_PWM_CH0, breathBright);
    ledcWrite(LED_PWM_CH1, breathBright);
    ledcWrite(LED_PWM_CH2, breathBright);
  }
}

// 新增：2. 闪烁灯效（3个LED同步亮灭）
void blinkEffect() {
  if (led11_13_Mode != "灯条效果") return; 
  
  unsigned long currentMillis = millis();
  unsigned long interval = map(lightSpeed, 1, 10, 100, 1000); // 速度适配
  
  if (currentMillis - lightTimer >= interval) {
    lightTimer = currentMillis;
    blinkState = !blinkState;
    uint8_t blinkBright = blinkState ? 255 : 0;
    ledcWrite(LED_PWM_CH0, blinkBright);
    ledcWrite(LED_PWM_CH1, blinkBright);
    ledcWrite(LED_PWM_CH2, blinkBright);
  }
}

// 新增：3. 渐变切换灯效（依次渐变亮灭）
void gradientSwitchEffect() {
  if (led11_13_Mode != "灯条效果") return; 
  
  unsigned long currentMillis = millis();
  unsigned long interval = map(lightSpeed, 1, 10, 30, 300); // 速度适配
  
  if (currentMillis - lightTimer >= interval) {
    lightTimer = currentMillis;
    
    switch(gradIndex) {
      case 0: brightness11 = min(brightness11 + 10, 255); ledcWrite(LED_PWM_CH0, brightness11); 
              if (brightness11 >= 255) gradIndex=1; break;
      case 1: brightness12 = min(brightness12 + 10, 255); ledcWrite(LED_PWM_CH1, brightness12); 
              if (brightness12 >= 255) gradIndex=2; break;
      case 2: brightness13 = min(brightness13 + 10, 255); ledcWrite(LED_PWM_CH2, brightness13); 
              if (brightness13 >= 255) gradIndex=3; break;
      case 3: brightness11 = max(brightness11 - 10, 0);   ledcWrite(LED_PWM_CH0, brightness11); 
              if (brightness11 <= 0) gradIndex=4; break;
      case 4: brightness12 = max(brightness12 - 10, 0);   ledcWrite(LED_PWM_CH1, brightness12); 
              if (brightness12 <= 0) gradIndex=5; break;
      case 5: brightness13 = max(brightness13 - 10, 0);   ledcWrite(LED_PWM_CH2, brightness13); 
              if (brightness13 <= 0) gradIndex=0; break;
      default: gradIndex=0; break;
    }
  }
}

// 新增：灯效总切换（根据选择的灯效调用对应函数，不修改原有逻辑）
void updateLED11_13Effects() {
  if (led11_13_Mode != "灯条效果") return;
  
  // 切换灯效前重置计时器和变量（避免干扰）
  static String lastEffect = "";
  if (lastEffect != led11_13_Effect) {
    lightTimer = millis();
    brightness11 = brightness12 = brightness13 = 0;
    ledcWrite(LED_PWM_CH0, 0);
    ledcWrite(LED_PWM_CH1, 0);
    ledcWrite(LED_PWM_CH2, 0);
    lastEffect = led11_13_Effect;
    // 重置各灯效专用变量
    if (led11_13_Effect == "呼吸灯") {breathBright=0; breathStep=5;}
    else if (led11_13_Effect == "闪烁") {blinkState=false;}
    else if (led11_13_Effect == "渐变切换") {gradIndex=0;}
    else if (led11_13_Effect == "快速流水") {lightState=0;}
  }
  
  // 根据选择的灯效调用对应函数
  if (led11_13_Effect == "快速流水") updateLEDWaterFlow();  // 原有函数
  else if (led11_13_Effect == "呼吸灯") breathLightEffect(); // 新增
  else if (led11_13_Effect == "闪烁") blinkEffect();         // 新增
  else if (led11_13_Effect == "渐变切换") gradientSwitchEffect(); // 新增
}

// ---------------------- 自动巡航函数（原有代码，无修改，仅调用修改后的left()/right()） ----------------------
void updateAutoCruise() {
  if (!autoCruiseActive) return; 
  
  unsigned long currentMillis = millis();
  if (currentMillis - cruiseTimer >= cruiseDurations[cruiseState]) {
    cruiseTimer = currentMillis;
    cruiseState = (cruiseState + 1) % 4; 
    
    switch(cruiseState) {
      case 0: forward();    break; 
      case 1: left();       break; // 调用修改后的left()
      case 2: forward();    break; 
      case 3: right();      break; // 调用修改后的right()
    }
  }
}

// ---------------------- WS2812控制函数（原有代码，完全无修改） ----------------------
void turnOffWS2812() {
  for(int i=0; i<WS2812_COUNT; i++) {
    ws2812Strip.setPixelColor(i, 0);
  }
  ws2812Strip.show();
  ws2812_Mode = "关闭";
}

void updateWS2812Animations() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= animationInterval) {
    previousMillis = currentMillis;
    
    if (ws2812_Mode == "跑马灯") {
      if (animationPosition < WS2812_COUNT) {
        ws2812Strip.setPixelColor(animationPosition, colors[currentColorIndex]);
        ws2812Strip.show();
        animationPosition++;
      } else {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
        turnOffWS2812();
        ws2812_Mode = "跑马灯";
      }
    } 
    else if (ws2812_Mode == "流水灯") {
      for(int i=0; i<WS2812_COUNT; i++) ws2812Strip.setPixelColor(i, 0);
      ws2812Strip.setPixelColor(animationPosition, colors[currentColorIndex]);
      ws2812Strip.show();
      animationPosition++;
      if (animationPosition >= WS2812_COUNT) {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
      }
    }
    else if (ws2812_Mode == "全亮跑马灯") {
      uint32_t currentColor = colors[currentColorIndex];
      uint32_t nextColor = colors[(currentColorIndex + 1) % colorCount];
      ws2812Strip.setPixelColor(animationPosition, nextColor);
      ws2812Strip.show();
      animationPosition++;
      if (animationPosition >= WS2812_COUNT) {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
      }
    }
    else if (ws2812_Mode == "全亮流水灯") {
      uint32_t baseColor = ws2812Strip.Color(50, 50, 50);
      uint32_t highlightColor = colors[currentColorIndex];
      for(int i=0; i<WS2812_COUNT; i++) ws2812Strip.setPixelColor(i, baseColor);
      ws2812Strip.setPixelColor(animationPosition, highlightColor);
      ws2812Strip.show();
      animationPosition++;
      if (animationPosition >= WS2812_COUNT) {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
      }
    }
  }
}

// ---------------------- Web请求处理（仅在原有LED11-13控制区新增灯效按钮，其他无修改） ----------------------
void handleRoot() {
  String htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP32 电机与灯光控制</title>
    <meta charset="UTF-8">
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; text-align: center; margin: 20px; background-color: #f0f0f0; }
        .container { max-width: 600px; margin: 0 auto; }
        h1 { color: #333; }
        .status { font-size: 18px; margin: 15px 0; padding: 10px; background-color: white; border-radius: 5px; }
        .control-section { background-color: white; padding: 20px; border-radius: 10px; margin-bottom: 20px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        h2 { color: #444; margin-top: 0; }
        .motor-buttons { display: flex; flex-direction: column; align-items: center; gap: 10px; margin-top: 10px; }
        .row { display: flex; gap: 10px; }
        .button { padding: 15px; font-size: 16px; border: none; border-radius: 8px; cursor: pointer; background-color: #4285f4; color: white; transition: background-color 0.3s; }
        .button:hover { background-color: #3367d6; }
        .stop-button { background-color: #ea4335; }
        .stop-button:hover { background-color: #c62828; }
        .auto-button { background-color: #fbbc04; color: #333; }
        .auto-button:hover { background-color: #f9ab00; }
        .led-buttons { display: flex; flex-wrap: wrap; justify-content: center; gap: 10px; margin-top: 15px; }
        .led-button { padding: 10px 15px; font-size: 14px; flex: 1; min-width: 100px; }
        .slider-container { margin: 20px 0; }
        .slider { width: 80%; padding: 10px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 控制系统</h1>
        
        <!-- 电机控制区（原有代码，完全无修改） -->
        <div class="control-section">
            <h2>电机控制</h2>
            <div class="status">电机状态: <span id="motorStatus">停止</span></div>
            
            <div class="motor-buttons">
                <div class="row">
                    <button class="button" onclick="controlMotor('forward')">前进</button>
                </div>
                <div class="row">
                    <button class="button" onclick="controlMotor('left')">左转</button>
                    <button class="button stop-button" onclick="controlMotor('stop')">停止</button>
                    <button class="button" onclick="controlMotor('right')">右转</button>
                </div>
                <div class="row">
                    <button class="button" onclick="controlMotor('backward')">后退</button>
                </div>
                <div class="row" style="margin-top:10px;">
                    <button class="button auto-button" onclick="controlMotor('autoStart')">开始自动巡航</button>
                    <button class="button stop-button" onclick="controlMotor('autoStop')">停止自动巡航</button>
                </div>
            </div>
        </div>
        
        <!-- LED11-13控制区（仅新增灯效按钮，其他无修改） -->
        <div class="control-section">
            <h2>LED11-13 灯条控制</h2>
            <div class="status">状态: <span id="led11_13_Status">关闭</span></div>
            
            <div class="led-buttons">
                <button class="button led-button stop-button" onclick="controlLED11_13('off')">关闭</button>
                <!-- 原有：快速流水按钮（保留） -->
                <button class="button led-button" onclick="setLED11_13Effect('快速流水')">快速流水</button>
                <!-- 新增：3个灯效按钮 -->
                <button class="button led-button" onclick="setLED11_13Effect('呼吸灯')">呼吸灯</button>
                <button class="button led-button" onclick="setLED11_13Effect('闪烁')">闪烁</button>
                <button class="button led-button" onclick="setLED11_13Effect('渐变切换')">渐变切换</button>
                <!-- 原有：灯条效果启动按钮（保留，无修改） -->
                <button class="button led-button" onclick="controlLED11_13('lightEffect')">启动灯条</button>
            </div>
            
            <!-- 原有：速度滑块（保留，无修改） -->
            <div class="slider-container" id="lightSpeedContainer" style="display:none;">
                <label for="lightSpeedSlider">速度（1=最快，10=最慢）：</label>
                <input type="range" id="lightSpeedSlider" class="slider" min="1" max="10" value="5" oninput="setLightSpeed(this.value)">
                <span id="lightSpeedValue">5</span>
            </div>
        </div>
        
        <!-- WS2812控制区（原有代码，完全无修改） -->
        <div class="control-section">
            <h2>WS2812 灯条控制（LED2-LED9）</h2>
            <div class="status">灯条模式: <span id="ws2812_Status">关闭</span></div>
            
            <div class="led-buttons">
                <button class="button led-button" onclick="controlWS2812('off')">关闭</button>
                <button class="button led-button" onclick="controlWS2812('flow')">流水灯</button>
                <button class="button led-button" onclick="controlWS2812('marquee')">跑马灯</button>
                <button class="button led-button" onclick="controlWS2812('full_flow')">全亮流水灯</button>
                <button class="button led-button" onclick="controlWS2812('full_marquee')">全亮跑马灯</button>
            </div>
        </div>
    </div>
    
    <script>
        // 电机控制（原有代码，完全无修改）
        function controlMotor(action) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("motorStatus").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/motor?action=" + action, true);
            xhttp.send();
        }
        
        // 原有：LED11-13模式控制（保留，无修改）
        function controlLED11_13(mode) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("led11_13_Status").innerHTML = this.responseText;
                    if (mode == "lightEffect") {
                        document.getElementById("lightSpeedContainer").style.display = "block";
                    } else {
                        document.getElementById("lightSpeedContainer").style.display = "none";
                    }
                }
            };
            xhttp.open("GET", "/led11_13?mode=" + mode, true);
            xhttp.send();
        }
        
        // 新增：LED11-13灯效选择（仅新增，不修改原有JS）
        function setLED11_13Effect(effect) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("led11_13_Status").innerHTML = "已选择：" + effect + "（点击启动灯条生效）";
                }
            };
            xhttp.open("GET", "/led11_13?effect=" + effect, true);
            xhttp.send();
        }
        
        // WS2812灯条控制（原有代码，完全无修改）
        function controlWS2812(mode) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ws2812_Status").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/ws2812?mode=" + mode, true);
            xhttp.send();
        }
        
        // 原有：速度控制（保留，无修改）
        function setLightSpeed(speed) {
            document.getElementById("lightSpeedValue").innerText = speed;
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/led11_13?lightSpeed=" + speed, true);
            xhttp.send();
        }
    </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", htmlPage);
}

// ---------------------- 控制处理函数（仅新增灯效参数判断，不修改原有逻辑） ----------------------
void handleMotorControl() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    
    if (action == "forward") { forward(); autoCruiseActive = false; }
    else if (action == "backward") { backward(); autoCruiseActive = false; }
    else if (action == "left") { left(); autoCruiseActive = false; }
    else if (action == "right") { right(); autoCruiseActive = false; }
    else if (action == "stop") { stopMotors(); }
    else if (action == "autoStart") {
      autoCruiseActive = true;
      cruiseState = 0;
      cruiseTimer = millis();
      forward(); 
      motorStatus = "自动巡航中（前进）";
    }
    else if (action == "autoStop") {
      autoCruiseActive = false;
      stopMotors();
      motorStatus = "自动巡航已停止";
    }
  }
  server.send(200, "text/plain", motorStatus);
}

// LED11-13控制处理（仅新增灯效参数判断，不修改原有逻辑）
void handleLED11_13Control() {
  // 原有：速度参数处理（保留，无修改）
  if (server.hasArg("lightSpeed")) {
    lightSpeed = server.arg("lightSpeed").toInt();
    lightSpeed = constrain(lightSpeed, 1, 10); 
    server.send(200, "text/plain", "速度已设置为：" + String(lightSpeed));
    return;
  }
  
  // 新增：灯效选择参数处理（仅新增，不影响原有）
  if (server.hasArg("effect")) {
    led11_13_Effect = server.arg("effect");
    server.send(200, "text/plain", "已选择灯效：" + led11_13_Effect);
    return;
  }
  
  // 原有：模式参数处理（保留，无修改）
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    
    if (mode == "off") {
      ledcWrite(LED_PWM_CH0, 0);
      ledcWrite(LED_PWM_CH1, 0);
      ledcWrite(LED_PWM_CH2, 0);
      brightness11 = brightness12 = brightness13 = 0;
      lightState = 0;
      led11_13_Mode = "关闭";
    } else if (mode == "lightEffect") {
      led11_13_Mode = "灯条效果";
      lightState = 0;
      brightness11 = brightness12 = brightness13 = 0; 
      ledcWrite(LED_PWM_CH0, 0);
      ledcWrite(LED_PWM_CH1, 0);
      ledcWrite(LED_PWM_CH2, 0);
    }
  }
  server.send(200, "text/plain", led11_13_Mode + "（当前灯效：" + led11_13_Effect + "）");
}

// WS2812控制处理（原有代码，完全无修改）
void handleWS2812Control() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    
    if (mode == "off") {
      turnOffWS2812();
    } else if (mode == "flow") {
      ws2812_Mode = "流水灯";
      animationPosition = 0;
      turnOffWS2812();
      ws2812_Mode = "流水灯";
    } else if (mode == "marquee") {
      ws2812_Mode = "跑马灯";
      animationPosition = 0;
    } else if (mode == "full_flow") {
      ws2812_Mode = "全亮流水灯";
      animationPosition = 0;
      for(int i=0; i<WS2812_COUNT; i++) ws2812Strip.setPixelColor(i, colors[currentColorIndex]);
      ws2812Strip.show();
    } else if (mode == "full_marquee") {
      ws2812_Mode = "全亮跑马灯";
      animationPosition = 0;
    }
  }
  server.send(200, "text/plain", ws2812_Mode);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// ---------------------- Setup函数（原有代码，完全无修改） ----------------------
void setup() {
  Serial.begin(115200);
  
  // 电机引脚初始化
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  stopMotors();
  
  // LED11-13的PWM初始化
  ledcSetup(LED_PWM_CH0, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(LED_PWM_CH1, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(LED_PWM_CH2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED11, LED_PWM_CH0);
  ledcAttachPin(LED12, LED_PWM_CH1);
  ledcAttachPin(LED13, LED_PWM_CH2);
  ledcWrite(LED_PWM_CH0, 0);
  ledcWrite(LED_PWM_CH1, 0);
  ledcWrite(LED_PWM_CH2, 0);
  
  // WS2812初始化
  ws2812Strip.begin();
  ws2812Strip.setBrightness(WS2812_BRIGHTNESS);
  ws2812Strip.clear();
  ws2812Strip.show();
  
  // 启动WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP地址: ");
  Serial.println(WiFi.softAPIP());
  
  // Web服务器路由
  server.on("/", handleRoot);
  server.on("/motor", handleMotorControl);
  server.on("/led11_13", handleLED11_13Control);
  server.on("/ws2812", handleWS2812Control);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("Web服务器已启动");
}

// ---------------------- Loop函数（仅新增灯效总调用，不修改原有逻辑） ----------------------
void loop() {
  server.handleClient(); 
  
  // 原有：LED11-13快速流水（保留，无修改）
  // 替换为新增的灯效总控制（自动根据选择的灯效调用对应函数）
  if (led11_13_Mode == "灯条效果") {
    updateLED11_13Effects(); // 新增：灯效总调用
  }
  
  // WS2812动画更新（原有代码，无修改）
  if (ws2812_Mode == "流水灯" || ws2812_Mode == "跑马灯" || 
      ws2812_Mode == "全亮流水灯" || ws2812_Mode == "全亮跑马灯") {
    updateWS2812Animations();
  }
  
  // 自动巡航更新（原有代码，无修改）
  if (autoCruiseActive) {
    updateAutoCruise();
  }
}