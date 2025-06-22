Modular Home Automation Sensor Hub with ESP32 & REST API

## Overview

This project demonstrates a scalable and modular IoT-based Home Automation Sensor Hub built using ESP32 microcontrollers. 
It leverages RESTful API architecture to simulate and serve real-time data for multiple sensor types (temperature, humidity, vibration) over HTTP, enabling seamless integration with clients like **Node-RED** running on **Raspberry Pi**.

This solution is ideal for building smart home prototypes, sensor networks, or academic IoT experiments.

## Objectives

- Develop a modular ESP32 firmware that can expose simulated sensor data via REST API.
- Design a system that supports multiple sensor types and logical port configurations.
- Integrate with **Node-RED** for visualizing and managing the data through a dashboard.
- Ensure the architecture is scalable and easy to maintain or extend.

## Features

-  REST API server running on ESP32
-  Simulated multi-sensor data (Temperature, Humidity, Vibration)
-  Sensor type and port selection through URL query parameters
-  JSON-formatted API responses
-  Node-RED integration for visualization on Raspberry Pi
- 🛠 Extensible code structure for future hardware integration

## Sample REST API

```
GET http://<esp32-ip>/sensor?type=temperature&port=1
```
### Sample Response:

```json
{
  "device": "ESP32",
  "mac_address": "24:6F:28:AA:BB:CC",
  "sensor": "temperature",
  "port": 1,
  "assigned_sensor": "Living Room Temp",
  "status": "OK",
  "value": 25.78,
  "unit": "°C"
}
```
## Bill of Materials (BOM)

| Component                  | Quantity | Description                                     |
|---------------------------|----------|-------------------------------------------------|
| ESP32 Development Board   | 2        | WiFi-enabled MCU for REST API simulation        |
| Raspberry Pi 3B+ or later | 1        | Node-RED client and dashboard server            |
| Micro USB Cable           | 2        | Power and serial communication                  |
| 5V Power Adapter (for Pi) | 1        | Minimum 3A recommended                          |
| Router / Access Point     | 1        | All devices must be connected to same network   |
| Breadboard + Jumpers      | Optional | For future sensor hardware integration          |

## Installation & Setup

### ESP32 Firmware
- Use Arduino IDE or PlatformIO
- Install dependencies:
  - `WiFi.h`
  - `WebServer.h`
  - `ArduinoJson.h`
- Flash the firmware from `firmware/main.cpp`
- Configure your Wi-Fi SSID/password

### Node-RED (Raspberry Pi)
- Install Node-RED:
  [https://nodered.org/docs/getting-started/raspberrypi](https://nodered.org/docs/getting-started/raspberrypi)
- Import flow from `node-red/flows.json`
- Customize API URLs to match your ESP32 IPs
- Deploy dashboard

- ## Architecture Diagram

```
ESP32 (Simulated Sensors)
     |
     |  REST API (GET /sensor?type=...&port=...)
     |
Raspberry Pi (Node-RED Dashboard)
```

## Extensibility

You can:
- Add more sensor types (e.g., light, gas)
- Connect real sensors to ESP32 GPIO/I2C/SPI
- Expand the dashboard for alerts and automation
- Integrate MQTT alongside REST

## Future Roadmap

- [ ] Real sensor integration via GPIO
- [ ] Add authentication to API
- [ ] MQTT publishing support
- [ ] Remote OTA firmware updates
- [ ] Auto-discovery of sensors

## Useful Links

- [ESP32 HTTP Server](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html)
- [ArduinoJson Library](https://arduinojson.org/)
- [Node-RED Docs](https://nodered.org/docs/)
- [ESP32 on Arduino IDE](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide/)
