/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

// 电机控制引脚定义
// 左电机
#define LEFT_MOTOR_IN1 0
#define LEFT_MOTOR_IN2 1
// 右电机
#define RIGHT_MOTOR_IN1 4
#define RIGHT_MOTOR_IN2 8

// LED
#define LED11 2
#define LED12 3
#define LED13 10

// WS2812配置 (使用Adafruit NeoPixel库)
#define LED_PIN     6       // WS2812数据引脚
#define LED_COUNT   8       // 灯珠数量
#define BRIGHTNESS  128     // 亮度(0-255)

// 创建WS2812对象
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// WiFi配置
const char* ssid = "yin-xuezhou";// 定义WiFi热点名称
const char* password = "11223344";// 定义WiFi热点密码

// Web服务器
WebServer server(80);

// 系统状态
String motorStatus = "停止";
String ledMode = "关闭";
uint8_t currentColorIndex = 0; // 当前颜色索引
unsigned long previousMillis = 0;
unsigned long animationInterval = 100; // 动画间隔(ms)
int animationPosition = 0; // 动画位置

// 预设颜色 (RGB值)
uint32_t colors[] = {
  strip.Color(255, 0, 0),      // 纯红
  strip.Color(255, 127, 0),    // 橙红
  strip.Color(255, 255, 0),    // 纯黄
  strip.Color(127, 255, 0),    // 黄绿
  strip.Color(0, 255, 0),      // 纯绿
  strip.Color(0, 255, 127),    // 青绿
  strip.Color(0, 255, 255),    // 纯青
  strip.Color(0, 127, 255),    // 浅蓝
  strip.Color(0, 0, 255),      // 纯蓝
  strip.Color(127, 0, 255),    // 靛蓝
  strip.Color(255, 0, 255),    // 纯紫
  strip.Color(255, 0, 127),    // 粉红
  
  // 高饱和过渡色
  strip.Color(255, 100, 100),  // 亮粉
  strip.Color(100, 255, 100),  // 亮绿
  strip.Color(100, 100, 255),  // 亮蓝
  strip.Color(255, 255, 100),  // 亮黄
  strip.Color(255, 100, 255),  // 亮紫
  strip.Color(100, 255, 255),  // 亮青
  
  // 柔和梦幻色
  strip.Color(200, 100, 150),  // 玫瑰色
  strip.Color(100, 200, 150),  // 薄荷色
  strip.Color(100, 150, 200),  // 天蓝
  strip.Color(200, 150, 100),  // 暖橙
  strip.Color(150, 100, 200),  // 薰衣草紫
  strip.Color(150, 200, 100)   // 嫩黄绿
};
const uint8_t colorCount = sizeof(colors) / sizeof(colors[0]);

// HTML网页内容
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
            <h2>灯光控制</h2>
            
            <div class="led-buttons">
                <button class="button led-button" onclick="controlLED('off')">关闭</button>
                <button class="button led-button" onclick="controlLED('flow')">流水灯</button>
                <button class="button led-button" onclick="controlLED('marquee')">跑马灯</button>
                <button class="button led-button" onclick="controlLED('full_flow')">全亮流水灯</button>
                <button class="button led-button" onclick="controlLED('full_marquee')">全亮跑马灯</button>
            </div>
        </div>
    </div>
    
    <script>
        // 控制电机
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
        
        // 控制灯光
        function controlLED(mode) {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ledStatus").innerHTML = this.responseText;
                }
            };
            xhttp.open("GET", "/led?mode=" + mode, true);
            xhttp.send();
        }
    </script>
</body>
</html>
)rawliteral";

// 电机控制函数
void stopMotors() {
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "停止";
}

void forward() {
  // 左电机正转
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  // 右电机正转
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "前进";
}

void backward() {
  // 左电机反转
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, HIGH);
  // 右电机反转
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  motorStatus = "后退";
}

void left() {
  // 左电机停止
  digitalWrite(LEFT_MOTOR_IN1, HIGH);
  digitalWrite(LEFT_MOTOR_IN2, LOW);
  // 右电机正转
  digitalWrite(RIGHT_MOTOR_IN1, LOW);
  digitalWrite(RIGHT_MOTOR_IN2, HIGH);
  motorStatus = "左转";
}

void right() {
  // 左电机正转
  digitalWrite(LEFT_MOTOR_IN1, LOW);
  digitalWrite(LEFT_MOTOR_IN2, HIGH);
  // 右电机停止
  digitalWrite(RIGHT_MOTOR_IN1, HIGH);
  digitalWrite(RIGHT_MOTOR_IN2, LOW);
  motorStatus = "右转";
}

