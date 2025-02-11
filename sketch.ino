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
    while (!Serial) {
        delay(100);
    }
    
    Serial.println("\nESP32 Air Quality Monitor Starting...");

    // Initialize the scheduler which will handle all components
    Scheduler::init();
}

void loop() {
    Scheduler::run();
}
