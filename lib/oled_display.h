#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Reset pin (not used)
#define SCREEN_ADDRESS 0x3C  // Default I2C address for 0.96" OLED

class OLEDDisplay {
private:
    static Adafruit_SSD1306 display;

public:
    static void init() {
        if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
            Serial.println("SSD1306 OLED allocation failed");
            return;
        }
		Serial.println("OLED initialized");
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.display();
    }

    static void update(float temperatureF, float humidity, float pressure, int co2, int pm1_0, int pm2_5, int pm10, int aqi, int enhanced_aqi) {
        display.clearDisplay();
        display.setCursor(0, 0);

        // Format and display sensor data
        display.print("Temp: "); display.print(temperatureF, 1); display.println(" F");
        display.print("Humidity: "); display.print(humidity, 1); display.println(" %");
        display.print("Pressure: "); display.print(pressure, 1); display.println(" hPa");
        display.print("CO2: "); display.print(co2); display.println(" ppm");
        display.print("PM: ");
        display.print(pm1_0); display.print("/");
        display.print(pm2_5); display.print("/");
        display.print(pm10); display.println(" ug/m3");
		display.print("AQI: "); display.print(aqi); 
		display.print(" | EAQI: "); display.print(enhanced_aqi);
        
        display.display();
    }
};

// Declare static member
Adafruit_SSD1306 OLEDDisplay::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#endif // OLED_DISPLAY_H
