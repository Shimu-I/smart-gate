This sketch turns our **ESP32** into a **Wi-Fi access point + web server** that issues, lists, and validates **secure “tickets”** (temporary access tokens).

It could be used, for example, in a **smart gate / RFID door system** — the ESP32 generates short-lived tokens for authorized entries.

---

## 🔹 1. Libraries and initial setup

```cpp
//#include <Adafruit_NeoPixel.h>   // (commented out)
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "mbedtls/md.h"
#include <vector>

```

- **Adafruit_NeoPixel**: (currently commented) planned for RGB LED feedback.
- **WiFi.h**: handles Wi-Fi access-point mode and network communication.
- **WebServer.h**: lightweight HTTP server for ESP32/ESP8266.
- **ArduinoJson.h**: easy JSON handling (parse/serialize).
- **mbedtls/md.h**: provides cryptographic hashing (HMAC-SHA256).
- **vector**: C++ standard dynamic array container.

---

## 🔹 2. Wi-Fi Access-Point configuration

```cpp
const char* ap_ssid     = "ESP";
const char* ap_password = "password";

```

- The ESP32 will host its own Wi-Fi network named **“ESP”** with password **“password”**.
- We can connect our phone/laptop directly to it.

---

## 🔹 3. Secret key

```cpp
#define SECRET_KEY "X7m!a9Pq$Lf2#Dz8&Vr4^Bblackbeard"

```

- Used for **HMAC-SHA256** signing of tickets — ensures authenticity.
- Must match the key used by any other device (e.g., a gate controller).
- Think of it like a shared password for cryptographic validation.

---

## 🔹 4. Global objects

```cpp
WebServer server(80);

```

- Creates an HTTP server listening on port **80**.

```cpp
struct Ticket {
  String payload;
  uint32_t exp;
  bool used;
};
std::vector<Ticket> tickets;

```

- Defines a **Ticket** data structure:
    - `payload`: the token string itself.
    - `exp`: expiry time (in seconds since boot).
    - `used`: whether it’s already been consumed.
- `tickets`: a list (vector) storing all issued tickets.

---

## 🔹 5. Base64 URL-safe encoding

```cpp
String base64url(const uint8_t* data, size_t len) { ... }

```

- Converts binary data into a URL-safe Base64 string (using  and `_` instead of `+` and `/`).
- Used for encoding random bytes or cryptographic hashes into readable text that can be sent via HTTP or stored easily.

---

## 🔹 6. HMAC-SHA256 signature

```cpp
bool hmac_sha256(const uint8_t* key, size_t keylen,
                 const uint8_t* msg, size_t msglen,
                 uint8_t out[32]) { ... }

```

- Uses the **mbedtls** library to compute a 32-byte **HMAC-SHA256** digest:
    - Inputs: `key`, `msg` (message string).
    - Output: `out` = 32 bytes of authentication data.
- Ensures the ticket can’t be forged or modified — only systems with the same secret key can produce valid signatures.

---

## 🔹 7. Ticket issuing

```cpp
String issueTicket() {
  uint32_t now = millis()/1000;
  uint32_t exp = now + 600; // 10-minute expiry
  ...
}

```

Here’s what happens inside:

1. Get current uptime (`now`) in seconds.
2. Set expiry to 10 minutes later (`exp = now + 600`).
3. Generate an 8-byte **random nonce** using `esp_random()` for uniqueness.
4. Pack expiry + nonce into 12 bytes → `buf[12]`.
5. Convert `buf` to Base64URL → `token`.
6. Compute `HMAC-SHA256(SECRET_KEY, token)` → `mac[32]`.
7. Truncate first 12 bytes of the MAC → Base64URL → `sig`.
8. Combine them:
    
    ```
    payload = token + "." + sig
    
    ```
    
    e.g. `AbCdEfG... . xyZ123...`
    
9. Store the ticket (payload, expiry, unused) in the vector.
10. Return the payload string.

