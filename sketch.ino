#include "include/sensors/sgp30_sensor.h"
#include "include/sensors/scd41_sensor.h"
#include "include/sensors/pms7003_sensor.h"
#include "include/lib/scheduler.h"
#include "include/lib/wifi_manager.h"
#include "include/lib/mqtt_client.h"
#include "include/lib/oled_display.h"
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
