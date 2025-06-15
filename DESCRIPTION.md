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

LeakLock is a smart, affordable HVAC fault detection system designed to identify duct leaks and airflow inefficiencies in real-time. Built using ESP32 microcontrollers, distributed pressure and temperature sensors, and a web-based dashboard, it enables intelligent zone-based regulation across a three-zone mock HVAC duct. OTA updates, Wi-Fi connectivity, and live dashboard control make it scalable and user-friendly. LeakLock operates under $50 and offers an alternative to expensive commercial systems, making HVAC optimization accessible to both residential and developing regions.

## Introduction

LeakLock aims to solve the often-overlooked problem of HVAC inefficiency due to undetected faults. This embedded system monitors airflow and temperature data, then responds with fan control and damper adjustments to restore balance. The solution uses off-the-shelf hardware and a custom interface built with HTML/CSS/JS, focusing on simplicity and affordability compared to commercial alternatives.

### Problem Statement

HVAC systems lose energy due to hidden inefficiencies like duct leaks and sensor failures. Most current solutions are expensive, require complex setup, and lack real-time response. LeakLock addresses these limitations by offering a modular, responsive, low-cost alternative for multi-zone HVAC diagnostics.

## Design Requirements and Constraints

### Key Requirements
- Multi-zone temperature and pressure monitoring
- Real-time sensor feedback and response
- OTA firmware updates over Wi-Fi
- Live dashboard for monitoring and manual overrides
- Target system cost under $50

### Design Constraints
- Budget limited sensor precision (e.g., low Pa sensitivity in LPS25HB)
- Component integration required space optimization
- I²C channel limits required a multiplexer
- Environmental simulation with 3D printed ducts lacked real-world turbulence/humidity

## Engineering Standards

- Software written in Arduino C++ with modular FreeRTOS tasks
- Sensors used I²C protocol, communication handled via WebSocket (RFC 6455)
- PCB followed best practices in bypassing, grounding, and 3.3V regulation
- Fail-safes included timeout-based actuator shutdowns and startup defaults

## Broader Considerations

LeakLock's accessibility has wide-reaching implications for energy conservation, especially in underserved regions. Its privacy-respecting local data use and intuitive UI encourage widespread adoption. Future improvements could include encryption and authentication for remote access.

## Design Description

### System Overview
- TMP102 (Temp), LPS25HB (Pressure), 5V fans, servos per zone
- ESP32 Wroom-32E controls each zone, communicates via WebSockets
- Web UI supports 3D HVAC model and manual control

### Block Diagram
> *(Insert diagram: block_diagram.png)*

### Firmware and Code
- Structured around FreeRTOS for modular tasking
- Handles sensor reads, UI communication, actuator control, OTA updates
- Uses TCA I²C MUX to manage multiple sensors per board

### 3D Printing
LeakLock uses a T-shaped 3D printed duct system sealed with aluminum tape to simulate real HVAC airflow. It allows testing of backpressure effects and zone control but lacks full thermal or turbulent fidelity.

### PCB Design
Two iterations were produced:
1. Breakout + breadboard with separate ESP32 (60x70mm)
2. Final compact board (45x50mm) with integrated microcontroller, motor driver, MUX, and indicator LEDs

## Final Product

### Initial Goal vs Final Product
- Retained core goal of <$50 cost per module (actual: $16.35)
- UI moved from 3D model to reliable data display
- RTOS replaced loop-based firmware for better parallelism
- Dampers functional but not mechanically integrated due to fit challenges

## Performance and Limitations

- System latency: <150ms round trip
- FreeRTOS improved responsiveness and multitasking
- Sensor precision limited by hardware
- UI does not currently support trend logging or analytics

## Challenges Encountered

- Mechanical issues with damper integration in 3D duct
- ML model failed due to lack of labeled datasets (43.6% error)
- I²C misreads from misinterpreting 16-bit register format
- Early complexity managing multiple sensors and task flows

## Planning and Organization

Project executed over 8 weeks with milestones tracked via Gantt chart. Major tasks included PCB design, mechanical build, UI development, and firmware migration to RTOS.

## Bill of Materials (BOM)

| Component                | Quantity | Unit Price | Total |
|--------------------------|----------|------------|-------|
| TB6612FNG H-Bridge       | 5        | $1.82      | $9.10 |
| TMP102 Temp Sensor       | 12       | $0.69      | $8.28 |
| LPS25HB Pressure Sensor  | 12       | $4.62      | $55.44|
| TCA9548A I²C Mux         | 5        | $1.17      | $5.85 |
| Passive Components       | —        | —          | ~$10  |
| **Total Cost (approx.)** |          |            | **$93.26** |

## Interview Results and Market Research

Professionals and renters alike expressed interest in an affordable, intelligent HVAC tool. Market size expected to reach $479M by 2033. Key interview insights emphasized automation, accessibility, cost-efficiency, and security. LeakLock compares favorably to $500–$2000 commercial competitors like Ecobee, Robinair, and INFICON.

## Conclusion

LeakLock brought together hardware, firmware, UI, and real-world design principles into a system that’s scalable and cost-effective. Despite setbacks like sensor limitations and incomplete ML models, it stands as a robust foundation for future HVAC optimization tools. Future directions include cloud integration, mobile apps, better prediction models, and improved mechanical integration.

## References

- Chvala, W. D. (2021). *Duct leakage in residential homes (FY1024)*. University of Florida IFAS Extension. https://edis.ifas.ufl.edu/publication/FY1024  
- Market Growth Reports (2025). *Leak Detection Equipment Market Forecast*. https://www.marketgrowthreports.com/market-reports/leak-detection-equipment-market-111538  
