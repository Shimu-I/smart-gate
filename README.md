![GitHub repo size](https://img.shields.io/github/repo-size/Shimu-I/smart-gate)
![GitHub last commit](https://img.shields.io/github/last-commit/shimu-i/smart-gate)
![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Made with Arduino](https://img.shields.io/badge/made%20with-Arduino-00979D?logo=arduino)
![Visitor Count](https://visitor-badge.laobi.icu/badge?page_id=shimu-i/smart-gate)


## 🚡 [project presentation link](https://www.canva.com/design/DAGxvjW1ttA/XTJ3PxfRbfMikCMG0TYQAw/edit?utm_content=DAGxvjW1ttA&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)
## 🖥️ [presentation file](SmartGate.pptx/)
## 📄 [project proposal file](proposal.docx/)
## 🎥 [demonstration video]()


# Smart Metro Gate Access System Using ESP32

## Motivation

During the July 2024 student revolution in Bangladesh, many metro station gates were damaged due to failures in the centralized ticketing system, causing chaos and unrest. This project aims to develop a **low-cost, reliable, and decentralized smart gate system** that keeps functioning even when the internet or central servers are down. Our goal is to ensure smoother passenger flow, safer public transport, and robust operation during emergencies.

---

## 🚀 Project Overview

The **Smart Metro Gate Access System Using ESP32** is a **decentralized, dual-access metro entry system** that operates independently of internet or centralized servers. It is designed to ensure uninterrupted passenger flow during both normal operations and network outages.

Unlike Dhaka Metro’s current setup — which relies heavily on centralized ticket servers and mechanical gates — this system uses **multiple ESP32 microcontrollers** to locally handle ticket generation, validation, and gate control within a **self-contained Wi-Fi network**.

The system consists of three main units:

1. **ESP32 Server Unit (Main Controller):**
    - Acts as a local Wi-Fi Access Point and database server.
    - Hosts a web-based ticket management interface where administrators can issue, view, and delete tickets.
    - Generates unique, time-limited tickets secured with **HMAC-SHA256 encryption** and displays them as **QR codes** on a TFT screen.
2. **Gate Control Unit (ESP32-S3):**
    - Connects to the server via the local Wi-Fi network.
    - Controls a **servo motor** that physically opens or closes the gate.
    - Uses **two IR sensors** to detect passenger presence and direction, ensuring safe gate operation.
3. **ESP32-CAM & PN532 Reader Unit:**
    - **ESP32-CAM** scans QR code tickets presented by passengers.
    - **PN532 RFID module** reads RFID cards for alternate access.
    - Both modules communicate with the server to validate user credentials.

All components communicate wirelessly within a **local network**, allowing the entire system to operate **offline**. Ticket validation, gate actuation, and expiration checks occur locally — ensuring robustness even when internet connectivity or central infrastructure fails.

In short, the system demonstrates how **low-cost microcontrollers** can be combined to create a **resilient, intelligent, and secure metro gate solution** suitable for modern urban transportation.
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

## 🛠️ Project Updates

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

## ⚙️ How It Works

The **Smart Metro Gate Access System** operates through a **local Wi-Fi network** managed by one ESP32 module acting as the **main server**. Other ESP32 devices (such as the gate controller and camera unit) connect to it to perform specific tasks. The system is designed to continue operating even when offline, making it reliable in emergencies or unstable network conditions.

### 🔹 Step 1: Ticket Creation and Management

- The **main ESP32 server** runs a local **Wi-Fi Access Point (AP)** and hosts a **web-based ticket management interface**.
- Passengers or administrators can connect to the ESP32’s Wi-Fi and generate tickets through this web page.
- Each ticket is:
    - A **unique, randomly generated token** signed with **HMAC-SHA256** for security.
    - Stored temporarily in the ESP32’s memory.
    - Set to **expire after 10 minutes** to prevent misuse or sharing.
- The server also displays the generated ticket as a **QR code** on a TFT display for easy scanning.

### 🔹 Step 2: Passenger Detection

- Two **IR sensors** are installed near the gate to detect passenger presence and direction of movement.
- When a person approaches:
    - The **first IR sensor** detects entry.
    - The **second IR sensor** confirms movement through the gate.
- This dual-sensor setup helps the system **differentiate between entry and exit** and prevents false triggering.

### 🔹 Step 3: Ticket Verification

- Passengers can authenticate using **either**:
    - A **QR code**, scanned by the **ESP32-CAM**, or
    - An **RFID card**, read by the **PN532 RFID module**.
- Once scanned or tapped:
    - The corresponding ESP32 sends the ticket data (token or UID) to the **main ESP32 server** over the local Wi-Fi network.
    - The server checks whether the ticket or card is **valid, unused, and unexpired** in its local database.

### 🔹 Step 4: Gate Control

- If the validation is successful:
    - The **main ESP32** sends a command to the **gate ESP32 module**, which controls a **servo motor**.
    - The servo rotates **90° to open** the gate, allowing the passenger to pass.
    - After the IR sensors confirm passage, the servo **returns to the closed position**.
- Invalid or expired tickets trigger a **denial signal**, keeping the gate closed.

### 🔹 Step 5: Data Handling and Cleanup

- Each used ticket is immediately **marked as used** and **removed from memory**.
- A background process automatically deletes **expired tickets** to free up space and maintain security.
- The system requires no external storage or internet, keeping all validation **local and secure**.

---

## ✅ Advantages

- **1. Offline Operation:**
    
    Works without internet or centralized servers, ensuring continuous functionality during outages or emergencies.
    
- **2. Low-Cost Implementation:**
    
    Uses affordable components like ESP32, PN532, and IR sensors — ideal for developing countries or small-scale transport systems.
    
- **3. Dual Access System:**
    
    Supports both QR codes and RFID cards, offering flexibility for users and redundancy in case one system fails.
    
- **4. Local Ticket Validation:**
    
    Reduces dependency on remote databases and prevents network delays or failures from affecting gate operation.
    
- **5. Security & Anti-Reuse:**
    
    Each ticket is encrypted with HMAC-SHA256, expires after 10 minutes, and is automatically deleted after use.
    
- **6. Scalability:**
    
    Can be easily expanded to support multiple gates within a local network or connected later to a central monitoring system.
    
- **7. Compact & Modular Design:**
    
    The hardware is lightweight and portable, making it easy to demonstrate, test, or integrate into existing gate systems.
    

---

## ⚠️ Disadvantages

- **1. Limited Range for RFID Reading:**
    
    PN532 modules have short detection distances, requiring cards to be placed close to the reader.
    
- **2. No Central Database Synchronization (Yet):**
    
    Since it’s locally operated, tickets or user data aren’t shared between gates unless manually configured.
    
- **3. Hardware Fragility:**
    
    Breadboard or PVC-based setups can be prone to connection issues or instability during transport or extended use.
    
- **4. Limited Processing Power:**
    
    ESP32 has restricted memory and CPU capacity, which may limit handling of high passenger volumes in large stations.
    
- **5. Manual Maintenance:**
    
    Requires occasional reboots or resets to clear memory and ensure stable long-term performance.
    
- **6. Limited Visual Feedback:**
    
    Small TFT displays may not provide enough visibility in bright outdoor conditions or for larger crowds.
    

---

