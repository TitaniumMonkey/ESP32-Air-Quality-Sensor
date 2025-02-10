#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include "sensors/bme680_sensor.h"
#include "sensors/scd41_sensor.h"
#include "sensors/pms7003_sensor.h"
#include "lib/mqtt_client.h"
#include "lib/oled_display.h"
#include "lib/enhanced_aqi.h"

class Scheduler {
private:
    static float temperatureF, humidity, pressure;
    static int co2, pm1_0, pm2_5, pm10, aqi, enhanced_aqi;
    static unsigned long lastOLEDUpdate, lastSerialUpdate, lastMQTTUpdate;

    // Function to calculate AQI based on EPA standards
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
        BME680Sensor::begin();
        SCD41Sensor::begin();
        PMS7003Sensor::begin();
        MQTTClient::init();
        if (lastOLEDUpdate == 0) OLEDDisplay::init();
        lastOLEDUpdate = lastSerialUpdate = lastMQTTUpdate = millis();
    }

    static void updateSensors() {
        BME680Sensor::read();
        PMS7003Sensor::read();

        temperatureF = BME680Sensor::getTemperatureF();
        humidity = BME680Sensor::getHumidity();
        pressure = BME680Sensor::getPressure();

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

        if (now - lastOLEDUpdate >= 1000) {
            lastOLEDUpdate = now;
            updateSensors();
            OLEDDisplay::update(temperatureF, humidity, pressure, co2, pm1_0, pm2_5, pm10, aqi, enhanced_aqi);
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

        if (now - lastMQTTUpdate >= 60000) {
            lastMQTTUpdate = now;
            updateSensors();
            MQTTClient::publish("homeassistant/sensor/esp32_temperature/state", "{ \"temperature\": " + String(temperatureF) + " }");
            MQTTClient::publish("homeassistant/sensor/esp32_humidity/state", "{ \"humidity\": " + String(humidity) + " }");
            MQTTClient::publish("homeassistant/sensor/esp32_pressure/state", "{ \"pressure\": " + String(pressure) + " }");
            MQTTClient::publish("homeassistant/sensor/esp32_co2/state", String(co2));
            MQTTClient::publish("homeassistant/sensor/esp32_pm1_0/state", String(pm1_0));
            MQTTClient::publish("homeassistant/sensor/esp32_pm2_5/state", String(pm2_5));
            MQTTClient::publish("homeassistant/sensor/esp32_pm10/state", String(pm10));
            MQTTClient::publish("homeassistant/sensor/esp32_aqi/state", String(enhanced_aqi));
        }
    }
};

float Scheduler::temperatureF = 0;
float Scheduler::humidity = 0;
float Scheduler::pressure = 0;
int Scheduler::co2 = 0;
int Scheduler::pm1_0 = 0;
int Scheduler::pm2_5 = 0;
int Scheduler::pm10 = 0;
int Scheduler::aqi = 0;
int Scheduler::enhanced_aqi = 0;
unsigned long Scheduler::lastOLEDUpdate = 0;
unsigned long Scheduler::lastSerialUpdate = 0;
unsigned long Scheduler::lastMQTTUpdate = 0;

#endif // SCHEDULER_H
