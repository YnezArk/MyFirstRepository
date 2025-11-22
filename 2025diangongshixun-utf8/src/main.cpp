/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yuting"
  2. Open http://192.168.4.1 in browser to control motors and LEDs

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
  Modified: Added GPIO LED control with sequential flow and new pulse_flow mode + speed slider.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// 电机控制引脚定义
#define LEFT_MOTOR_IN1 0
#define LEFT_MOTOR_IN2 1
#define RIGHT_MOTOR_IN1 4
#define RIGHT_MOTOR_IN2 8

// 普通 LED 引脚（支持 PWM）
#define LED11 2
#define LED12 3
#define LED13 10
const uint8_t targetLEDs[] = {LED11, LED12, LED13};

// WS2812 配置
#define LED_PIN     6
#define LED_COUNT   8
#define BRIGHTNESS  128
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi 配置
const char* ssid = "yinxuezhou";
const char* password = "20051002";

// Web 服务器
WebServer server(80);

// 系统状态
String motorStatus = "停止";
String ledMode = "关闭";          // WS2812 模式
String gpioLedMode = "seq_flow";  // 普通 LED 模式: "off", "on", "blink", "seq_flow", "pulse_flow"

// 新增：普通 LED 流水速度（ms）
unsigned long gpioLedSpeed = 100; // 默认 100ms

// WS2812 动画变量（保持不变）
uint8_t currentColorIndex = 0;
unsigned long previousMillis = 0;
unsigned long animationInterval = 100;
int animationPosition = 0;

// 预设颜色（保持不变）
uint32_t colors[] = {
 // === 更多花里胡哨的颜色（共阳极 WS2812B / NeoPixel 兼容）===
strip.Color(255, 20, 147),   // 深粉红 (Deep Pink)
strip.Color(255, 20, 20),    // 鲜红 (Crimson Red)
strip.Color(255, 165, 0),    // 橙红 (Orange-Red)
strip.Color(255, 99, 71),    // 番茄红 (Tomato)
strip.Color(220, 20, 60),    // 蔷薇红 (Ruby)
strip.Color(255, 105, 180),  // 热粉 (Hot Pink)
strip.Color(218, 112, 214),  // 兰花紫 (Orchid)
strip.Color(238, 130, 238),  // 紫罗兰 (Violet)
strip.Color(186, 85, 211),   // 中紫罗兰 (Medium Orchid)
strip.Color(148, 0, 211),    // 深紫罗兰 (Dark Violet)
strip.Color(75, 0, 130),     // 靛青 (Indigo)
strip.Color(0, 0, 139),      // 深蓝 (Dark Blue)
strip.Color(0, 191, 255),    // 深天蓝 (Deep Sky Blue)
strip.Color(30, 144, 255),   // 道奇蓝 (Dodger Blue)
strip.Color(135, 206, 250),  // 亮天蓝 (Light Sky Blue)
strip.Color(173, 216, 230),  // 淡蓝 (Light Blue)
strip.Color(176, 224, 230),  // 粉蓝 (Powder Blue)
strip.Color(0, 255, 127),    // 春绿 (Spring Green)
strip.Color(64, 224, 208),   // 海蓝宝石 (Turquoise)
strip.Color(72, 209, 204),   // 中海蓝 (Medium Turquoise)
strip.Color(0, 128, 128),    // 鸭绿 (Teal)
strip.Color(46, 139, 87),    // 海绿色 (Sea Green)
strip.Color(34, 139, 34),    // 森林绿 (Forest Green)
strip.Color(154, 205, 50),   // 黄绿色 (Yellow Green)
strip.Color(189, 183, 107),  // 卡其绿 (Dark Khaki)
strip.Color(240, 230, 140),  // 卡其黄 (Khaki)
strip.Color(255, 215, 0),    // 金色 (Gold)
strip.Color(255, 223, 0),    // 鲜金 (Bright Gold)
strip.Color(255, 140, 0),    // 深橙 (Dark Orange)
strip.Color(255, 69, 0),     // 红橙 (Red-Orange / Firebrick)
strip.Color(255, 192, 203),  // 粉红 (Pink)
strip.Color(255, 182, 193),  // 浅粉红 (Light Pink)
strip.Color(250, 128, 114),  // 三文鱼红 (Salmon)
strip.Color(255, 160, 122),  // 浅鲑鱼色 (Light Salmon)
strip.Color(255, 228, 181),  // 玉米色 (Moccasin)
strip.Color(255, 248, 220),  // 玉米丝白 (Cornsilk)
strip.Color(255, 250, 205),  // 柠檬绸 (Lemon Chiffon)
strip.Color(255, 255, 224),  // 象牙白 (Ivory) — 微黄白，很柔和
strip.Color(245, 222, 179),  // 小麦色 (Wheat)
strip.Color(244, 164, 96),   // 沙棕色 (Sandy Brown)
strip.Color(210, 180, 140),  // 肉桂色 (Tan)
strip.Color(160, 82, 45),    // 巧克力棕 (Chocolate)
strip.Color(139, 69, 19),    // 深棕 (SaddleBrown)
};
const uint8_t colorCount = sizeof(colors) / sizeof(colors[0]);

