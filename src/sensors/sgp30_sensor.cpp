#include "include/sensors/sgp30_sensor.h"

// Initialize static members
Adafruit_SGP30 SGP30Sensor::sgp;
float SGP30Sensor::tvoc = 0;
float SGP30Sensor::h2 = 0;
float SGP30Sensor::ethanol = 0;
bool SGP30Sensor::initialized = false;

void SGP30Sensor::begin() {
    if (!sgp.begin()) {
        Serial.println("SGP30 sensor not found!");
        return;
    }
    Serial.println("SGP30 sensor found!");
    initialized = true;
}

void SGP30Sensor::read() {
    if (!initialized) return;

    if (sgp.IAQmeasure()) {
        tvoc = sgp.TVOC;
        // Get raw H2 and ethanol values
        if (sgp.IAQmeasureRaw()) {
            h2 = sgp.rawH2;
            ethanol = sgp.rawEthanol;
        }
    } else {
        Serial.println("Measurement failed");
    }
}

float SGP30Sensor::getTVOC() {
    return tvoc;
}

float SGP30Sensor::getH2() {
    return h2;
}

float SGP30Sensor::getEthanol() {
    return ethanol;
} 