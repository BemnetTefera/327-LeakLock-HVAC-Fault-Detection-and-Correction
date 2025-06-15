// ========== ESP32 HVAC FIRMWARE (Rewritten) ==========
#include <Arduino.h>
#include <Wire.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "wifi_manager.h"

#define DEVICE_ID "zone2"  // Change this per ESP32: zone1, zone2, zone3
#define TMP102_ADDR 0x48
#define LPS25HB_ADDR 0x5D
#define TCA_ADDR 0x70

// Fan control
#define AIN1 25
#define AIN2 26
#define PWMA 27
#define BIN1 14
#define BIN2 12
#define PWMB 13
#define STBY 33
#define PWM_CHANNEL_A 0
#define PWM_CHANNEL_B 1
#define PWM_FREQ 1000
#define PWM_RES 8

// Servo
#define SERVO_PIN 18
#define PWM_CHANNEL_SERVO 2
#define SERVO_FREQ 50

// LEDs
#define WIFI_LED 4
#define WS_LED 19

// WebSocket target
const char* websocket_host = "192.168.12.211";
const uint16_t websocket_port = 8888;
const char* websocket_path = "/ws_esp32";

// State
int damperAngle = 90, lastDamperAngle = -1;
bool fanForward = true;
uint8_t fanSpeed = 128;
bool wifiConnected = false;
bool wsConnected = false;

WebSocketsClient webSocket;

// Tasks
TaskHandle_t h_sensorTask, h_webSocketTask, h_damperTask;

void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void applyFanSettings() {
  digitalWrite(STBY, HIGH);
  if (fanForward) {
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW); ledcWrite(PWM_CHANNEL_B, 0);
    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); ledcWrite(PWM_CHANNEL_A, fanSpeed);
  } else {
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW); ledcWrite(PWM_CHANNEL_A, 0);
    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); ledcWrite(PWM_CHANNEL_B, fanSpeed);
  }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_CONNECTED) {
    wsConnected = true;
    // Send handshake with device_id
    DynamicJsonDocument doc(64);
    doc["device_id"] = DEVICE_ID;
    String handshake;
    serializeJson(doc, handshake);
    webSocket.sendTXT(handshake);
  }
  else if (type == WStype_DISCONNECTED) wsConnected = false;

  if (type == WStype_TEXT) {
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, payload);
    if (error || !doc.containsKey("control")) return;
    if (doc["device_id"] != DEVICE_ID) return;

    String control = doc["control"];
    if (control == "speed") fanSpeed = doc["value"];
    else if (control == "direction") {
    String direction = doc["value"].as<String>();
    fanForward = (direction == "forward");
    } else if (control == "angle") damperAngle = constrain(doc["value"], 0, 180);
    applyFanSettings();
  }
}

float readTemperature(uint8_t ch) {
  tcaSelect(ch);
  Wire.beginTransmission(TMP102_ADDR);
  Wire.write(0x00);
  if (Wire.endTransmission() != 0) return 999;
  Wire.requestFrom(TMP102_ADDR, 2);
  if (Wire.available() < 2) return 999;
  int16_t raw = (Wire.read() << 4) | (Wire.read() >> 4);
  if (raw > 0x7FF) raw |= 0xF000;
  return raw * 0.0625 - 7.8;
}

float readPressure(uint8_t ch) {
  tcaSelect(ch);
  Wire.beginTransmission(LPS25HB_ADDR);
  Wire.write(0x28 | 0x80);
  if (Wire.endTransmission() != 0) return 999;
  Wire.requestFrom(LPS25HB_ADDR, 3);
  if (Wire.available() < 3) return 999;
  uint32_t raw = Wire.read();
  raw |= (Wire.read() << 8);
  raw |= (Wire.read() << 16);
  return raw / 4096.0 + 57;
}

void setServoAngle(int angle) {
  uint32_t duty = map(angle, 0, 180, 1638, 8192);
  ledcWrite(PWM_CHANNEL_SERVO, duty);
}

void sensorTask(void* pvParams) {
  while (true) {
    float t1 = readTemperature(0), t2 = readTemperature(2);
    float p1 = readPressure(1), p2 = readPressure(3);
    float avgT = (t1 + t2) / 2.0;
    float avgP = (p1 + p2) / 2.0;

    DynamicJsonDocument doc(256);
    doc["device_id"] = DEVICE_ID;
    doc["temp"] = avgT;
    doc["pressure"] = avgP;
    doc["fan_speed"] = fanSpeed;
    doc["fan_direction"] = fanForward ? "forward" : "reverse";
    doc["damper_angle"] = damperAngle;

    String msg;
    serializeJson(doc, msg);
    if (webSocket.isConnected()) webSocket.sendTXT(msg);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void webSocketTask(void* pvParams) {
  while (true) {
    webSocket.loop();
    handleOTA();
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void damperTask(void* pvParams) {
  while (true) {
    if (abs(damperAngle - lastDamperAngle) >= 2) {
      setServoAngle(damperAngle);
      lastDamperAngle = damperAngle;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void ledTask(void* pvParams) {
  unsigned long prevMillisWiFi = millis();
  unsigned long prevMillisWS = millis();
  bool wifiLedState = false;
  bool wsLedState = false;

  while (true) {
    unsigned long now = millis();

    // WIFI_LED: toggle every 125ms (2 times per second)
    if (now - prevMillisWiFi >= 1000) {
      prevMillisWiFi = now;
      wifiLedState = !wifiLedState;
      digitalWrite(WIFI_LED, wifiLedState);
    }

    // WS_LED: toggle every 250ms (1/4 second)
    if (now - prevMillisWS >= 250) {
      prevMillisWS = now;
      wsLedState = !wsLedState;
      digitalWrite(WS_LED, wsLedState);
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);  // Prevent busy loop
  }
}


void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT); digitalWrite(STBY, HIGH);
  pinMode(WIFI_LED, OUTPUT); pinMode(WS_LED, OUTPUT);

  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RES); ledcAttachPin(PWMA, PWM_CHANNEL_A);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RES); ledcAttachPin(PWMB, PWM_CHANNEL_B);
  ledcSetup(PWM_CHANNEL_SERVO, SERVO_FREQ, 16); ledcAttachPin(SERVO_PIN, PWM_CHANNEL_SERVO);
  setServoAngle(damperAngle);
  applyFanSettings();

  for (int ch : {1, 3}) {
    tcaSelect(ch);
    Wire.beginTransmission(LPS25HB_ADDR);
    Wire.write(0x20); Wire.write(0b11000000); Wire.endTransmission();
  }

  printMacAddress();
  connectToWiFiAndStartOTA();
  wifiConnected = true;

  webSocket.begin(websocket_host, websocket_port, websocket_path);
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  xTaskCreatePinnedToCore(sensorTask,    "Sensor",    4096, NULL, 2, &h_sensorTask,    1);
  xTaskCreatePinnedToCore(webSocketTask, "WebSocket", 2048, NULL, 1, &h_webSocketTask, 1);
  xTaskCreatePinnedToCore(damperTask,    "Damper",    2048, NULL, 1, &h_damperTask,    1);
  xTaskCreatePinnedToCore(ledTask,       "LED",       1024, NULL, 1, NULL,             0);
}

void loop() {}