#include "include/sensors/pms7003_sensor.h"

// Initialize static members
HardwareSerial PMS7003Sensor::pmsSerial(2);
int PMS7003Sensor::pm1_0 = 0;
int PMS7003Sensor::pm2_5 = 0;
int PMS7003Sensor::pm10 = 0;
bool PMS7003Sensor::newDataAvailable = false;

void PMS7003Sensor::begin() {
    pmsSerial.begin(9600, SERIAL_8N1, PMS7003_RX_PIN, PMS7003_TX_PIN);
    Serial.println("PMS7003 sensor initialized");

    // Force PMS7003 to enter Active Mode
    uint8_t cmd[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x70};
    pmsSerial.write(cmd, sizeof(cmd));
    delay(1000);
}

void PMS7003Sensor::read() {
    static uint8_t buffer[32];
    static int index = 0;
    newDataAvailable = false;

    while (pmsSerial.available() && !newDataAvailable) {
        uint8_t incomingByte = pmsSerial.read();

        // Ensure frame starts with 0x42 0x4D
        if (index == 0 && incomingByte != 0x42) continue;
        if (index == 1 && incomingByte != 0x4D) {
            index = 0;  // Reset if second byte isn't 0x4D
            continue;
        }

        buffer[index++] = incomingByte;

        // Full frame received (32 bytes)
        if (index >= 32) {
            processPMSFrame(buffer);
            index = 0;  // Reset index for next frame
            newDataAvailable = true; // Mark new data available
        }
    }
}

void PMS7003Sensor::processPMSFrame(uint8_t* buffer) {
    // Verify valid PMS7003 data frame
    if (buffer[0] != 0x42 || buffer[1] != 0x4D) {
        return;
    }

    // Extract PM values (Standard Atmosphere values)
    pm1_0  = (buffer[10] << 8) | buffer[11];
    pm2_5  = (buffer[12] << 8) | buffer[13];
    pm10   = (buffer[14] << 8) | buffer[15];
}

bool PMS7003Sensor::hasNewData() { 
    return newDataAvailable; 
}

int PMS7003Sensor::getPM1_0() { 
    return pm1_0; 
}

int PMS7003Sensor::getPM2_5() { 
    return pm2_5; 
}

int PMS7003Sensor::getPM10() { 
    return pm10; 
} 