#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <qrcode_st7735.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <HTTPClient.h>

// ====== WiFi Access Point ======
const char *ssid = "ESP";
const char *password = "password";
WebServer server(80);

// ====== TFT Display ======
#define TFT_CS   10
#define TFT_RST  4
#define TFT_DC   2
#define TFT_MOSI 11
#define TFT_SCLK 12

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
QRcode_ST7735 qrcode(&tft);

// ====== PN532 RFID ======
#define PN532_SDA 21
#define PN532_SCL 17
Adafruit_PN532 nfc(PN532_SDA, PN532_SCL);

// ====== Ticket System ======
#define MAX_TICKETS 20
const unsigned long TICKET_LIFETIME_MS = 10UL * 60UL * 1000UL; // 10 min
const unsigned long DELETE_AFTER_MS = 2UL * 60UL * 1000UL;      // 2 min after expiry

struct Ticket {
  String id;
  unsigned long expiry;
  bool active;
  int scans;
};
Ticket tickets[MAX_TICKETS];

// ====== Gate Control ======
const char* gateURL = "http://192.168.4.3/opengate";  // Gate ESP32's IP

// ====== Helper Functions ======
String randomTicket() {
  const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  String out;
  for (int i = 0; i < 8; i++) out += chars[esp_random() % (sizeof(chars) - 1)];
  return out;
}

void drawQR(const String &text) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(6, 6);
  tft.print("Ticket:");
  tft.setCursor(6, 18);
  tft.print(text);
  delay(100);
  qrcode.create(text.c_str());
  Serial.println("QR displayed: " + text);
}

void hideQR() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(6, 6);
  tft.setTextColor(ST77XX_RED);
  tft.print("QR Hidden");
  Serial.println("QR hidden.");
}

int findActiveTicketIndex(const String &id) {
  unsigned long now = millis();
  for (int i = 0; i < MAX_TICKETS; i++) {
    if (tickets[i].active && tickets[i].id == id && tickets[i].expiry > now)
      return i;
  }
  return -1;
}

int findFreeSlot() {
  unsigned long now = millis();
  for (int i = 0; i < MAX_TICKETS; i++)
    if (!tickets[i].active) return i;

  for (int i = 0; i < MAX_TICKETS; i++) {
    if (tickets[i].active && (now > tickets[i].expiry + DELETE_AFTER_MS)) {
      tickets[i].active = false;
      tickets[i].id = "";
      return i;
    }
  }
  return -1;
}

void addOrRefreshTicket(const String &id) {
  unsigned long now = millis();
  int idx = findActiveTicketIndex(id);
  if (idx >= 0) {
    tickets[idx].expiry = now + TICKET_LIFETIME_MS;
    tickets[idx].scans = 0;
    Serial.println("Refreshed ticket: " + id);
    drawQR(id);
    return;
  }

  int slot = findFreeSlot();
  if (slot < 0) {
    Serial.println("No free ticket slots!");
    return;
  }

  tickets[slot].id = id;
  tickets[slot].expiry = now + TICKET_LIFETIME_MS;
  tickets[slot].active = true;
  tickets[slot].scans = 0;

  Serial.println("Created ticket: " + id);
  drawQR(id);
}

// ====== HTML Page ======
String htmlPage() {
  String html = "<!doctype html><html><head><meta charset='utf-8'><title>ESP Gate Server</title>";
  html += "<style>body{font-family:Arial;text-align:center;}table{margin:auto;border-collapse:collapse;}td,th{border:1px solid #999;padding:8px;}button{padding:8px 15px;margin:5px;}</style>";
  html += "</head><body><h2>ESP32-S3 Ticket Server</h2>";
  html += "<form action='/new' method='POST' style='display:inline;'><button>Create New Ticket</button></form>";
  html += "<form action='/hideqr' method='POST' style='display:inline;'><button>Hide QR</button></form>";
  html += "<form action='/scanrfid' method='POST' style='display:inline;'><button>Scan RFID</button></form>";
  html += "<table><tr><th>ID</th><th>Expires(s)</th><th>Scans</th><th>Actions</th></tr>";

  unsigned long now = millis();
  for (int i = 0; i < MAX_TICKETS; i++) {
    if (!tickets[i].active) continue;
    long rem = (long)(tickets[i].expiry > now ? (tickets[i].expiry - now) / 1000 : 0);
    html += "<tr><td>" + tickets[i].id + "</td><td>" + String(rem) + "</td><td>" + String(tickets[i].scans) + "</td>";
    html += "<td><a href='/showqr?id=" + tickets[i].id + "'><button>Show QR</button></a>";
    html += "<a href='/opengate'><button>OG</button></a></td></tr>";
  }

  html += "</table><p>Tickets valid 10 min, usable twice, removed 2 min after expiry.</p></body></html>";
  return html;
}