// ====== 普通 LED 流水灯状态变量 ======
enum LedFlowState { FLOW_FILL, FLOW_EMPTY };
LedFlowState flowState = FLOW_FILL;
int8_t flowIndex = 0;
unsigned long prevFlowTime = 0;

// ====== Pulse Flow 变量 ======
bool pulseDirection = true; // true = fade in, false = fade out
uint8_t pulseBrightness = 0;
unsigned long prevPulseTime = 0;
const unsigned long PULSE_STEP_TIME = 10; // ms per brightness step
int8_t pulseLedIndex = -1; // 当前正在脉动的 LED 索引 (-1 表示无)

// HTML 页面（已更新：加入滑块）
String htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP32 电机与灯光控制</title>
    <meta charset="UTF-8">
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { 
            font-family: Arial; 
            text-align: center; 
            margin: 20px;
            background-color: #f0f0f0;
        }
        .container {
            max-width: 600px;
            margin: 0 auto;
        }
        h1 {
            color: #333;
        }
        .status {
            font-size: 18px;
            margin: 15px 0;
            padding: 10px;
            background-color: white;
            border-radius: 5px;
        }
        .control-section {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            margin-bottom: 20px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        h2 {
            color: #444;
            margin-top: 0;
        }
        .motor-buttons {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 10px;
            margin-top: 10px;
        }
        .row {
            display: flex;
            gap: 10px;
        }
        .button {
            padding: 15px;
            font-size: 16px;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            background-color: #4285f4;
            color: white;
            transition: background-color 0.3s;
        }
        .button:hover {
            background-color: #3367d6;
        }
        .stop-button {
            background-color: #ea4335;
        }
        .stop-button:hover {
            background-color: #c62828;
        }
        .led-buttons {
            display: flex;
            flex-wrap: wrap;
            justify-content: center;
            gap: 10px;
            margin-top: 15px;
        }
        .led-button {
            padding: 10px 15px;
            font-size: 14px;
            flex: 1;
            min-width: 120px;
        }
        .slider-container {
            margin: 15px 0;
        }
        input[type="range"] {
            width: 80%;
            margin: 10px 0;
        }
        label {
            display: block;
            margin-top: 10px;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 控制系统</h1>
        
        <div class="control-section">
            <h2>电机控制</h2>
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
            </div>
        </div>
        
        <div class="control-section">
            <h2>WS2812 灯带控制</h2>
            <div class="led-buttons">
                <button class="button led-button" onclick="controlLED('off')">关闭</button>
                <button class="button led-button" onclick="controlLED('flow')">流水灯</button>
                <button class="button led-button" onclick="controlLED('marquee')">跑马灯</button>
                <button class="button led-button" onclick="controlLED('full_flow')">全亮流水灯</button>
                <button class="button led-button" onclick="controlLED('full_marquee')">全亮跑马灯</button>
            </div>
        </div>

        <div class="control-section">
            <h2>普通 LED 控制 (LED11/12/13)</h2>
            <div class="led-buttons">
                <button class="button led-button" onclick="controlGPIOLED('off')">关闭</button>
                <button class="button led-button" onclick="controlGPIOLED('on')">全亮</button>
                <button class="button led-button" onclick="controlGPIOLED('blink')">轮流闪</button>
                <button class="button led-button" onclick="controlGPIOLED('seq_flow')">推进流水</button>
                <button class="button led-button" onclick="controlGPIOLED('pulse_flow')">脉冲流水</button>
            </div>

            <div class="slider-container">
                <label for="speedSlider">流水/脉冲速度 (50~500 ms): <span id="speedValue">100</span> ms</label>
                <input type="range" id="speedSlider" min="50" max="500" value="100" oninput="setSpeed(this.value)">
            </div>
        </div>
    </div>
    
    <script>
        function controlMotor(action) {
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/motor?action=" + action, true);
            xhttp.send();
        }
        
        function controlLED(mode) {
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/led?mode=" + mode, true);
            xhttp.send();
        }

        function controlGPIOLED(mode) {
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/gpioled?mode=" + mode, true);
            xhttp.send();
        }

        function setSpeed(value) {
            document.getElementById('speedValue').innerText = value;
            var xhttp = new XMLHttpRequest();
            xhttp.open("GET", "/speed?value=" + value, true);
            xhttp.send();
        }
    </script>
</body>
</html>
)rawliteral";

// === 电机控制函数（不变）===
void stopMotors() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "停止";
}
void forward() {
  digitalWrite(LEFT_MOTOR_IN1, HIGH); digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, HIGH); digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "前进";
}
void backward() {
  digitalWrite(LEFT_MOTOR_IN1, LOW); digitalWrite(LEFT_MOTOR_IN2, HIGH);
  digitalWrite(RIGHT_MOTOR_IN1, LOW); digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  motorStatus = "后退";
}
void left() {
  digitalWrite(LEFT_MOTOR_IN1, HIGH); digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW); digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  motorStatus = "左转";
}
void right() {
  digitalWrite(LEFT_MOTOR_IN1, LOW); digitalWrite(LEFT_MOTOR_IN2, HIGH);
  digitalWrite(RIGHT_MOTOR_IN1, HIGH); digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "右转";
}