**Result:** Each ticket is unique, time-limited, and cryptographically signed.

---

## 🔹 8. Ticket validation

```cpp
bool validateTicket(String payload) { ... }

```

1. Search for a ticket in the vector matching `payload`.
2. If found:
    - Reject if already used.
    - Reject if current time > expiry.
    - Otherwise, mark as `used = true` and accept.
3. If not found → invalid.

**Purpose:** ensures each ticket can be used **only once** before expiry.

---

## 🔹 9. Ticket cleanup

```cpp
void cleanupTickets() {
  uint32_t now = millis()/1000;
  for (int i = tickets.size()-1; i>=0; i--) {
    if (tickets[i].used || now > (tickets[i].exp + 120)) {
      tickets.erase(tickets.begin()+i);
    }
  }
}

```

- Runs periodically.
- Removes tickets that are already used or expired more than 2 minutes ago.
- Keeps memory usage small.

---

## 🔹 10. HTML admin interface

```cpp
const char adminPage[] PROGMEM = R"rawliteral( ... )rawliteral";

```

- Embedded HTML/JavaScript page stored in program memory (PROGMEM).
- Provides a simple **web UI** for managing tickets.

### Features:

- “New Ticket” button → calls `/issue_ticket`.
- Shows current tickets in a table (`payload`, `expires`, `status`).
- Auto-refreshes every 5 seconds.

**Browser flow:**

1. You connect to the ESP32 Wi-Fi (“ESP”).
2. Visit `http://192.168.4.1/admin`.
3. The admin panel appears.
4. You can issue tickets and see them update live.

---

## 🔹 11. HTTP endpoints (handlers)

| Endpoint | Method | Description |
| --- | --- | --- |
| `/admin` | GET | Serves the admin HTML page |
| `/issue_ticket` | GET | Issues a new ticket and returns JSON `{payload: "..."}` |
| `/validate_ticket` | POST | Validates a ticket (from JSON body) and returns `{ok:true}` or `{ok:false}` |
| `/tickets` | GET | Returns a JSON list of all tickets with expiry/status info |

Example JSON from `/tickets`:

```json
[
  {"payload":"ABC123.DEF456","expires":520,"status":"ACTIVE"},
  {"payload":"XYZ999.QWE111","expires":0,"status":"EXPIRED"}
]

```

---

## 🔹 12. `setup()` — initialization

```cpp
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("AP started: " + String(ap_ssid));
  Serial.println("Server IP: " + WiFi.softAPIP().toString());
  ...
  server.begin();
}

```

1. Starts serial logging.
2. Configures ESP32 as a **Wi-Fi Access Point**.
3. Starts a web server and registers the handlers for all routes.
4. Prints the network SSID and IP address to the Serial Monitor.

---

## 🔹 13. `loop()` — main runtime

```cpp
void loop() {
  server.handleClient();
  if (millis()-lastCleanup>60000) {
    cleanupTickets();
    lastCleanup=millis();
  }
}

```

- Continuously:
    - Processes incoming HTTP requests.
    - Every 60 seconds → cleans up expired/used tickets.

---

## 🔹 14. (Planned) RGB feedback

```cpp
//#include <Adafruit_NeoPixel.h>
//#define RGB_PIN 48
//#define NUM_PIXELS 1
//Adafruit_NeoPixel pixels(NUM_PIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);
//todo: later include rgb code for errors/successful operations

```

- The code hints at adding an RGB LED indicator later:
    - **Green** → success
    - **Red** → error
    - **Blue** → waiting
- These lines are commented out for now.

---

## 🔹 Summary — What the Program Does

1. **Creates its own Wi-Fi network** (`ESP` / `password`).
2. Hosts a **web dashboard** to manage “tickets.”
3. Issues **time-limited HMAC-signed tokens** (10-minute expiry).
4. Allows other systems (like an RFID gate or client) to **validate** tickets.
5. Periodically cleans up expired or used tickets.
6. (Future) will blink an RGB LED to indicate system status.
