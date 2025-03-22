#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "scheduler.h"  // Add this include for isOledOn()

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin (not used)
#define SCREEN_ADDRESS 0x3C  // Default I2C address for 0.96" OLED

class Scheduler;  // Forward declaration

// Declare function instead of calling it directly
extern bool isOledOn();

class OLEDDisplay {
private:
    static Adafruit_SSD1306 display;

public:
    static void init();
    static void update(float temperatureF, float humidity, int co2, 
                      int pm1_0, int pm2_5, int pm10, int aqi, 
                      float tvoc, float h2, float ethanol);
};

#endif // OLED_DISPLAY_H
