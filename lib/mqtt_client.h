#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

class MQTTClient {
private:
    static WiFiClient espClient;
    static PubSubClient client;

public:
    static void init() {
        client.setClient(espClient);
        client.setServer(MQTT_SERVER, 1883);
        connect();
    }

    static void connect() {
        while (!client.connected()) {
            Serial.print("Connecting to MQTT...");
            if (client.connect("ESP32Client", MQTT_USER, MQTT_PASS)) {
                Serial.println("Connected!");
                // Register MQTT topics with correct state_topic
                client.publish("homeassistant/sensor/esp32_temperature/config", "{\"name\":\"ESP32 Temperature\",\"state_topic\":\"homeassistant/sensor/esp32_temperature/state\",\"unit_of_measurement\":\"°F\",\"device_class\":\"temperature\"}");
                client.publish("homeassistant/sensor/esp32_humidity/config", "{\"name\":\"ESP32 Humidity\",\"state_topic\":\"homeassistant/sensor/esp32_humidity/state\",\"unit_of_measurement\":\"%\",\"device_class\":\"humidity\"}");
                client.publish("homeassistant/sensor/esp32_pressure/config", "{\"name\":\"ESP32 Pressure\",\"state_topic\":\"homeassistant/sensor/esp32_pressure/state\",\"unit_of_measurement\":\"hPa\",\"device_class\":\"pressure\"}");
                client.publish("homeassistant/sensor/esp32_co2/config", "{\"name\":\"ESP32 CO2\",\"state_topic\":\"homeassistant/sensor/esp32_co2/state\",\"unit_of_measurement\":\"ppm\",\"device_class\":\"carbon_dioxide\"}");
                client.publish("homeassistant/sensor/esp32_pm1_0/config", "{\"name\":\"ESP32 PM1.0\",\"state_topic\":\"homeassistant/sensor/esp32_pm1_0/state\",\"unit_of_measurement\":\"µg/m³\"}");
                client.publish("homeassistant/sensor/esp32_pm2_5/config", "{\"name\":\"ESP32 PM2.5\",\"state_topic\":\"homeassistant/sensor/esp32_pm2_5/state\",\"unit_of_measurement\":\"µg/m³\"}");
                client.publish("homeassistant/sensor/esp32_pm10/config", "{\"name\":\"ESP32 PM10\",\"state_topic\":\"homeassistant/sensor/esp32_pm10/state\",\"unit_of_measurement\":\"µg/m³\"}");
                client.publish("homeassistant/sensor/esp32_aqi/config", "{\"name\":\"ESP32 AQI\",\"state_topic\":\"homeassistant/sensor/esp32_aqi/state\",\"unit_of_measurement\":\"AQI\"}");
            } else {
                Serial.print("Failed, rc="); Serial.print(client.state());
                Serial.println(" Retrying in 5 seconds...");
                delay(5000);
            }
        }
    }

    static void publish(const char* topic, const String& payload) {
        if (!client.connected()) {
            connect();
        }
        client.publish(topic, payload.c_str());
    }

    static void loop() {
        if (!client.connected()) {
            connect();
        }
        client.loop();
    }
};

// Declare static members
WiFiClient MQTTClient::espClient;
PubSubClient MQTTClient::client;

#endif // MQTT_CLIENT_H