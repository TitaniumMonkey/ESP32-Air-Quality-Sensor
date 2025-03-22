#include "include/lib/scheduler.h"

// Define static member variables
float Scheduler::temperatureF = 0;
float Scheduler::humidity = 0;
float Scheduler::tvoc = 0;
float Scheduler::h2 = 0;
float Scheduler::ethanol = 0;
int Scheduler::co2 = 0;
int Scheduler::pm1_0 = 0;
int Scheduler::pm2_5 = 0;
int Scheduler::pm10 = 0;
int Scheduler::aqi = 0;
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
unsigned long Scheduler::lastSuccessfulRead = 0;
unsigned long Scheduler::lastReboot = 0;

int Scheduler::calculateAQI(int pm2_5, int pm10) {
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

void Scheduler::init() {
    Serial.println("Scheduler initialized");
    
    // Start core functionality first
    SGP30Sensor::begin();
    SCD41Sensor::begin();
    PMS7003Sensor::begin();
    OLEDDisplay::init();
    
    oledTimer = millis();
    oledOn = true;
    lastReboot = millis();
    lastSuccessfulRead = millis();
    
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

void Scheduler::attemptConnection() {
    if (!wifiConnected) {
        Serial.println("Attempting WiFi connection...");
        if (WiFiManager::connect(WIFI_CONNECT_TIMEOUT)) {
            wifiConnected = true;
            Serial.println("WiFi connected successfully");
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

    if (connectionRetryCount >= MQTT_MAX_RETRIES) {
        Serial.println("Maximum connection attempts reached. Will retry in 15 minutes.");
        lastConnectionAttempt = millis();
        connectionRetryCount = 0;
    }
}

void Scheduler::updateSensors() {
    bool allSensorsWorking = true;
    
    // Update SCD41 sensor
    if (SCD41Sensor::read()) {
        temperatureF = SCD41Sensor::getTemperatureF();
        humidity = SCD41Sensor::getHumidity();
        co2 = SCD41Sensor::getCO2();
        lastSuccessfulRead = millis();
    } else {
        allSensorsWorking = false;
    }
    
    // Update SGP30 sensor
    SGP30Sensor::read();
    tvoc = SGP30Sensor::getTVOC();
    h2 = SGP30Sensor::getH2();
    ethanol = SGP30Sensor::getEthanol();
    
    // Update PMS7003 sensor
    if (PMS7003Sensor::read()) {
        pm1_0 = PMS7003Sensor::getPM1_0();
        pm2_5 = PMS7003Sensor::getPM2_5();
        pm10 = PMS7003Sensor::getPM10();
        aqi = calculateAQI(pm2_5, pm10);
        lastSuccessfulRead = millis();
    } else {
        allSensorsWorking = false;
    }
    
    // If all sensors are working, update the last successful read time
    if (allSensorsWorking) {
        lastSuccessfulRead = millis();
    }
}

void Scheduler::run() {
    // Check for reboot conditions
    checkAndReboot();
    
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

    if (oledOn && now - lastOLEDUpdate >= 500) {
        lastOLEDUpdate = now;
        updateSensors();
        OLEDDisplay::update(temperatureF, humidity, co2, pm1_0, pm2_5, pm10, aqi, tvoc, h2, ethanol);
    }
    
    if (now - lastSerialUpdate >= 10000) {
        lastSerialUpdate = now;
        updateSensors();
        Serial.print("Temp: "); Serial.print(temperatureF, 2); Serial.print(" °F | ");
        Serial.print("Humidity: "); Serial.print(humidity, 2); Serial.print(" % | ");
        Serial.print("CO2: "); Serial.print(co2); Serial.print(" ppm | ");
        Serial.print("PM1.0: "); Serial.print(pm1_0); Serial.print(" µg/m³ | ");
        Serial.print("PM2.5: "); Serial.print(pm2_5); Serial.print(" µg/m³ | ");
        Serial.print("PM10: "); Serial.print(pm10); Serial.print(" µg/m³ | ");
        Serial.print("AQI: "); Serial.print(aqi);
        Serial.print(" | TVOC: "); Serial.print(tvoc, 0); Serial.print(" ppb");
        Serial.print(" | H2: "); Serial.print(h2, 0); Serial.print(" res");
        Serial.print(" | Ethanol: "); Serial.print(ethanol, 0); Serial.println(" res");
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
            MQTTClient::publish("homeassistant/sensor/esp32_co2/state", String(co2));
            MQTTClient::publish("homeassistant/sensor/esp32_pm1_0/state", String(pm1_0));
            MQTTClient::publish("homeassistant/sensor/esp32_pm2_5/state", String(pm2_5));
            MQTTClient::publish("homeassistant/sensor/esp32_pm10/state", String(pm10));
            MQTTClient::publish("homeassistant/sensor/esp32_aqi/state", String(aqi));
            MQTTClient::publish("homeassistant/sensor/esp32_tvoc/state", String(tvoc));
            MQTTClient::publish("homeassistant/sensor/esp32_h2/state", String(h2));
            MQTTClient::publish("homeassistant/sensor/esp32_ethanol/state", String(ethanol));
        }
    }
}

void Scheduler::setOledToggleRequested() {
    oledToggleRequested = true;
}

bool Scheduler::isOledOn() {
    return oledOn;
}

// Interrupt Service Routine (ISR) for button press
void IRAM_ATTR handleButtonPress() {
    Scheduler::setOledToggleRequested();
}

void Scheduler::checkAndReboot() {
    unsigned long currentTime = millis();
    
    // Check for scheduled reboot (every 6 hours)
    if (currentTime - lastReboot >= SCHEDULED_REBOOT_INTERVAL) {
        Serial.println("Performing scheduled reboot...");
        performReboot();
        return;
    }
    
    // Check for emergency reboot (no readings for 5 minutes)
    if (currentTime - lastSuccessfulRead >= EMERGENCY_REBOOT_TIMEOUT) {
        Serial.println("No sensor readings for 5 minutes, performing emergency reboot...");
        performReboot();
        return;
    }
}

void Scheduler::performReboot() {
    // Try to send a message to MQTT before rebooting
    if (mqttEnabled) {
        MQTTClient::publish("homeassistant/sensor/esp32_status/state", "rebooting");
        delay(1000);  // Give time for the message to be sent
    }
    
    // Clean up
    if (mqttEnabled) {
        MQTTClient::disconnect();
    }
    
    // Display reboot message on OLED if it's on
    if (oledOn) {
        OLEDDisplay::update(temperatureF, humidity, co2, pm1_0, pm2_5, pm10, aqi, tvoc, h2, ethanol);
        delay(1000);
    }
    
    // Perform the reboot
    ESP.restart();
} 
