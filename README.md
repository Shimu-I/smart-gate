

## [project presentation link🚡](https://www.canva.com/design/DAGxvjW1ttA/XTJ3PxfRbfMikCMG0TYQAw/edit?utm_content=DAGxvjW1ttA&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)
## [presentation file](SmartGate.pptx/)
## [project proposal file](proposal.docx/)
## [demonstration video]()


# Smart Metro Gate Access System Using ESP32

## Motivation

During the July 2024 student revolution in Bangladesh, many metro station gates were damaged due to failures in the centralized ticketing system, causing chaos and unrest. This project aims to develop a **low-cost, reliable, and decentralized smart gate system** that keeps functioning even when the internet or central servers are down. Our goal is to ensure smoother passenger flow, safer public transport, and robust operation during emergencies.

---

## Project Overview

The current metro rail system in Dhaka depends on centralized servers, plastic smart cards, and mechanical gates. Once these fail, the system stops. To address this, we designed a **dual-access metro gate** powered by ESP32 microcontrollers.

- **Passenger Access:** QR code or RFID card.
- **Local Operation:** System runs on a local Wi-Fi network, independent of the internet or central server.
- **Secure & Reliable:** Works during network failures, ensuring uninterrupted service.

### System Diagram

```
        ┌──────────────┐         Wi-Fi (Local AP)        ┌──────────────┐
        │   Server     │◄──────────────────────────────►│   Gate ESP32 │
        │ ESP32 (AP)   │                                 │   (Servo +   │
        │ - Ticket DB  │                                 │   IR + PN532)│
        │ - QR Display │                                 └──────────────┘
        │ - RFID Write │
        └──────┬───────┘
               │
               ▼
        ┌──────────────┐
        │ ESP32-CAM    │
        │ - QR Scan    │
        │ - Validate   │
        └──────────────┘

```

---

## Features

- Passengers can enter using **QR codes** or **RFID cards**.
- Ticket validation happens **locally**, without internet dependency.
- Tickets **expire after 10 minutes** and are deleted automatically.
- **Two IR sensors** detect human presence for safe gate operation.
- **Servo motor** controls the gate barrier for smooth operation.

---

## Hardware Components

| Component | Quantity | Role in Project |
| --- | --- | --- |
| ESP32-S3 Dev Module | 2 | Controls servo, IR, and RFID; manages gate system logic |
| ESP32-CAM | 1 | Scans and validates QR code tickets |
| ESP32 Dev Module (Server) | 1 | Runs Wi-Fi AP, ticket database, GUI, and QR display |
| 2.4 inch TFT Display (ILI9341) | 1 | Displays generated QR codes |
| PN532 RFID Module | 2 | Reads and writes RFID cards for passenger authentication |
| Servo Motor | 2 | Opens and closes the gate automatically (180° rotation, 90° each side) |
| IR Sensors | 2 | Detects passenger entry and exit; measures distance via infrared reflection |
| Soldering Lead & Wires | - | Connects all electronic components securely |
| PVC Board | 1 | Provides a stable structure for mounting hardware |

---

## Project Updates

### **1st Update: Ticket Generation**

- Implemented a **Wi-Fi-based ticketing system** using ESP32.
- The ESP32 creates a **local Wi-Fi network** and hosts a **web server** where tickets can be issued and monitored.
- Each ticket:
    - Is a **secure, random token** signed with **HMAC-SHA256**
    - Has a **10-minute expiry**
    - Can **only be used once**
- Tickets are stored in memory and **old or used tickets are automatically cleaned up**.
- The **admin web page** allows generating, viewing, and managing tickets through a simple interface.


### **2nd Update: Servo & IR Modules**

- **IR Modules:** Send infrared light, detect reflection time, and calculate distance to determine passenger direction.
- **Servo Motor:** Rotates 180° (90° each side) to open/close the gate when a passenger is detected.

### **3rd Update: RFID Integration**

- Set up an **ESP32 with a PN532 RFID reader** using **I2C communication**.
- The ESP32:
    - Initializes the PN532 reader and checks its firmware
    - Prepares to detect nearby RFID cards
- In the main loop:
    - Scans for RFID cards in real-time
    - Prints each card’s **UID** (unique ID) in **hexadecimal format** to the serial monitor
    - Waits 2 seconds before scanning again
- Essentially, the ESP32 functions as a **real-time RFID card reader**, enabling identification of authorized passengers at the gate.
- Hardware setup:
    - ESP32 module installed on gate side
    - Two IR modules for entry/exit detection
    - PN532 RFID reader (short range — card must be close to read)
    - PVC board for structural support

### **Next (Final) Update**

- ESP32-CAM scanning QR codes
- Display monitor showing QR codes
- Final soldering work

**Notes:**

- ESP32-CAM and display were low in stock, requiring in-person purchase.
- Assistance from a senior was needed for soldering due to limited equipment.

---

## How It Works

1. Passenger presents **RFID card** or **QR code ticket**.
2. **IR sensors** detect the passenger's presence.
3. **ESP32 server** validates ticket locally.
4. **Servo motor** opens the gate if the ticket is valid.
5. Ticket expires after 10 minutes and is deleted to prevent reuse.


