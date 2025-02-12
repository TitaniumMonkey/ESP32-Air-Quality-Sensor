#ifndef BME680_SENSOR_H
#define BME680_SENSOR_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define BME680_I2C_ADDR 0x77 // Default I2C address

class BME680Sensor {
private:
    static Adafruit_BME680 bme;
    static float temperatureC, temperatureF, humidity, pressure, altitude, gasResistance;
    static constexpr float TEMP_CALIBRATION_OFFSET = 2.1f;  // Temperature calibration offset in Celsius

public:
    static void begin() {
        if (!bme.begin(BME680_I2C_ADDR)) {
            Serial.println("Could not find a valid BME680 sensor, check wiring!");
            return;
        }
        Serial.println("BME680 sensor initialized");

        // Set up oversampling and filter initialization
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150); // 320°C for 150ms
    }

    static void read() {
        if (!bme.performReading()) {
            Serial.println("Failed to perform BME680 reading");
            return;
        }

        // Store calibrated values
        temperatureC = bme.temperature - TEMP_CALIBRATION_OFFSET;  // Apply calibration
        temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
        humidity = bme.humidity;
        pressure = bme.pressure / 100.0; // Convert Pa to hPa
        altitude = 44330 * (1.0 - pow(pressure / 1013.25, 0.1903));
        gasResistance = bme.gas_resistance;
    }

    // Getter functions for latest stored values
    static float getTemperatureC() { return temperatureC; }
    static float getTemperatureF() { return temperatureF; }
    static float getHumidity() { return humidity; }
    static float getPressure() { return pressure; }
    static float getAltitude() { return altitude; }
    static float getGasResistance() { return gasResistance; } // New function to get gas resistance
};

// Declare static variables
Adafruit_BME680 BME680Sensor::bme;
float BME680Sensor::temperatureC = 0.0;
float BME680Sensor::temperatureF = 0.0;
float BME680Sensor::humidity = 0.0;
float BME680Sensor::pressure = 0.0;
float BME680Sensor::altitude = 0.0;
float BME680Sensor::gasResistance = 0.0; // Initialize gas resistance

#endif // BME680_SENSOR_H
