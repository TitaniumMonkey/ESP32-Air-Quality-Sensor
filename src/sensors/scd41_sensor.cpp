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
    Wire.begin();  // Ensure I2C is initialized
    delay(100);    // Give devices time to initialize
    
    if (!scd41.begin()) {
        Serial.println("Could not find a valid SCD41 sensor, check wiring!");
        return;
    }
    Serial.println("SCD41 sensor initialized");

    // Start periodic measurements
    scd41.startPeriodicMeasurement();
}

bool SCD41Sensor::read() {
    static unsigned long lastRead = 0;
    static bool measurementStarted = false;
    static unsigned long lastError = 0;
    
    // Ensure periodic measurement is started
    if (!measurementStarted) {
        if (!scd41.startPeriodicMeasurement()) {
            Serial.println("Failed to start SCD41 periodic measurement");
            return false;
        }
        measurementStarted = true;
        lastRead = millis();
        return false;
    }
    
    // Wait at least 5 seconds between reads
    if (millis() - lastRead < 5000) {
        return false;
    }
    
    // Try to read measurement
    if (scd41.readMeasurement()) {
        co2 = scd41.getCO2();
        temperatureC = scd41.getTemperature();
        humidity = scd41.getHumidity();
        lastRead = millis();
        return true;
    }
    
    // If read fails, try to recover
    if (millis() - lastError > 30000) {  // Only try recovery every 30 seconds
        Serial.println("SCD41 read failed, attempting recovery...");
        lastError = millis();
        
        // Stop periodic measurement
        scd41.stopPeriodicMeasurement();
        delay(100);
        
        // Reinitialize I2C
        Wire.end();
        delay(100);
        Wire.begin();
        delay(100);
        
        // Restart sensor
        if (!scd41.begin()) {
            Serial.println("SCD41 recovery failed - sensor not found");
            return false;
        }
        
        // Restart periodic measurement
        if (!scd41.startPeriodicMeasurement()) {
            Serial.println("SCD41 recovery failed - could not start measurement");
            return false;
        }
        
        measurementStarted = true;
        lastRead = millis();
    }
    
    return false;
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
