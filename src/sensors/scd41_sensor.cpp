#include "include/sensors/scd41_sensor.h"

// Initialize static members
SCD4x SCD41Sensor::scd41;
int SCD41Sensor::co2 = 0;
float SCD41Sensor::temperatureC = 0;
float SCD41Sensor::humidity = 0;

float SCD41Sensor::celsiusToFahrenheit(float celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

void SCD41Sensor::begin() {
    if (!scd41.begin()) {
        Serial.println("Could not find a valid SCD41 sensor, check wiring!");
        return;
    }
    Serial.println("SCD41 sensor initialized");

    // Start periodic measurements
    scd41.startPeriodicMeasurement();
}

bool SCD41Sensor::read() {
    if (scd41.readMeasurement()) {
        co2 = scd41.getCO2();
        temperatureC = scd41.getTemperature();
        humidity = scd41.getHumidity();
        return true;
    }
    return false;  // Return false on failure but don't print an error
}

int SCD41Sensor::getCO2() { 
    return co2; 
}

float SCD41Sensor::getTemperatureF() { 
    return celsiusToFahrenheit(temperatureC); 
}

float SCD41Sensor::getHumidity() { 
    return humidity; 
} 