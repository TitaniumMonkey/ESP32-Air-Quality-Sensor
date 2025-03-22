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
    static bool init();
    static bool isConnected();
    static bool publish(const char* topic, const String& payload);
    static void disconnect();
    static void loop();
};

#endif // MQTT_CLIENT_H
