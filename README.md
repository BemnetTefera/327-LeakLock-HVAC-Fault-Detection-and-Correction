# LeakLock – Smart HVAC Fault Detection & Correction System

**LeakLock** is a low-cost, real-time HVAC monitoring solution that detects air duct leaks and corrects airflow across a three-zone system. Built with an ESP32, distributed sensors, and a web interface, it combines embedded systems, IoT, and sustainable design principles to solve a real-world energy efficiency problem.

---
![Image](https://github.com/user-attachments/assets/7bac3121-fc6c-4088-bebc-9764c4e9dd74)

# 📄 [Full Design Description (Click Here)](description.md)


## 🔧 Features

- 📶 Wi-Fi-enabled ESP32 microcontroller
- 🌡️ Real-time temperature and pressure sensing
- 🌀 Dual fan and motorized damper control
- 🖥️ Custom web interface using HTML, CSS, and JavaScript
- 🔄 OTA (Over-the-Air) updates for remote firmware management
- 🧩 PCB design files for sensor integration
- 📦 Total cost under $50

---

## 📁 Repository Structure

| File/Folder              | Description                                       |
|--------------------------|---------------------------------------------------|
| `index.html`             | Web interface frontend                           |
| `style.css`              | Styling for the frontend                         |
| `server.py`              | Tornado-based WebSocket server backend           |
| `main.cpp`               | ESP32 firmware: sensor reading & motor control   |
| `wifi_manager.cpp/h`     | Wi-Fi connection handler                         |
| `sensor_final_v1.brd`    | PCB board layout (EAGLE)                         |
| `sensor_final_v1.sch`    | PCB schematic (EAGLE)                            |
| `README.md`              | This file 📄                                     |

---

## ⚙️ How It Works

1. Sensors collect pressure and temperature data from three duct zones.
2. ESP32 processes this data and communicates with a web dashboard over Wi-Fi.
3. Based on detected anomalies (e.g., pressure drop), the system adjusts fans and damper positions to maintain balance.
4. The user can monitor and override controls via the web interface.
5. Updates can be deployed remotely via OTA.

---


## 🚀 Requirements

- ESP32 Dev Module
- TMP102 Temperature Sensor(s)
- LPS25HB Pressure Sensor(s)
- TB6612FNG Dual H-Bridge Motor Driver
- Servo Motor (for damper control)
- 3D-printed or physical duct model (optional)
- Tornado (Python backend)
- EAGLE (for PCB design)
