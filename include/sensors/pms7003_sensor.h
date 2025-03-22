#ifndef PMS7003_SENSOR_H
#define PMS7003_SENSOR_H

#include <HardwareSerial.h>

#define PMS7003_RX_PIN 14  // RX = D14 (ESP32 receives data)
#define PMS7003_TX_PIN 27  // TX = D27 (ESP32 sends data)

class PMS7003Sensor {
private:
    static HardwareSerial pmsSerial;
    static int pm1_0, pm2_5, pm10; // Store sensor values
    static bool newDataAvailable;

    static void processPMSFrame(uint8_t* buffer);

public:
    static void begin();
    static void read();
    static bool hasNewData();
    static int getPM1_0();
    static int getPM2_5();
    static int getPM10();
};

#endif // PMS7003_SENSOR_H