// === WS2812 控制（不变）===
void turnOffLEDs() {
  for(int i=0; i<LED_COUNT; i++) strip.setPixelColor(i, 0);
  strip.show();
  ledMode = "关闭";
}
void updateAnimations() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= animationInterval) {
    previousMillis = currentMillis;
    if (ledMode == "跑马灯") {
      if (animationPosition < LED_COUNT) {
        strip.setPixelColor(animationPosition, colors[currentColorIndex]);
        strip.show();
        animationPosition++;
      } else {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
        turnOffLEDs();
        ledMode = "跑马灯";
      }
    } else if (ledMode == "流水灯") {
      for(int i=0; i<LED_COUNT; i++) strip.setPixelColor(i, 0);
      strip.setPixelColor(animationPosition, colors[currentColorIndex]);
      strip.show();
      animationPosition = (animationPosition + 1) % LED_COUNT;
      if (animationPosition == 0) currentColorIndex = (currentColorIndex + 1) % colorCount;
    } else if (ledMode == "全亮跑马灯") {
      uint32_t currentColor = colors[currentColorIndex];
      uint32_t nextColor = colors[(currentColorIndex + 1) % colorCount];
      strip.setPixelColor(animationPosition, nextColor);
      strip.show();
      animationPosition = (animationPosition + 1) % LED_COUNT;
      if (animationPosition == 0) currentColorIndex = (currentColorIndex + 1) % colorCount;
    } else if (ledMode == "全亮流水灯") {
      uint32_t baseColor = strip.Color(50, 50, 50);
      uint32_t highlightColor = colors[currentColorIndex];
      for(int i=0; i<LED_COUNT; i++) strip.setPixelColor(i, baseColor);
      strip.setPixelColor(animationPosition, highlightColor);
      strip.show();
      animationPosition = (animationPosition + 1) % LED_COUNT;
      if (animationPosition == 0) currentColorIndex = (currentColorIndex + 1) % colorCount;
    }
  }
}

