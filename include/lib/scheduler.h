#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include "include/sensors/sgp30_sensor.h"
#include "include/sensors/scd41_sensor.h"
#include "include/sensors/pms7003_sensor.h"
#include "include/lib/mqtt_client.h"
#include "include/lib/oled_display.h"
#include "include/lib/wifi_manager.h"

#define OLED_TIMEOUT 300000  // 5 minutes timeout in milliseconds
#define BOOT_BUTTON_PIN 0    // ESP32 Boot Button (GPIO 0)
#define MQTT_RETRY_INTERVAL 5000      // 5 seconds between immediate retries
#define MQTT_MAX_RETRIES 3            // Maximum number of immediate retries
#define MQTT_LONG_RETRY_INTERVAL 900000 // 15 minutes between long retry attempts
#define WIFI_CONNECT_TIMEOUT 15000    // 15 seconds timeout for WiFi connection

void IRAM_ATTR handleButtonPress();

class Scheduler {
private:
    static float temperatureF, humidity, tvoc, h2, ethanol;
    static int co2, pm1_0, pm2_5, pm10, aqi;
    static unsigned long lastOLEDUpdate, lastSerialUpdate, lastMQTTUpdate, oledTimer;
    static bool oledOn;
    static volatile bool oledToggleRequested;
    static bool mqttEnabled;
    static unsigned long lastConnectionAttempt;
    static int connectionRetryCount;
    static bool wifiConnected;

    static int calculateAQI(int pm2_5, int pm10);
    static void updateSensors();
    static void attemptConnection();

public:
    static void init();
    static void run();
    static void setOledToggleRequested();
    static bool isOledOn();
};

// Define isOledOn() function for external use
inline bool isOledOn() {
    return Scheduler::isOledOn();
}

#endif // SCHEDULER_H
