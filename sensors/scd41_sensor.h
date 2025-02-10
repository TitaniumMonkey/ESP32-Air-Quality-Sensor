#ifndef SCD41_SENSOR_H
#define SCD41_SENSOR_H

#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"

class SCD41Sensor {
private:
    static SCD4x scd41;
    static int co2;

public:
    static void begin() {
        if (!scd41.begin()) {
            Serial.println("Could not find a valid SCD41 sensor, check wiring!");
            return;
        }
        Serial.println("SCD41 sensor initialized");

        // Start periodic measurements
        scd41.startPeriodicMeasurement();
    }

    static bool read() {
        if (scd41.readMeasurement()) {
            co2 = scd41.getCO2();
            return true;
        }
        return false;  // Return false on failure but don't print an error
    }

    static int getCO2() { return co2; }
};

// Declare static variables
SCD4x SCD41Sensor::scd41;
int SCD41Sensor::co2 = 0;

#endif // SCD41_SENSOR_H