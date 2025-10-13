#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_PN532.h>

// ====== Gate Hardware Pins ======
#define SERVO_PIN 14
#define IR_SENSOR 13
#define PN532_SDA 21
#define PN532_SCL 17

// ====== WiFi Settings ======
const char* ssid = "ESP";
const char* password = "password";
const char* serverURL = "http://192.168.4.1/validate"; // server validation endpoint

// ====== Objects ======
WebServer server(80);
Servo gateServo;
Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);

// ====== State ======
bool gateOpen = false;
unsigned long openTs = 0;
const unsigned long OPEN_MS = 3000; // open duration
unsigned long lastRFIDCheck = 0;
const unsigned long RFID_INTERVAL = 500; // 0.5 sec

// ====== Functions ======
void openGate() {
  gateServo.write(90); // adjust angle for your servo
  gateOpen = true;
  openTs = millis();
  Serial.println("✅ Gate opened");
}

void closeGate() {
  gateServo.write(0);
  gateOpen = false;
  Serial.println("❌ Gate closed");
}

void handleOpen() {
  openGate();
  server.send(200, "text/plain", "OK");
}

bool validateTicket(String id) {
  HTTPClient http;
  String url = String(serverURL) + "?id=" + id;
  http.begin(url);
  int code = http.GET();
  if (code == 200) {
    String res = http.getString();
    http.end();
    if (res == "VALID") {
      Serial.println("✅ VALID Ticket: " + id);
      return true;
    } else {
      Serial.println("❌ INVALID Ticket: " + id);
      return false;
    }
  } else {
    Serial.println("⚠️ Server not reachable!");
  }
  http.end();
  return false;
}

// ====== Setup ======
void setup() {
  Serial.begin(9600);
  delay(100);

  // Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("IP: "); Serial.println(WiFi.localIP());

  // Servo
  gateServo.attach(SERVO_PIN);
  gateServo.write(0);

  // IR Sensor
  pinMode(IR_SENSOR, INPUT);

  // PN532 RFID
  Wire.begin(PN532_SDA, PN532_SCL);
  nfc.begin();
  uint32_t ver = nfc.getFirmwareVersion();
  if (!ver) {
    Serial.println("❌ PN532 not found!");
  } else {
    nfc.SAMConfig();
    Serial.println("✅ PN532 Ready for RFID scan");
  }

  // Web handler
  server.on("/opengate", HTTP_GET, handleOpen);
  server.begin();
  Serial.println("🌐 Gate server ready");
}

// ====== Loop ======
void loop() {
  server.handleClient();

  unsigned long now = millis();

  // ===== RFID Scan every 0.5 sec =====
  if (now - lastRFIDCheck >= RFID_INTERVAL) {
    lastRFIDCheck = now;

    uint8_t uid[7];
    uint8_t uidLen = 0;

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 50)) {
      char buf[32] = {0};
      for (uint8_t i = 0; i < uidLen; i++) sprintf(buf + strlen(buf), "%02X", uid[i]);
      String cardID = String(buf);
      Serial.println("🔍 RFID Detected: " + cardID);

      if (validateTicket(cardID)) {
        openGate();
      } else {
        Serial.println("Access Denied!");
      }

      delay(1000); // debounce delay
    }
  }

  // ===== IR Sensor Manual Trigger =====
  if (digitalRead(IR_SENSOR) == LOW) {
    openGate();
  }

  // ===== Auto Close =====
  if (gateOpen && millis() - openTs >= OPEN_MS && digitalRead(IR_SENSOR) == HIGH) {
    closeGate();
  }
}