// WS2812控制函数 (使用Adafruit库函数)
void turnOffLEDs() {
  for(int i=0; i<LED_COUNT; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
  ledMode = "关闭";
}

// 更新动画
void updateAnimations() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= animationInterval) {
    previousMillis = currentMillis;
    
    if (ledMode == "跑马灯") {
      // 流水灯效果 - 依次点亮并保持
      if (animationPosition < LED_COUNT) {
        strip.setPixelColor(animationPosition, colors[currentColorIndex]);
        strip.show();
        animationPosition++;
      } else {
        // 一轮完成后切换颜色并重置
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
        turnOffLEDs(); // 清除当前显示，准备下一轮
        ledMode = "跑马灯"; // 重新设置模式，避免被关闭
      }
    } 
    else if (ledMode == "流水灯") {
      // 跑马灯效果 - 单个灯珠移动
      // 先清除所有灯
      for(int i=0; i<LED_COUNT; i++) {
        strip.setPixelColor(i, 0);
      }
      // 点亮当前位置的灯
      strip.setPixelColor(animationPosition, colors[currentColorIndex]);
      strip.show();
      
      // 移动到下一个位置
      animationPosition++;
      if (animationPosition >= LED_COUNT) {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
      }
    }
    else if (ledMode == "全亮跑马灯") {
      // 全亮流水灯 - 所有灯亮，依次改变颜色
      uint32_t currentColor = colors[currentColorIndex];
      uint32_t nextColor = colors[(currentColorIndex + 1) % colorCount];
      
      // 逐个灯珠切换到下一种颜色
      strip.setPixelColor(animationPosition, nextColor);
      strip.show();
      
      animationPosition++;
      if (animationPosition >= LED_COUNT) {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
      }
    }
    else if (ledMode == "全亮流水灯") {
      // 全亮跑马灯 - 所有灯亮，有一个高亮灯珠移动
      uint32_t baseColor = strip.Color(50, 50, 50); // 基础暗色调
      uint32_t highlightColor = colors[currentColorIndex]; // 高亮颜色
      
      // 所有灯设置为基础色
      for(int i=0; i<LED_COUNT; i++) {
        strip.setPixelColor(i, baseColor);
      }
      
      // 当前位置设置为高亮色
      strip.setPixelColor(animationPosition, highlightColor);
      strip.show();
      
      // 移动到下一个位置
      animationPosition++;
      if (animationPosition >= LED_COUNT) {
        animationPosition = 0;
        currentColorIndex = (currentColorIndex + 1) % colorCount;
      }
    }
  }
}

// Web请求处理函数
void handleRoot() {
  server.sendHeader("Content-Type", "text/html; charset=UTF-8");
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
  server.sendHeader("Content-Type", "text/plain; charset=UTF-8");
  server.send(200, "text/plain", motorStatus);
}

void handleLEDControl() {
  if (server.hasArg("mode")) {
    String mode = server.arg("mode");
    
    if (mode == "off") {
      turnOffLEDs();
    } 
    else if (mode == "flow") {
      ledMode = "流水灯";
      animationPosition = 0; // 重置动画位置
      turnOffLEDs(); // 清除当前显示
      ledMode = "流水灯"; // 重新设置模式
    } 
    else if (mode == "marquee") {
      ledMode = "跑马灯";
      animationPosition = 0; // 重置动画位置
    }
    else if (mode == "full_flow") {
      ledMode = "全亮流水灯";
      animationPosition = 0;
      // 初始化所有灯为当前颜色
      for(int i=0; i<LED_COUNT; i++) {
        strip.setPixelColor(i, colors[currentColorIndex]);
      }
      strip.show();
    }
    else if (mode == "full_marquee") {
      ledMode = "全亮跑马灯";
      animationPosition = 0;
    }
  }
  server.sendHeader("Content-Type", "text/plain; charset=UTF-8");
  server.send(200, "text/plain", ledMode);
}

void handleNotFound() {
  server.sendHeader("Content-Type", "text/plain; charset=UTF-8");
  server.send(404, "text/plain", "Not found");
}

void setup() {
  // 初始化串口
  Serial.begin(115200);
  
  // 初始化电机引脚
  pinMode(LEFT_MOTOR_IN1, OUTPUT);
  pinMode(LEFT_MOTOR_IN2, OUTPUT);
  pinMode(RIGHT_MOTOR_IN1, OUTPUT);
  pinMode(RIGHT_MOTOR_IN2, OUTPUT);
  stopMotors();
  
  // 初始化WS2812 (Adafruit库初始化)
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();
  
  //初始化LED灯
  pinMode(LED11, OUTPUT);
  pinMode(LED12, OUTPUT);
  pinMode(LED13, OUTPUT);
  digitalWrite(LED11, HIGH);
  digitalWrite(LED12, HIGH);
  digitalWrite(LED13, HIGH);
  // 启动WiFi AP
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // 配置Web服务器路由
  server.on("/", handleRoot);
  server.on("/motor", handleMotorControl);
  server.on("/led", handleLEDControl);
  server.onNotFound(handleNotFound);
  
  // 启动Web服务器
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // 处理Web请求
  server.handleClient();
  
  // 更新灯光动画
  if (ledMode == "流水灯" || ledMode == "跑马灯" || 
      ledMode == "全亮流水灯" || ledMode == "全亮跑马灯") {
    updateAnimations();
  }
}