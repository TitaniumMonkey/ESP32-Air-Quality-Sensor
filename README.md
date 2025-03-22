# ESP32 Air Quality Sensor with OLED Display, MQTT & Enhanced AQI

<p align="center">
  <img src="https://i.imgur.com/nQ48kV8.png">
  <img src="https://i.imgur.com/MAuDw7c.png" width="501" height="793">
</p>

## Overview
This project is an **ESP32-based Air Quality Monitoring System** that measures and reports environmental data, including:
- **Temperature & Humidity** (GY-SGP30)
- **CO₂ Levels** (SCD41)
- **Particulate Matter (PM1.0, PM2.5, PM10)** (PMS7003)
- **Air Quality Index (AQI)** (Calculated from PM data)
- **TVOC, Hydrogen, Ethenol** (GY-SGP30)

The collected data is displayed on an **SSD1306 OLED screen** and published to an **MQTT broker** every **1 minute** for integration with platforms like **Home Assistant**.

## Features

### Sensor Capabilities
- Temperature and Humidity monitoring (GY-SGP30)
- CO2 level monitoring (SCD41)
- Particulate Matter monitoring (PMS7003)
- PM1.0, PM2.5, and PM10 measurements
- Total volatile organic compounds(TVOC) including Hydrogen and Ethenol (GY-SGP30)
- Standard AQI calculation

### Display and Controls
- OLED Display showing real-time sensor readings
- Auto display shutdown after 5 minutes to prevent burn-in
- Boot button (GPIO 0) functions as OLED toggle switch
- Display automatically reactivates when new button press detected

### Network Features
- Graceful offline operation when WiFi is unavailable
- Automatic WiFi reconnection attempts
- MQTT integration with Home Assistant when network available
- Smart connection management:
  - Initial connection attempts with 15-second timeout
  - After 3 failed attempts, retries every 15 minutes
  - Automatic recovery when network becomes available
  - No impact on sensor operation during network outages

### Data Reporting
- Local display via OLED
- Serial output for debugging
- MQTT publishing to Home Assistant when connected
- Automatic sensor discovery in Home Assistant


## Project Structure

```
📁 **ESP32_Air_Quality_Sensor**
├── 📄 `sketch.ino`               # Main entry point
├── 📄 `README.md`                # Project documentation
├── 📄 `secrets.h`                # Wi-Fi & MQTT credentials (template included but must be updated)
├── 📄 `LICENSE`                  # License file
├── 📁 `include`                  # Header files (.h)
│   ├── 📁 `lib`                  # Library component headers
│   │   ├── 📄 `mqtt_client.h`    # MQTT connection management
│   │   ├── 📄 `oled_display.h`   # OLED display control
│   │   ├── 📄 `scheduler.h`      # Task scheduling
│   │   ├── 📄 `enhanced_aqi.h`   # Enhanced AQI calculation
│   │   └── 📄 `wifi_manager.h`   # Manages Wi-Fi connection
│   └── 📁 `sensors`              # Sensor headers
│       ├── 📄 `sgp30_sensor.h`   # SGP30 sensor interface
│       ├── 📄 `scd41_sensor.h`   # SCD41 sensor interface
│       └── 📄 `pms7003_sensor.h` # PMS7003 sensor interface
└── 📁 `src`                      # Implementation files (.cpp)
    ├── 📁 `lib`                  # Library component implementations
    │   ├── 📄 `mqtt_client.cpp`  # MQTT connection implementation
    │   ├── 📄 `oled_display.cpp` # OLED display implementation
    │   ├── 📄 `scheduler.cpp`    # Task scheduling implementation
    │   ├── 📄 `enhanced_aqi.cpp` # Enhanced AQI implementation
    │   └── 📄 `wifi_manager.cpp` # Wi-Fi management implementation
    └── 📁 `sensors`              # Sensor implementations
        ├── 📄 `sgp30_sensor.cpp` # SGP30 sensor implementation
        ├── 📄 `scd41_sensor.cpp` # SCD41 sensor implementation
        └── 📄 `pms7003_sensor.cpp` # PMS7003 sensor implementation
```

## Components Used
| Component             | Description                    |
|----------------------|--------------------------------|
| **ESP32 Dev Board**  | Wi-Fi-enabled microcontroller |
| **SGP30 Sensor**    | Measures temperature, humidity, TVOC, H2 & Ethenol |
| **SCD41 Sensor**     | Measures CO₂ concentration |
| **PMS7003 Sensor**   | Measures particulate matter (PM1.0, PM2.5, PM10) |
| **SSD1306 OLED**     | 0.96-inch I2C OLED screen |
| **MQTT Broker**      | Receives and processes sensor data |

