#include "sensors/bme680_sensor.h"
#include "sensors/scd41_sensor.h"
#include "sensors/pms7003_sensor.h"
#include "lib/scheduler.h"
#include "lib/wifi_manager.h"
#include "lib/mqtt_client.h"
#include "lib/oled_display.h"
#include "lib/enhanced_aqi.h"
#include "secrets.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nESP32 Air Quality Sensor Initializing...");
    
    // Connect to WiFi and sync NTP time
    WiFiManager::connect();
    WiFiManager::syncNTP();
    
    // Initialize Sensors and Communication
    Scheduler::init();
    MQTTClient::init();
    OLEDDisplay::init();
}

void loop() {
    Scheduler::run(); // Run the event-driven scheduler
    MQTTClient::loop(); // Maintain MQTT connection
}
