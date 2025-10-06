This program is written for an **ESP32-S3 microcontroller** that uses an **Adafruit PN532 NFC/RFID module** over **I2C** communication.

It detects RFID cards and prints their **unique identifier (UID)** to the **Serial Monitor**.

---

## 🔹 Step 1: Include necessary libraries

```cpp
#include <Wire.h>
#include <Adafruit_PN532.h>

```

- `Wire.h` → enables I2C communication (used to talk to the PN532 chip).
- `Adafruit_PN532.h` → Adafruit’s official library for handling PN532 RFID/NFC module functions.

---

## 🔹 Step 2: Define I2C pins

```cpp
#define SDA_PIN 21
#define SCL_PIN 17

```

- Defines which pins of the ESP32-S3 will be used for I2C communication:
    - **SDA (Serial Data Line)** = 21
    - **SCL (Serial Clock Line)** = 17

Different ESP32 boards can have different pin mappings — so this tells the program which ones to use.

---

## 🔹 Step 3: Create PN532 instance

```cpp
Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

```

- This creates an object named `nfc` to control the PN532.
- It tells the library to use I2C with the pins defined above.

---

## 🔹 Step 4: Setup function

The `setup()` function runs **once** when the device starts.

### a) Start Serial Monitor

```cpp
Serial.begin(9600);
Serial.println("Initializing PN532 RFID reader (I2C mode)...");

```

- Initializes serial communication at **9600 bps** so you can see messages on your computer.
- Prints a status message.

---

### b) Initialize I2C communication

```cpp
Wire.begin(SDA_PIN, SCL_PIN);

```

- Starts the I2C bus with the chosen pins.
- Without this, the ESP32 won’t be able to communicate with the PN532.

---

### c) Initialize PN532 module

```cpp
nfc.begin();

```

- Initializes the PN532 hardware and prepares it for commands.

---

### d) Check if PN532 is detected

```cpp
uint32_t versiondata = nfc.getFirmwareVersion();
if (!versiondata) {
  Serial.println("Didn't find PN532 chip. Check wiring or I2C mode jumpers.");
  while (1);
}

```

- Sends a command to the PN532 asking for its firmware version.
- If it doesn’t respond (`versiondata == 0`), the program stops (`while(1)` freezes it) and prints an error message.
- This helps confirm that the module is connected and working.

---

### e) Print firmware info

```cpp
Serial.print("Found PN532 chip with firmware version: ");
Serial.print((versiondata >> 16) & 0xFF, DEC);
Serial.print('.');
Serial.println((versiondata >> 8) & 0xFF, DEC);

```

- The firmware version is encoded in a 32-bit number.
- Bit shifting (`>>`) extracts the version’s **major** and **minor** parts:
    - `(versiondata >> 16) & 0xFF` → major version
    - `(versiondata >> 8) & 0xFF` → minor version
- Prints something like:
    
    `Found PN532 chip with firmware version: 1.6`
    

---

### f) Configure PN532 for reading RFID cards

```cpp
nfc.SAMConfig();

```

- Configures the PN532’s **SAM (Secure Access Module)** mode so it can read passive RFID/NFC tags.
- It’s required before calling `readPassiveTargetID()`.

---

### g) Ready message

```cpp
Serial.println("PN532 ready! Tap an RFID card...");
Serial.println("--------------------------------------");

```

- Prints a message indicating that everything is ready.

---

## 🔹 Step 5: Loop function

The `loop()` function runs **continuously** after setup.

### a) Declare variables

```cpp
uint8_t success;
uint8_t uid[7];
uint8_t uidLength;

```

- `success`: indicates whether a card was detected.
- `uid`: array to store the card’s **unique ID** (up to 7 bytes long).
- `uidLength`: actual length of the UID (some cards use 4 bytes, some 7).

---

### b) Check for RFID card

```cpp
success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

```

- The PN532 listens for **MIFARE / ISO14443A** RFID tags.
- If a card is detected:
    - `success` = `1` (true)
    - `uid` contains the card’s ID bytes
    - `uidLength` tells how many bytes the ID has

If no card is present, `success = 0`.

---

### c) If a card is detected, print its UID

```cpp
if (success) {
  Serial.print("RFID Card Detected! UID: ");
  for (uint8_t i = 0; i < uidLength; i++) {
    if (uid[i] < 0x10) Serial.print("0");
    Serial.print(uid[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.println("--------------------------------------");

  delay(2000);
}

```

- Loops through each byte in the UID and prints it in **hexadecimal** format.
- Adds a leading `0` for single-digit bytes (for consistent formatting).
- Example output:
    
    ```
    RFID Card Detected! UID: 04 A3 B2 6F
    --------------------------------------
    
    ```
    
- Waits **2 seconds** before scanning again (`delay(2000)`), to avoid reading the same card multiple times rapidly.

---

## 🔹 Summary of Program Flow

1. Setup serial and I2C communication.
2. Initialize PN532 and check if it’s working.
3. Configure PN532 for RFID tag detection.
4. Continuously check for cards in the loop.
5. When a card is found, print its UID to the serial monitor.