// ====== Web Handlers ======
void handleRoot() { server.send(200, "text/html", htmlPage()); }

void handleNew() {
  addOrRefreshTicket(randomTicket());
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleShowQR() {
  if (server.hasArg("id")) {
    String id = server.arg("id");
    drawQR(id);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleHideQR() {
  hideQR();
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleScanRFID() {
  server.send(200, "text/plain", "Waiting for RFID scan...");
  Serial.println("RFID scan started — place your card.");

  uint8_t uid[7];
  uint8_t uidLen;
  unsigned long start = millis();

  while (millis() - start < 10000) { // wait up to 10s
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 50)) {
      char buf[32] = {0};
      for (uint8_t i = 0; i < uidLen; i++) sprintf(buf + strlen(buf), "%02X", uid[i]);
      String cardID = String(buf);
      Serial.println("RFID Card scanned: " + cardID);
      addOrRefreshTicket(cardID);
      return;
    }
    delay(50);
  }

  Serial.println("RFID scan timeout.");
}

void handleOpenGate() {
  HTTPClient http;
  http.begin(gateURL);
  int code = http.GET();
  http.end();

  if (code == 200) Serial.println("Gate open command sent.");
  else Serial.println("Gate command failed.");

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleValidate() {
  if (!server.hasArg("id")) { server.send(400, "text/plain", "Missing id"); return; }
  String id = server.arg("id");
  unsigned long now = millis();
  bool ok = false;
  for (int i = 0; i < MAX_TICKETS; i++) {
    if (tickets[i].active && tickets[i].id == id && tickets[i].expiry > now && tickets[i].scans < 2) {
      tickets[i].scans++;
      ok = true;
      break;
    }
  }
  if (ok) server.send(200, "text/plain", "VALID");
  else server.send(403, "text/plain", "INVALID");
}

// ====== Setup ======
void setup() {
  Serial.begin(9600);
  delay(100);
  randomSeed(analogRead(0));

  // TFT
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setCursor(6, 6);
  tft.println("ESP32-S3 Server Booting...");
  qrcode.init();

  // PN532
  Wire.begin(PN532_SDA, PN532_SCL);
  nfc.begin();
  uint32_t ver = nfc.getFirmwareVersion();
  if (!ver) {
    Serial.println("PN532 not found!");
    tft.setCursor(6, 24);
    tft.println("PN532 not found!");
  } else {
    nfc.SAMConfig();
    Serial.println("PN532 Ready.");
  }

  // Wi-Fi AP
  WiFi.softAP(ssid, password);
  Serial.println("AP started: " + String(ssid));
  Serial.print("IP: "); Serial.println(WiFi.softAPIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/new", HTTP_POST, handleNew);
  server.on("/showqr", handleShowQR);
  server.on("/hideqr", HTTP_POST, handleHideQR);
  server.on("/scanrfid", HTTP_POST, handleScanRFID);
  server.on("/opengate", handleOpenGate);
  server.on("/validate", handleValidate);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  // Cleanup expired tickets
  unsigned long now = millis();
  for (int i = 0; i < MAX_TICKETS; i++) {
    if (tickets[i].active && (now > tickets[i].expiry + DELETE_AFTER_MS)) {
      tickets[i].active = false;
      tickets[i].id = "";
      tickets[i].scans = 0;
    }
  }
}
