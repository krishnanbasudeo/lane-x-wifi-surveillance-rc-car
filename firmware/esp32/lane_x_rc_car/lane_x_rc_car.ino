#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

// Replace with your board's camera model
#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

// AP credentials
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// Motor control pins
int enA = D0;
int in1 = D1;
int in2 = D2;
int in3 = D3;
int in4 = D4;
int enB = D5;

const int buzPin = D6;
const int ledPin1 = D7;
const int ledPin2 = D8;
const int wifiLedPin = D9;

String command;
int SPEED = 1023;
int speed_Coeff = 3;

// Light states
bool frontLightState = false;
bool backLightState = false;

WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
  Serial.println("\n*WiFi Car with Camera - ESP32-S3 Version*");

  // Pin setup for car control
  pinMode(buzPin, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);

  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  digitalWrite(wifiLedPin, HIGH);

  analogWrite(enA, 0);
  analogWrite(enB, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // Camera configuration
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }

  // Start in Access Point mode
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  digitalWrite(wifiLedPin, LOW);

  // Setup web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/control", HTTP_GET, handleCarControl);
  server.on("/video", HTTP_GET, handleVideo);
  
  server.begin();
  
  Serial.println("Web Server Ready! Connect to AP and visit http://" + IP.toString());
  Serial.println("You can control the car and see camera footage from the web page");

  // OTA
  ArduinoOTA.begin();
  Serial.println("OTA Update Ready");
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  delay(2);
}