// === 普通 LED 控制（共阳极版本）===
void updateGPIOLEDs() {
  auto writeLed = [](uint8_t channel, uint8_t brightness) {
    // brightness: 0 = 灭, 255 = 最亮 (逻辑值)
    // 共阳极：实际 PWM = 255 - brightness
    ledcWrite(channel, 255 - brightness);
  };

  if (gpioLedMode == "off") {
    writeLed(0, 0); writeLed(1, 0); writeLed(2, 0); // 全灭（逻辑亮度=0）
  } else if (gpioLedMode == "on") {
    writeLed(0, 255); writeLed(1, 255); writeLed(2, 255); // 全亮（逻辑亮度=255）
  } else if (gpioLedMode == "blink") {
    static unsigned long lastBlink = 0;
    static uint8_t idx = 0;
    const unsigned long BLINK_INTV = 200;
    if (millis() - lastBlink > BLINK_INTV) {
      lastBlink = millis();
      writeLed(0, 0); writeLed(1, 0); writeLed(2, 0);
      writeLed(idx, 255);
      idx = (idx + 1) % 3;
    }
  } else if (gpioLedMode == "seq_flow") {
    unsigned long now = millis();
    if (now - prevFlowTime >= gpioLedSpeed) {
      prevFlowTime = now;
      // 全灭
      writeLed(0, 0); writeLed(1, 0); writeLed(2, 0);
      if (flowState == FLOW_FILL) {
        for (int i = 0; i <= flowIndex; i++) {
          writeLed(i, 255);
        }
        flowIndex++;
        if (flowIndex > 2) {
          flowIndex = 0;
          flowState = FLOW_EMPTY;
        }
      } else {
        for (int i = flowIndex + 1; i <= 2; i++) {
          writeLed(i, 255);
        }
        flowIndex++;
        if (flowIndex > 2) {
          flowIndex = 0;
          flowState = FLOW_FILL;
        }
      }
    }
  } else if (gpioLedMode == "pulse_flow") {
    unsigned long now = millis();

    // 切换新 LED？
    if (pulseLedIndex == -1) {
      // 全灭
      writeLed(0, 0); writeLed(1, 0); writeLed(2, 0);
      // 选择下一个 LED（按 seq_flow 顺序）
      if (flowState == FLOW_FILL) {
        pulseLedIndex = flowIndex;
        flowIndex++;
        if (flowIndex > 2) {
          flowIndex = 0;
          flowState = FLOW_EMPTY;
        }
      } else {
        pulseLedIndex = flowIndex;
        flowIndex++;
        if (flowIndex > 2) {
          flowIndex = 0;
          flowState = FLOW_FILL;
        }
      }
      pulseBrightness = 0;
      pulseDirection = true; // 开始淡入（逻辑亮度从 0 → 255）
      prevPulseTime = now;
      return;
    }

    // 脉冲动画
    if (now - prevPulseTime >= PULSE_STEP_TIME) {
      prevPulseTime = now;

      if (pulseDirection) {
        pulseBrightness += 5;
        if (pulseBrightness >= 255) {
          pulseBrightness = 255;
          pulseDirection = false; // 开始淡出
        }
      } else {
        pulseBrightness -= 5;
        if (pulseBrightness <= 0) {
          pulseBrightness = 0;
          pulseLedIndex = -1; // 结束，下次切换 LED
          return;
        }
      }

      // 写入当前亮度（仅当前 LED）
      writeLed(0, 0); writeLed(1, 0); writeLed(2, 0);
      if (pulseLedIndex >= 0 && pulseLedIndex < 3) {
        writeLed(pulseLedIndex, pulseBrightness);
      }
    }
  }
}