## Required Libraries
Install the following libraries in **Arduino IDE**:
1. **WiFi** (Built-in for ESP32)
2. **PubSubClient** (by Nick O'Leary)
3. **Adafruit SGP30 Sensor** (by Adafruit)
4. **Adafruit GFX Library** (by Adafruit)
5. **Adafruit SSD1306** (by Adafruit)
6. **SparkFun SCD4x Arduino Library** (for SCD41 sensor)
7. **PMS Library** (for PMS7003 sensor)

### Installing Libraries
1. Open **Arduino IDE**
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for and install the required libraries

## Board Setup in Arduino IDE
1. **Install ESP32 Board Support**
   - Open **Arduino IDE**
   - Go to **File → Preferences**
   - In **Additional Board Manager URLs**, add:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Boards Manager**
   - Search for **ESP32** and install the latest version

2. **Select the Board**
   - Go to **Tools → Board**
   - Select **ESP32 Dev Module**
   - Set Upload Speed: **230400**
   - Flash Mode: **QIO**
   - Partition Scheme: **Default**

## Wiring Diagram
| ESP32 Pin | Sensor | Description |
|-----------|--------|-------------|
| 3.3V      | All Sensors | Power Supply |
| GND       | All Sensors | Ground |
| GPIO21    | SDA (OLED, SGP30, SCD41) | I2C Data |
| GPIO22    | SCL (OLED, SGP30, SCD41) | I2C Clock |
| GPIO14    | RX (PMS7003) | UART Receive |
| GPIO27    | TX (PMS7003) | UART Transmit |

## Configuration
### Setting Your Timezone
This project uses **NTP (Network Time Protocol)** to sync the correct time. Adjust the timezone in `wifi_manager.h`:
```cpp
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -8 * 3600; // Pacific Standard Time (PST)
const int daylightOffset_sec = 0;     // No DST (Set to 3600 for 1 hour DST adjustment)
```

## Storing WiFi & MQTT Credentials
Before uploading the code, create a `secrets.h` file next to your `.ino` file with your **Wi-Fi and MQTT credentials**:
```cpp
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "Your_WiFi_SSID"
#define WIFI_PASSWORD "Your_WiFi_Password"

#define MQTT_SERVER "Your_MQTT_Broker_IP"
#define MQTT_USER "Your_MQTT_Username"
#define MQTT_PASS "Your_MQTT_Password"

#endif // SECRETS_H
```

## Uploading the Code
1. **Connect the ESP32 to your PC via USB**
2. Open the **Arduino IDE**
3. Select **ESP32 Dev Module** under **Tools → Board**
4. Open **Tools → Port** and select the correct COM port
5. Click the **Upload** button
6. Open the **Serial Monitor** (baud rate: `115200`) to check the logs

## Expected OLED Display Layout
```
Temp:  70.97 F
Humidity:  34.05 %
CO2:  674 ppm
PM:  81 / 121 / 126 ug/m3
AQI:  184
TVOC: 30 ppb
H2: 10000 ppb
Ethanol: 13000 ppb
```

## Troubleshooting
### 1. **OLED Display Not Working**
- Ensure **SDA (GPIO21) & SCL (GPIO22)** are correctly connected.
- Check **I2C address** (default is `0x3C`).
- Try scanning I2C devices using an I2C scanner sketch.

### 2. **ESP32 Not Connecting to Wi-Fi**
- Double-check **SSID and Password** in `secrets.h`.
- Ensure your **Wi-Fi network is 2.4 GHz** (ESP32 doesn't support 5 GHz).

### 3. **MQTT Not Connecting**
- Verify the **MQTT broker IP and credentials** in `secrets.h`.
- Ensure the broker is running and accessible.
- Try disabling MQTT authentication temporarily.

## Future Plans.
1. **Additional Sensors** → Support for **VOC, NO₂, and other air quality sensors**.
2. **LoRa & Meshtastic Integration** → Use **LoRaWAN** or **Meshtastic** to transmit sensor data over long distances. 

---
### 🚀 Enjoy your ESP32 Air Quality Sensor!
Let me know if you have any issues. Feel free to contribute and improve this project! 😊
