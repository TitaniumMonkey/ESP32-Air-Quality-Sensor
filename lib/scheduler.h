#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include "sensors/bme680_sensor.h"
#include "sensors/scd41_sensor.h"
#include "sensors/pms7003_sensor.h"
#include "lib/mqtt_client.h"
#include "lib/oled_display.h"
#include "lib/enhanced_aqi.h"
#include "lib/wifi_manager.h"

#define OLED_TIMEOUT 300000  // 5 minutes timeout in milliseconds
#define BOOT_BUTTON_PIN 0    // ESP32 Boot Button (GPIO 0)
#define MQTT_RETRY_INTERVAL 5000      // 5 seconds between immediate retries
#define MQTT_MAX_RETRIES 3            // Maximum number of immediate retries
#define MQTT_LONG_RETRY_INTERVAL 900000 // 15 minutes between long retry attempts
#define WIFI_CONNECT_TIMEOUT 15000    // 15 seconds timeout for WiFi connection

void IRAM_ATTR handleButtonPress(); // Declare ISR function before use

class Scheduler {
private:
    static float temperatureF, humidity, pressure, gas_resistance;
    static int co2, pm1_0, pm2_5, pm10, aqi, enhanced_aqi;
    static unsigned long lastOLEDUpdate, lastSerialUpdate, lastMQTTUpdate, oledTimer;
    static bool oledOn;
    static volatile bool oledToggleRequested;
    static bool mqttEnabled;
    static unsigned long lastConnectionAttempt;
    static int connectionRetryCount;
    static bool wifiConnected;

    static int calculateAQI(int pm2_5, int pm10) {
        const int pm25Breakpoints[] = {0, 12, 35, 55, 150, 250, 500};
        const int pm25AQIValues[]   = {0, 50, 100, 150, 200, 300, 500};

        const int pm10Breakpoints[] = {0, 54, 154, 254, 354, 424, 604};
        const int pm10AQIValues[]   = {0, 50, 100, 150, 200, 300, 500};

        auto calculatePollutantAQI = [](int concentration, const int breakpoints[], const int aqiValues[]) {
            for (int i = 1; i < 7; i++) {
                if (concentration <= breakpoints[i]) {
                    return ((aqiValues[i] - aqiValues[i - 1]) * (concentration - breakpoints[i - 1])) /
                           (breakpoints[i] - breakpoints[i - 1]) + aqiValues[i - 1];
                }
            }
            return 500;
        };

        int aqi_pm2_5 = calculatePollutantAQI(pm2_5, pm25Breakpoints, pm25AQIValues);
        int aqi_pm10  = calculatePollutantAQI(pm10, pm10Breakpoints, pm10AQIValues);

        return max(aqi_pm2_5, aqi_pm10);
    }

public:
    static void init() {
        Serial.println("Scheduler initialized");
        
        // Start core functionality first
        BME680Sensor::begin();
        SCD41Sensor::begin();
        PMS7003Sensor::begin();
        OLEDDisplay::init();
        
        oledTimer = millis();
        oledOn = true;
        
        pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(BOOT_BUTTON_PIN), handleButtonPress, FALLING);

        lastOLEDUpdate = lastSerialUpdate = lastMQTTUpdate = millis();
        lastConnectionAttempt = 0;
        connectionRetryCount = 0;
        wifiConnected = false;
        mqttEnabled = false;