// === Web 处理函数 ===
void handleRoot() {
  server.send(200, "text/html", htmlPage);
}
void handleMotorControl() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "forward") forward();
    else if (action == "backward") backward();
    else if (action == "left") left();
    else if (action == "right") right();
    else if (action == "stop") stopMotors();
  }
  server.send(200, "text/plain", motorStatus);
}
void handleLEDControl() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    if (mode == "off") turnOffLEDs();
    else if (mode == "flow") {
      ledMode = "流水灯"; animationPosition = 0; turnOffLEDs(); ledMode = "流水灯";
    } else if (mode == "marquee") {
      ledMode = "跑马灯"; animationPosition = 0;
    } else if (mode == "full_flow") {
      ledMode = "全亮流水灯"; animationPosition = 0;
      for(int i=0; i<LED_COUNT; i++) strip.setPixelColor(i, colors[currentColorIndex]);
      strip.show();
    } else if (mode == "full_marquee") {
      ledcWrite(0, 0); ledcWrite(1, 0); ledcWrite(2, 0);
      ledMode = "全亮跑马灯"; animationPosition = 0;
    }
  }
  server.send(200, "text/plain", ledMode);
}

void handleGPIOLEDControl() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    if (mode == "off" || mode == "on" || mode == "blink" || mode == "seq_flow" || mode == "pulse_flow") {
      gpioLedMode = mode;
      // 重置状态
      if (mode == "seq_flow" || mode == "pulse_flow") {
        flowState = FLOW_FILL;
        flowIndex = 0;
        pulseLedIndex = -1;
      }
      // 静态模式立即应用
      if (mode == "off") {
        ledcWrite(0, 0); ledcWrite(1, 0); ledcWrite(2, 0);
      } else if (mode == "on") {
        ledcWrite(0, 255); ledcWrite(1, 255); ledcWrite(2, 255);
      }
    }
  }
  server.send(200, "text/plain", gpioLedMode);
}

// 新增：速度控制
void handleSpeedControl() {
  if (server.hasArg("value")) {
    gpioLedSpeed = server.arg("value").toInt();
    if (gpioLedSpeed < 50) gpioLedSpeed = 50;
    if (gpioLedSpeed > 500) gpioLedSpeed = 500;
  }
  server.send(200, "text/plain", String(gpioLedSpeed));
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  
  // 电机引脚
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  stopMotors();
  
  // WS2812
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();
  
  // 普通 LED：配置为 LEDC PWM 输出（共阴接法）
  // ESP32: LEDC 通道 0~15，频率 5000Hz，分辨率 8bit
  ledcSetup(0, 5000, 8); ledcAttachPin(LED11, 0);
  ledcSetup(1, 5000, 8); ledcAttachPin(LED12, 1);
  ledcSetup(2, 5000, 8); ledcAttachPin(LED13, 2);
  // 初始全灭
  ledcWrite(0, 0); ledcWrite(1, 0); ledcWrite(2, 0);

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Web 路由
  server.on("/", handleRoot);
  server.on("/motor", handleMotorControl);
  server.on("/led", handleLEDControl);
  server.on("/gpioled", handleGPIOLEDControl);
  server.on("/speed", handleSpeedControl); // 新增
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("Web server started");
}

// === Loop ===
void loop() {
  server.handleClient();
  
  if (ledMode == "流水灯" || ledMode == "跑马灯" || 
      ledMode == "全亮流水灯" || ledMode == "全亮跑马灯") {
    updateAnimations();
  }

  updateGPIOLEDs();
}