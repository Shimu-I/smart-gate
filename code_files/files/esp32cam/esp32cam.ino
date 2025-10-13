#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <string.h>
#include <stdlib.h>
#include <quirc.h>  // QR decoder

// ===== WiFi =====
const char* ssid = "ESP";
const char* password = "password";
const char* serverValidateURL = "http://192.168.4.1/validate?id=";  // server ESP32 IP

// ===== Pins =====
#define IR_PIN     13
#define SERVO_PIN  14

// ===== Camera Pins (AI Thinker) =====
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===== Globals =====
Servo gateServo;
struct quirc *qr = NULL;
TaskHandle_t qrTaskHandle = NULL;
volatile bool newFrame = false;
camera_fb_t *fb = NULL;
bool gateOpen = false;

// ===== Functions =====
void openGate() {
  if (gateOpen) return;
  gateOpen = true;
  Serial.println("Gate opening...");
  gateServo.write(90);  // open
  delay(3000);
  gateServo.write(0);   // close
  gateOpen = false;
}

void validateQR(String code) {
  HTTPClient http;
  String url = String(serverValidateURL) + code;
  http.begin(url);
  int httpCode = http.GET();

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("Server response: " + payload);
    if (payload == "VALID") {
      openGate();
    } else {
      Serial.println("Invalid or expired ticket");
    }
  } else {
    Serial.println("Failed to reach server");
  }

  http.end();
}

// ===== QR Processing Task =====
void qrProcessTask(void *pvParameters) {
  struct quirc *local_qr = (struct quirc *)pvParameters;
  int w, h;

  while (1) {
    if (newFrame && fb != NULL) {
      uint8_t *image = quirc_begin(local_qr, &w, &h);
      if (image && fb->len >= w * h) {
        memcpy(image, fb->buf, w * h);
        quirc_end(local_qr);

        int count = quirc_count(local_qr);
        if (count > 0) {
          struct quirc_code code;
          struct quirc_data data;
          quirc_extract(local_qr, 0, &code);
          if (quirc_decode(&code, &data) == 0) {
            String qrText = String((char*)data.payload);
            Serial.println("QR detected: " + qrText);
            validateQR(qrText);
          }
        }
      }

      esp_camera_fb_return(fb);
      fb = NULL;
      newFrame = false;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);  // avoid CPU overuse
  }
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);

  gateServo.attach(SERVO_PIN);
  gateServo.write(0);

  // --- WiFi ---
  Serial.println("Connecting WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  // --- Camera Config ---
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_YUV422;
  config.frame_size = FRAMESIZE_QVGA;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return;
  }

  // --- QR Init ---
  qr = quirc_new();
  if (!qr) {
    Serial.println("Failed to allocate quirc");
    return;
  }

  if (quirc_resize(qr, 320, 240) < 0) {
    Serial.println("Failed to resize quirc buffer");
    return;
  }

  // --- QR Task ---
  xTaskCreatePinnedToCore(qrProcessTask, "qrTask", 8192, qr, 1, &qrTaskHandle, 1);

  Serial.println("ESP32-CAM QR gate ready");
}

// ===== Loop =====
void loop() {
  // Detect presence (optional)
  if (digitalRead(IR_PIN) == LOW) {
    Serial.println("Presence detected near gate");
  }

  // Capture frame
  if (!newFrame && fb == NULL) {
    fb = esp_camera_fb_get();
    if (fb) newFrame = true;
  }

  delay(100);
}
