#include "include/lib/oled_display.h"

// Initialize static member
Adafruit_SSD1306 OLEDDisplay::display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void OLEDDisplay::init() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("SSD1306 OLED allocation failed");
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.display();
}

void OLEDDisplay::update(float temperatureF, float humidity, int co2, 
                        int pm1_0, int pm2_5, int pm10, int aqi, 
                        float tvoc, float h2, float ethanol) {
    if (!isOledOn()) {
        return;  // Don't do anything if display is off
    }

    display.clearDisplay();
    display.setCursor(0, 0);

    // Format and display sensor data
    display.print("Temp: "); display.print(temperatureF, 1); display.println(" F");
    display.print("Humidity: "); display.print(humidity, 1); display.println(" %");
    display.print("CO2: "); display.print(co2); display.println(" ppm");
    display.print("PM: ");
    display.print(pm1_0); display.print("/");
    display.print(pm2_5); display.print("/");
    display.print(pm10); display.println(" ug/m3");
    display.print("AQI: "); display.println(aqi);
    display.print("TVOC: "); display.print(tvoc, 0); display.println(" ppb");
    display.print("H2: "); display.print(h2, 0); display.println(" res");
    display.print("Ethanol: "); display.print(ethanol, 0); display.println(" res");

    display.display();
} 