void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>RC Car Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: Arial, sans-serif;
        text-align: center;
        background: #f0f0f0;
        margin: 0;
        padding: 20px;
      }
      .container {
        max-width: 800px;
        margin: 0 auto;
      }
      .video-container {
        background: #000;
        border-radius: 10px;
        padding: 10px;
        margin-bottom: 20px;
      }
      .video-container img {
        width: 100%;
        max-width: 640px;
        border-radius: 5px;
      }
      .controls {
        background: white;
        padding: 20px;
        border-radius: 10px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1);
      }
      .control-row {
        display: flex;
        justify-content: center;
        margin: 10px 0;
      }
      button {
        padding: 15px 25px;
        margin: 5px;
        font-size: 16px;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        background: #4CAF50;
        color: white;
        transition: background 0.3s;
        user-select: none;
        -webkit-user-select: none;
        touch-action: manipulation;
      }
      button:hover {
        background: #45a049;
      }
      button:active {
        background: #3d8b40;
        transform: scale(0.98);
      }
      .stop-btn {
        background: #f44336;
      }
      .stop-btn:hover {
        background: #da190b;
      }
      .stop-btn:active {
        background: #c62828;
      }
      .horn-btn {
        background: #ff9800;
      }
      .light-btn-off {
        background: #666666;
      }
      .light-btn-on {
        background: #FFD700;
        color: black;
      }
      .speed-control {
        margin: 15px 0;
      }
      .speed-btn {
        background: #9C27B0;
        padding: 10px 15px;
      }
      .status {
        margin-top: 20px;
        padding: 10px;
        background: #e7f3ff;
        border-radius: 5px;
      }
      .active {
        background: #2E7D32 !important;
        box-shadow: 0 0 10px rgba(0,0,0,0.3);
      }
    </style>
  </head>
  <body>
    <div class="container">
      <h1>RC Car Controller</h1>
      
      <div class="video-container">
        <img src="/video" id="video" alt="Live Camera Feed">
      </div>
      
      <div class="controls">
        <h3>Car Controls (Press and Hold)</h3>
        
        <!-- Direction Controls -->
        <div class="control-row">
          <button id="btnForward" ontouchstart="startCommand('F')" ontouchend="stopCommand()" onmousedown="startCommand('F')" onmouseup="stopCommand()" onmouseleave="stopCommand()">↑ Forward</button>
        </div>
        <div class="control-row">
          <button id="btnLeft" ontouchstart="startCommand('L')" ontouchend="stopCommand()" onmousedown="startCommand('L')" onmouseup="stopCommand()" onmouseleave="stopCommand()">← Left</button>
          <button class="stop-btn" onclick="sendCommand('S')">Stop</button>
          <button id="btnRight" ontouchstart="startCommand('R')" ontouchend="stopCommand()" onmousedown="startCommand('R')" onmouseup="stopCommand()" onmouseleave="stopCommand()">Right →</button>
        </div>
        <div class="control-row">
          <button id="btnBackward" ontouchstart="startCommand('B')" ontouchend="stopCommand()" onmousedown="startCommand('B')" onmouseup="stopCommand()" onmouseleave="stopCommand()">↓ Backward</button>
        </div>
        
        <!-- Additional Controls -->
        <div class="control-row">
          <button class="horn-btn" onclick="sendCommand('V')">Beep Horn</button>
          <button class="light-btn-off" id="btnFrontLight" onclick="toggleFrontLight()">Front Light OFF</button>
          <button class="light-btn-off" id="btnBackLight" onclick="toggleBackLight()">Back Light OFF</button>
        </div>
        
        <!-- Speed Control -->
        <div class="speed-control">
          <h4>Speed Control</h4>
          <button class="speed-btn" onclick="setSpeed(1)">Speed 1</button>
          <button class="speed-btn" onclick="setSpeed(3)">Speed 3</button>
          <button class="speed-btn" onclick="setSpeed(5)">Speed 5</button>
          <button class="speed-btn" onclick="setSpeed(8)">Speed 8</button>
          <button class="speed-btn" onclick="setSpeed(10)">Max Speed</button>
        </div>
        
        <div class="status" id="status">
          Ready to control... Press and hold buttons to move
        </div>
      </div>
    </div>

    <script>
      let currentCommand = '';
      let frontLightOn = false;
      let backLightOn = false;
      
      // Update video stream
      function updateVideo() {
        const video = document.getElementById('video');
        video.src = "/video?" + new Date().getTime();
      }
      
      // Update video every 100ms for smooth streaming
      setInterval(updateVideo, 100);
      
      // Start command when button is pressed
      function startCommand(cmd) {
        currentCommand = cmd;
        sendCommand(cmd);
        
        // Add active class to button
        const btn = getButtonByCommand(cmd);
        if (btn) btn.classList.add('active');
        
        document.getElementById('status').innerHTML = 'Moving: ' + getCommandName(cmd);
      }
      
      // Stop command when button is released
      function stopCommand() {
        if (currentCommand) {
          sendCommand('S');
          
          // Remove active class from button
          const btn = getButtonByCommand(currentCommand);
          if (btn) btn.classList.remove('active');
          
          document.getElementById('status').innerHTML = 'Stopped';
          currentCommand = '';
        }
      }
      
      // Send single commands (for horn, lights, etc.)
      function sendCommand(cmd) {
        fetch('/control?State=' + cmd)
          .then(response => response.text())
          .then(data => {
            console.log('Command: ' + cmd + ' - ' + data);
          })
          .catch(error => {
            console.log('Error: ' + error);
            document.getElementById('status').innerHTML = 'Error: ' + error;
          });
      }
      
      // Toggle front light
      function toggleFrontLight() {
        const btn = document.getElementById('btnFrontLight');
        if (frontLightOn) {
          sendCommand('w');
          btn.innerHTML = 'Front Light OFF';
          btn.className = 'light-btn-off';
          document.getElementById('status').innerHTML = 'Front Light OFF';
        } else {
          sendCommand('W');
          btn.innerHTML = 'Front Light ON';
          btn.className = 'light-btn-on';
          document.getElementById('status').innerHTML = 'Front Light ON';
        }
        frontLightOn = !frontLightOn;
      }
      
      // Toggle back light
      function toggleBackLight() {
        const btn = document.getElementById('btnBackLight');
        if (backLightOn) {
          sendCommand('x');
          btn.innerHTML = 'Back Light OFF';
          btn.className = 'light-btn-off';
          document.getElementById('status').innerHTML = 'Back Light OFF';
        } else {
          sendCommand('X');
          btn.innerHTML = 'Back Light ON';
          btn.className = 'light-btn-on';
          document.getElementById('status').innerHTML = 'Back Light ON';
        }
        backLightOn = !backLightOn;
      }
      
      // Helper function to get button by command
      function getButtonByCommand(cmd) {
        switch(cmd) {
          case 'F': return document.getElementById('btnForward');
          case 'B': return document.getElementById('btnBackward');
          case 'L': return document.getElementById('btnLeft');
          case 'R': return document.getElementById('btnRight');
          default: return null;
        }
      }
      
      // Helper function to get command name
      function getCommandName(cmd) {
        switch(cmd) {
          case 'F': return 'Forward';
          case 'B': return 'Backward';
          case 'L': return 'Left';
          case 'R': return 'Right';
          case 'S': return 'Stop';
          default: return cmd;
        }
      }
      
      // Set speed
      function setSpeed(level) {
        sendCommand(level.toString());
        document.getElementById('status').innerHTML = 'Speed set to: ' + level;
      }
      
      // Keyboard controls with press and hold
      document.addEventListener('keydown', function(event) {
        if (!currentCommand) {
          switch(event.key) {
            case 'ArrowUp':
            case 'w':
            case 'W':
              startCommand('F');
              break;
            case 'ArrowDown':
            case 's':
            case 'S':
              startCommand('B');
              break;
            case 'ArrowLeft':
            case 'a':
            case 'A':
              startCommand('L');
              break;
            case 'ArrowRight':
            case 'd':
            case 'D':
              startCommand('R');
              break;
            case ' ':
              sendCommand('S');
              break;
            case 'h':
            case 'H':
              sendCommand('V');
              break;
            case 'f':
            case 'F':
              if (event.ctrlKey) toggleFrontLight();
              break;
            case 'b':
            case 'B':
              if (event.ctrlKey) toggleBackLight();
              break;
          }
        }
      });
      
      // Stop on key up
      document.addEventListener('keyup', function(event) {
        switch(event.key) {
          case 'ArrowUp':
          case 'w':
          case 'W':
          case 'ArrowDown':
          case 's':
          case 'S':
          case 'ArrowLeft':
          case 'a':
          case 'A':
          case 'ArrowRight':
          case 'd':
          case 'D':
            stopCommand();
            break;
        }
      });
      
      // Prevent context menu on long press
      document.addEventListener('contextmenu', function(e) {
        e.preventDefault();
        return false;
      });
    </script>
  </body>
  </html>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

