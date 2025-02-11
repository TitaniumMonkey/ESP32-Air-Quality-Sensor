#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"  // Include secrets.h for WiFi credentials

class WiFiManager {
public:
    static bool connect(unsigned long timeout) {
        // If already connected, return true
        if (WiFi.status() == WL_CONNECTED) {
            return true;
        }

        Serial.print("Connecting to WiFi: ");
        Serial.println(WIFI_SSID);  // Print which network we're connecting to
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);  // Use WIFI_PASS from secrets.h

        unsigned long startAttempt = millis();

        // Try to connect until timeout
        while (millis() - startAttempt < timeout) {
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println();
                Serial.print("Connected to WiFi. IP: ");
                Serial.println(WiFi.localIP());
                return true;
            }
            Serial.print(".");
            delay(500);
        }

        Serial.println();
        Serial.println("WiFi connection timed out");
        WiFi.disconnect();
        return false;
    }

    static void disconnect() {
        WiFi.disconnect();
    }

    static bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    static void syncNTP() {
        configTime(-8 * 3600, 0, "pool.ntp.org"); // PST Timezone
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to obtain NTP time");
            return;
        }
        Serial.println("NTP time synchronized");
    }
};

#endif // WIFI_MANAGER_H
