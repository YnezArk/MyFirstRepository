/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "ESP32-Control"
  2. Open http://192.168.4.1 in browser to control motors and LEDs

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
  Modified: Added GPIO LED control via web with sequential flow effect.
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

// 普通 LED 引脚
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
String gpioLedMode = "seq_flow";  // 普通 LED 模式: "off", "on", "blink", "seq_flow"

// WS2812 动画变量
uint8_t currentColorIndex = 0;
unsigned long previousMillis = 0;
unsigned long animationInterval = 100;
int animationPosition = 0;

// 预设颜色
uint32_t colors[] = {
  strip.Color(255, 0, 0),      // 红
  strip.Color(255, 127, 0),    // 橙
  strip.Color(255, 255, 0),    // 黄
  strip.Color(127, 255, 0),    // 黄绿
  strip.Color(0, 255, 0),      // 绿
  strip.Color(0, 255, 127),    // 青绿
  strip.Color(0, 255, 255),    // 青
  strip.Color(0, 127, 255),    // 浅蓝
  strip.Color(0, 0, 255),      // 蓝
  strip.Color(127, 0, 255),    // 靛蓝
  strip.Color(255, 0, 255),    // 紫
  strip.Color(255, 0, 127),    // 粉红
  strip.Color(255, 100, 100),  // 亮粉
  strip.Color(100, 255, 100),  // 亮绿
  strip.Color(100, 100, 255),  // 亮蓝
  strip.Color(255, 255, 100),  // 亮黄
  strip.Color(255, 100, 255),  // 亮紫
  strip.Color(100, 255, 255),  // 亮青
  strip.Color(200, 100, 150),  // 玫瑰
  strip.Color(100, 200, 150),  // 薄荷
  strip.Color(100, 150, 200),  // 天蓝
  strip.Color(200, 150, 100),  // 暖橙
  strip.Color(150, 100, 200),  // 薰衣草紫
  strip.Color(150, 200, 100)   // 嫩黄绿
};
const uint8_t colorCount = sizeof(colors) / sizeof(colors[0]);

// ====== 新增：普通 LED 流水灯状态变量 ======
enum LedFlowState { FLOW_FILL, FLOW_EMPTY };
LedFlowState flowState = FLOW_FILL;
int8_t flowIndex = 0;
unsigned long prevFlowTime = 0;
const unsigned long FLOW_INTERVAL = 100; // ms

// HTML 页面（已更新）
String htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP32 电机与灯光控制</title>
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

// === 新增：普通 LED 统一控制函数 ===
void updateGPIOLEDs() {
  if (gpioLedMode == "off") {
    digitalWrite(LED11, LOW);
    digitalWrite(LED12, LOW);
    digitalWrite(LED13, LOW);
  } else if (gpioLedMode == "on") {
    digitalWrite(LED11, HIGH);
    digitalWrite(LED12, HIGH);
    digitalWrite(LED13, HIGH);
  } else if (gpioLedMode == "blink") {
    static unsigned long lastBlink = 0;
    static uint8_t idx = 0;
    const unsigned long BLINK_INTV = 200;
    if (millis() - lastBlink > BLINK_INTV) {
      lastBlink = millis();
      digitalWrite(LED11, LOW);
      digitalWrite(LED12, LOW);
      digitalWrite(LED13, LOW);
      digitalWrite(targetLEDs[idx], HIGH);
      idx = (idx + 1) % 3;
    }
  } else if (gpioLedMode == "seq_flow") {
    unsigned long now = millis();
    if (now - prevFlowTime >= FLOW_INTERVAL) {
      prevFlowTime = now;

      // 先全灭
      digitalWrite(LED11, LOW);
      digitalWrite(LED12, LOW);
      digitalWrite(LED13, LOW);

      if (flowState == FLOW_FILL) {
        for (int i = 0; i <= flowIndex; i++) {
          digitalWrite(targetLEDs[i], HIGH);
        }
        flowIndex++;
        if (flowIndex > 2) {
          flowIndex = 0;
          flowState = FLOW_EMPTY;
        }
      } else { // FLOW_EMPTY
        for (int i = flowIndex + 1; i <= 2; i++) {
          digitalWrite(targetLEDs[i], HIGH);
        }
        flowIndex++;
        if (flowIndex > 2) {
          flowIndex = 0;
          flowState = FLOW_FILL;
        }
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
      ledMode = "全亮跑马灯"; animationPosition = 0;
    }
  }
  server.send(200, "text/plain", ledMode);
}

// === 新增：普通 LED Web 控制 ===
void handleGPIOLEDControl() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    if (mode == "off" || mode == "on" || mode == "blink" || mode == "seq_flow") {
      gpioLedMode = mode;
      // 立即应用静态状态（避免等待 loop）
      if (mode == "off") {
        digitalWrite(LED11, LOW); digitalWrite(LED12, LOW); digitalWrite(LED13, LOW);
      } else if (mode == "on") {
        digitalWrite(LED11, HIGH); digitalWrite(LED12, HIGH); digitalWrite(LED13, HIGH);
      }
      // blink 和 seq_flow 由 loop 控制，无需立即设置
    }
  }
  server.send(200, "text/plain", gpioLedMode);
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
  
  // 普通 LED
  pinMode(LED11, OUTPUT);
  pinMode(LED12, OUTPUT);
  pinMode(LED13, OUTPUT);
  digitalWrite(LED11, LOW);
  digitalWrite(LED12, LOW);
  digitalWrite(LED13, LOW);

  // WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Web 路由
  server.on("/", handleRoot);
  server.on("/motor", handleMotorControl);
  server.on("/led", handleLEDControl);
  server.on("/gpioled", handleGPIOLEDControl); // 新增路由
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

  updateGPIOLEDs(); // 控制 LED11/12/13
}