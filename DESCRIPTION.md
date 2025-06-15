
from pathlib import Path

# Load the raw content into a Path object for reference
file_path = Path("/mnt/data/description.md")

# We'll write a structured and formatted Markdown version of the description
formatted_description = """\
# LeakLock: HVAC Fault Detection and Correction System

**Northwestern University**  
**Department of Electrical and Computer Engineering**  
**ELEC_ENG 327: Electronic System Design II**  
**Project Report by Bemnet Tefera – May 2025**

---

## 📑 Table of Contents

1. [Abstract](#abstract)  
2. [Introduction](#introduction)  
3. [Design Requirements and Constraints](#design-requirements-and-constraints)  
4. [Engineering Standards](#engineering-standards)  
5. [Broader Considerations](#broader-considerations)  
6. [Design Description](#design-description)  
   - [System Overview](#system-overview)  
   - [Block Diagram](#block-diagram)  
   - [Firmware and Code](#firmware-and-code)  
   - [3D Printing](#3d-printing)  
   - [PCB Design](#pcb-design)  
7. [Final Product](#final-product)  
8. [Performance and Limitations](#performance-and-limitations)  
9. [Challenges Encountered](#challenges-encountered)  
10. [Planning and Organization](#planning-and-organization)  
11. [Bill of Materials (BOM)](#bill-of-materials-bom)  
12. [Interview Results and Market Research](#interview-results-and-market-research)  
13. [Conclusion](#conclusion)  
14. [References](#references)  

---

## Abstract

*HVAC systems are essential for maintaining our indoor comfort...*  
[Insert full paragraph here]

## Introduction

*The goal of this project was to design and implement...*

### Problem Statement

*HVAC systems are prone to efficiency losses due to...*

## Design Requirements and Constraints

### Key Requirements
- Multi-zone monitoring
- Real-time responsiveness
- Affordability
- Wireless communication (Wi-Fi & OTA)
- Interactive web interface

### Design Constraints
- Budget limitations
- Sensor sensitivity and resolution issues
- Environmental simulation challenges
- Timeline limitations

## Engineering Standards

- Arduino C++ framework
- I²C, SPI, WebSocket (RFC 6455)
- Power integrity and grounding principles
- Basic safety fallbacks on timeout/disconnect

## Broader Considerations

- Potential energy savings
- Ethical considerations on data privacy
- Focus on accessibility and inclusivity

## Design Description

### System Overview
- Sensors: TMP102, LPS25HB
- Actuators: 5V Fans and Servo
- ESP32 Wroom-32E microcontroller
- Communication: Wi-Fi + WebSocket
- Dashboard: HTML, CSS, JavaScript

### Block Diagram
> ![Block Diagram](images/block_diagram.png)

### Firmware and Code

Key Features:
- FreeRTOS-based task separation
- Real-time sensor reading & averaging
- WebSocket message handling
- Fan speed/direction control
- Servo angle update logic

Example Code Block:
```cpp
void sensorTask(void* pvParams) {
  while (true) {
    float t1 = readTemperature(0), t2 = readTemperature(2);
    float p1 = readPressure(1), p2 = readPressure(3);
    float avgT = (t1 + t2) / 2.0;
    float avgP = (p1 + p2) / 2.0;

    // JSON serialization & send logic here...
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
