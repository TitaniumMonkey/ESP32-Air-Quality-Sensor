#ifndef SGP30_SENSOR_H
#define SGP30_SENSOR_H

#include <Wire.h>
#include <Adafruit_SGP30.h>

class SGP30Sensor {
private:
    static Adafruit_SGP30 sgp;
    static float tvoc;
    static float h2;
    static float ethanol;
    static bool initialized;

public:
    static void begin();
    static void read();
    static float getTVOC();
    static float getH2();
    static float getEthanol();
};

#endif // SGP30_SENSOR_H 