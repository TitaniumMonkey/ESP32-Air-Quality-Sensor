#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "secrets.h"

#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESP32_AirQuality"

class MQTTClient {
private:
    static WiFiClient espClient;
    static PubSubClient client;
    static bool initialized;

public:
    static bool init() {
        if (!WiFi.isConnected()) {
            Serial.println("Cannot initialize MQTT - WiFi not connected");
            return false;
        }

        if (!initialized) {
            client.setClient(espClient);
            client.setServer(MQTT_SERVER, MQTT_PORT);
            initialized = true;
        }

        if (client.connected()) {
            return true;
        }

        Serial.print("Connecting to MQTT...");
        
        if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
            Serial.println("connected");
            // Register MQTT topics with correct state_topic
            client.publish("homeassistant/sensor/esp32_temperature/config", 
                "{\"name\":\"ESP32 Temperature\",\"state_topic\":\"homeassistant/sensor/esp32_temperature/state\",\"unit_of_measurement\":\"°F\",\"device_class\":\"temperature\"}");
            client.publish("homeassistant/sensor/esp32_humidity/config", 
                "{\"name\":\"ESP32 Humidity\",\"state_topic\":\"homeassistant/sensor/esp32_humidity/state\",\"unit_of_measurement\":\"%\",\"device_class\":\"humidity\"}");
            client.publish("homeassistant/sensor/esp32_pressure/config", 
                "{\"name\":\"ESP32 Pressure\",\"state_topic\":\"homeassistant/sensor/esp32_pressure/state\",\"unit_of_measurement\":\"hPa\",\"device_class\":\"pressure\"}");
            client.publish("homeassistant/sensor/esp32_co2/config", 
                "{\"name\":\"ESP32 CO2\",\"state_topic\":\"homeassistant/sensor/esp32_co2/state\",\"unit_of_measurement\":\"ppm\",\"device_class\":\"carbon_dioxide\"}");
            client.publish("homeassistant/sensor/esp32_pm1_0/config", 
                "{\"name\":\"ESP32 PM1.0\",\"state_topic\":\"homeassistant/sensor/esp32_pm1_0/state\",\"unit_of_measurement\":\"µg/m³\"}");
            client.publish("homeassistant/sensor/esp32_pm2_5/config", 
                "{\"name\":\"ESP32 PM2.5\",\"state_topic\":\"homeassistant/sensor/esp32_pm2_5/state\",\"unit_of_measurement\":\"µg/m³\"}");
            client.publish("homeassistant/sensor/esp32_pm10/config", 
                "{\"name\":\"ESP32 PM10\",\"state_topic\":\"homeassistant/sensor/esp32_pm10/state\",\"unit_of_measurement\":\"µg/m³\"}");
            client.publish("homeassistant/sensor/esp32_aqi/config", 
                "{\"name\":\"ESP32 AQI\",\"state_topic\":\"homeassistant/sensor/esp32_aqi/state\",\"unit_of_measurement\":\"AQI\"}");
            return true;
        } else {
            Serial.print("failed, rc=");
            Serial.println(client.state());
            return false;
        }
    }

    static bool isConnected() {
        return client.connected();
    }

    static bool publish(const char* topic, const String& payload) {
        if (!client.connected()) {
            return false;
        }
        return client.publish(topic, payload.c_str());
    }

    static void disconnect() {
        if (client.connected()) {
            client.disconnect();
        }
    }

    static void loop() {
        if (client.connected()) {
            client.loop();
        }
    }
};

// Initialize static members
WiFiClient MQTTClient::espClient;
PubSubClient MQTTClient::client(MQTTClient::espClient);
bool MQTTClient::initialized = false;

#endif // MQTT_CLIENT_H
