#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "secrets.h"

class WiFiManager {
public:
    static void connect() {
        Serial.print("Connecting to WiFi: ");
        Serial.println(WIFI_SSID);
        
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(1000);
            Serial.print(".");
            attempts++;
        }
        
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nWiFi Connected!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\nFailed to connect to WiFi");
        }
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