        // Try initial connection
        attemptConnection();
    }

    static void attemptConnection() {
        if (!wifiConnected) {
            Serial.println("Attempting WiFi connection...");
            if (WiFiManager::connect(WIFI_CONNECT_TIMEOUT)) {
                wifiConnected = true;
                Serial.println("WiFi connected successfully");
                // Try MQTT after successful WiFi connection
                if (MQTTClient::init()) {
                    mqttEnabled = true;
                    Serial.println("MQTT connected successfully");
                    connectionRetryCount = 0;
                } else {
                    mqttEnabled = false;
                    connectionRetryCount++;
                    Serial.println("MQTT connection failed");
                }
            } else {
                wifiConnected = false;
                connectionRetryCount++;
                Serial.println("WiFi connection failed");
            }
        } else if (!mqttEnabled) {
            // Try MQTT if WiFi is connected but MQTT isn't
            if (MQTTClient::init()) {
                mqttEnabled = true;
                Serial.println("MQTT connected successfully");
                connectionRetryCount = 0;
            } else {
                mqttEnabled = false;
                connectionRetryCount++;
                Serial.println("MQTT connection failed");
            }
        }

        // If we've failed too many times, wait for the long retry interval
        if (connectionRetryCount >= MQTT_MAX_RETRIES) {
            Serial.println("Maximum connection attempts reached. Will retry in 15 minutes.");
            lastConnectionAttempt = millis();
            connectionRetryCount = 0;
        }
    }

    static void setOledToggleRequested() {
        oledToggleRequested = true;
    }

    static void updateSensors() {
        BME680Sensor::read();
        PMS7003Sensor::read();

        temperatureF = BME680Sensor::getTemperatureF();
        humidity = BME680Sensor::getHumidity();
        pressure = BME680Sensor::getPressure();
        gas_resistance = BME680Sensor::getGasResistance();

        if (SCD41Sensor::read()) {
            co2 = SCD41Sensor::getCO2();
        }

        pm1_0 = PMS7003Sensor::getPM1_0();
        pm2_5 = PMS7003Sensor::getPM2_5();
        pm10 = PMS7003Sensor::getPM10();
        aqi = calculateAQI(pm2_5, pm10);
        enhanced_aqi = getEnhancedAQI();
    }

    static void run() {
        unsigned long now = millis();

        // Handle connection retries
        if (!mqttEnabled && (now - lastConnectionAttempt >= MQTT_LONG_RETRY_INTERVAL)) {
            Serial.println("Attempting periodic reconnection...");
            lastConnectionAttempt = now;
            attemptConnection();
        }

        // Handle MQTT client loop if connected
        if (mqttEnabled && wifiConnected) {
            MQTTClient::loop();
        }

        if (oledToggleRequested) {
            oledToggleRequested = false;
            oledOn = !oledOn;
            if (oledOn) {
                oledTimer = now;
                Serial.println("OLED turned ON.");
            } else {
                OLEDDisplay::init();
                Serial.println("OLED turned OFF.");
            }
        }

        if (oledOn && now - oledTimer >= OLED_TIMEOUT) {
            oledOn = false;
            OLEDDisplay::init();
            Serial.println("OLED Auto Shutoff.");
        }

        if (oledOn && now - lastOLEDUpdate >= 1000) {
            lastOLEDUpdate = now;
            updateSensors();
            OLEDDisplay::update(temperatureF, humidity, pressure, co2, pm1_0, pm2_5, pm10, aqi, enhanced_aqi, gas_resistance);
        }
		
		if (now - lastSerialUpdate >= 10000) {
            lastSerialUpdate = now;
            updateSensors();
            Serial.print("Temp: "); Serial.print(temperatureF, 2); Serial.print(" °F, ");
            Serial.print("Humidity: "); Serial.print(humidity, 2); Serial.print(" %, ");
            Serial.print("Pressure: "); Serial.print(pressure, 2); Serial.print(" hPa, ");
            Serial.print("CO2: "); Serial.print(co2); Serial.print(" ppm, ");
            Serial.print("PM1.0: "); Serial.print(pm1_0); Serial.print(" µg/m³, ");
            Serial.print("PM2.5: "); Serial.print(pm2_5); Serial.print(" µg/m³, ");
            Serial.print("PM10: "); Serial.print(pm10); Serial.print(" µg/m³, ");
            Serial.print("AQI: "); Serial.print(aqi);
            Serial.print(" | Enhanced AQI: "); Serial.println(enhanced_aqi);
        }

        // Only attempt MQTT updates if MQTT is enabled and connected
        if (mqttEnabled && wifiConnected && now - lastMQTTUpdate >= 60000) {
            if (!MQTTClient::isConnected()) {
                mqttEnabled = false;
                Serial.println("MQTT connection lost - will retry later");
            } else {
                lastMQTTUpdate = now;
                updateSensors();
                MQTTClient::publish("homeassistant/sensor/esp32_temperature/state", "{ \"temperature\": " + String(temperatureF) + " }");
                MQTTClient::publish("homeassistant/sensor/esp32_humidity/state", "{ \"humidity\": " + String(humidity) + " }");
                MQTTClient::publish("homeassistant/sensor/esp32_pressure/state", "{ \"pressure\": " + String(pressure) + " }");
                MQTTClient::publish("homeassistant/sensor/esp32_co2/state", String(co2));
                MQTTClient::publish("homeassistant/sensor/esp32_pm1_0/state", String(pm1_0));
                MQTTClient::publish("homeassistant/sensor/esp32_pm2_5/state", String(pm2_5));
                MQTTClient::publish("homeassistant/sensor/esp32_pm10/state", String(pm10));
                MQTTClient::publish("homeassistant/sensor/esp32_aqi/state", String(aqi));
            }
        }
    }

    static bool isOledOn() {
        return oledOn;
    }
};

// Define static member variables
float Scheduler::temperatureF = 0;
float Scheduler::humidity = 0;
float Scheduler::pressure = 0;
float Scheduler::gas_resistance = 0;
int Scheduler::co2 = 0;
int Scheduler::pm1_0 = 0;
int Scheduler::pm2_5 = 0;
int Scheduler::pm10 = 0;
int Scheduler::aqi = 0;
int Scheduler::enhanced_aqi = 0;
bool Scheduler::oledOn = true;
volatile bool Scheduler::oledToggleRequested = false;
unsigned long Scheduler::lastOLEDUpdate = 0;
unsigned long Scheduler::lastSerialUpdate = 0;
unsigned long Scheduler::lastMQTTUpdate = 0;
unsigned long Scheduler::oledTimer = 0;
bool Scheduler::mqttEnabled = false;
unsigned long Scheduler::lastConnectionAttempt = 0;
int Scheduler::connectionRetryCount = 0;
bool Scheduler::wifiConnected = false;

// Interrupt Service Routine (ISR) for button press
void IRAM_ATTR handleButtonPress() {
    Scheduler::setOledToggleRequested();
}

// Define isOledOn() function properly
inline bool isOledOn() {
    return Scheduler::isOledOn();
}

#endif // SCHEDULER_H
