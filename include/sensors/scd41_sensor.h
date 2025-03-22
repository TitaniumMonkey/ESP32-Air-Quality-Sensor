#ifndef SCD41_SENSOR_H
#define SCD41_SENSOR_H

#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"

class SCD41Sensor {
private:
    static SCD4x scd41;
    static int co2;
    static float temperatureC;
    static float humidity;

    static float celsiusToFahrenheit(float celsius);

public:
    static void begin();
    static bool read();
    static int getCO2();
    static float getTemperatureF();
    static float getHumidity();
};

#endif // SCD41_SENSOR_H