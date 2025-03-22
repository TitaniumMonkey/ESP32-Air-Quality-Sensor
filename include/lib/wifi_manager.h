#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"  // Include secrets.h for WiFi credentials

class WiFiManager {
public:
    static bool connect(unsigned long timeout);
    static void disconnect();
    static bool isConnected();
    static void syncNTP();
};

#endif // WIFI_MANAGER_H
