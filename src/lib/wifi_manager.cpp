#include "include/lib/wifi_manager.h"

bool WiFiManager::connect(unsigned long timeout) {
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

void WiFiManager::disconnect() {
    WiFi.disconnect();
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::syncNTP() {
    configTime(-8 * 3600, 0, "pool.ntp.org"); // PST Timezone
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain NTP time");
        return;
    }
    Serial.println("NTP time synchronized");
} 