void handleVideo() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  
  server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void handleCarControl() {
  if (server.hasArg("State")) {
    command = server.arg("State");
    Serial.println("Car Command received: " + command);
    
    // Process commands
    if (command == "F") Forward();
    else if (command == "B") Backward();
    else if (command == "R") TurnRight();
    else if (command == "L") TurnLeft();
    else if (command == "G") ForwardLeft();
    else if (command == "H") BackwardLeft();
    else if (command == "I") ForwardRight();
    else if (command == "J") BackwardRight();
    else if (command == "S") Stop();
    else if (command == "V") BeepHornOn();
    else if (command == "v") BeepHornOff();
    else if (command == "W") { 
      FrontLightOn();
      frontLightState = true;
    }
    else if (command == "w") { 
      FrontLightOff();
      frontLightState = false;
    }
    else if (command == "X") { 
      BackLightOn();
      backLightState = true;
    }
    else if (command == "x") { 
      BackLightOff();
      backLightState = false;
    }
    else if (command == "0") SPEED = 0;
    else if (command == "1") SPEED = 76;
    else if (command == "2") SPEED = 153;
    else if (command == "3") SPEED = 229;
    else if (command == "4") SPEED = 306;
    else if (command == "5") SPEED = 382;
    else if (command == "6") SPEED = 459;
    else if (command == "7") SPEED = 535;
    else if (command == "8") SPEED = 612;
    else if (command == "9") SPEED = 688;
    else if (command == "10") SPEED = 765;
    
    server.send(200, "text/plain", "OK: " + command);
  } else {
    server.send(200, "text/plain", "No command received");
  }
}

// Your movement functions (keep the same as before)
void Forward() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Moving Forward");
}

void Backward() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Moving Backward");
}

void TurnRight() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Turning Right");
}

void TurnLeft() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Turning Left");
}

void ForwardLeft() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Forward Left");
}

void BackwardLeft() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Backward Left");
}

void ForwardRight() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Forward Right");
}

void BackwardRight() {
  analogWrite(enA, SPEED / speed_Coeff);
  digitalWrite(in1, LOW); digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW); digitalWrite(in4, HIGH);
  analogWrite(enB, SPEED / speed_Coeff);
  Serial.println("Backward Right");
}

void Stop() {
  analogWrite(enA, 0); analogWrite(enB, 0);
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
  Serial.println("Stopped");
}

void BeepHornOn() {
  digitalWrite(buzPin, HIGH); delay(150);
  digitalWrite(buzPin, LOW); delay(80);
  Serial.println("Horn ON");
}

void BeepHornOff() {
  digitalWrite(buzPin, LOW);
  Serial.println("Horn OFF");
}

void FrontLightOn() {
  digitalWrite(ledPin1, HIGH);
  Serial.println("Front Light ON");
}

void FrontLightOff() {
  digitalWrite(ledPin1, LOW);
  Serial.println("Front Light OFF");
}

void BackLightOn() {
  digitalWrite(ledPin2, HIGH);
  Serial.println("Back Light ON");
}

void BackLightOff() {
  digitalWrite(ledPin2, LOW);
  Serial.println("Back Light OFF");
}