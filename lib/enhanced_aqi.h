#ifndef ENHANCED_AQI_H
#define ENHANCED_AQI_H

#include "sensors/bme680_sensor.h"
#include "sensors/scd41_sensor.h"
#include "sensors/pms7003_sensor.h"

float calculatePM_AQI(float pm25) {
    if (pm25 <= 12) return (pm25 / 12) * 50;
    else if (pm25 <= 35) return ((pm25 - 12) / (35 - 12)) * 50 + 50;
    else if (pm25 <= 55) return ((pm25 - 35) / (55 - 35)) * 50 + 100;
    else if (pm25 <= 150) return ((pm25 - 55) / (150 - 55)) * 100 + 150;
    else if (pm25 <= 250) return ((pm25 - 150) / (250 - 150)) * 100 + 200;
    else return 500;
}

float calculateVOC_AQI(float gas_resistance) {
    if (gas_resistance >= 500000) return 0; // Very clean air
    else if (gas_resistance >= 200000) return 50;
    else if (gas_resistance >= 100000) return 100;
    else if (gas_resistance >= 50000) return 150;
    else if (gas_resistance >= 10000) return 200;
    else if (gas_resistance >= 5000) return 300;
    else return 500; // Poor air quality (high VOCs)
}

float calculateCO2_AQI(float co2) {
    if (co2 <= 600) return (co2 / 600) * 50;
    else if (co2 <= 800) return ((co2 - 600) / (800 - 600)) * 50 + 50;
    else if (co2 <= 1000) return ((co2 - 800) / (1000 - 800)) * 50 + 100;
    else if (co2 <= 1500) return ((co2 - 1000) / (1500 - 1000)) * 100 + 150;
    else if (co2 <= 2000) return ((co2 - 1500) / (2000 - 1500)) * 100 + 200;
    else return 500;
}

float getEnhancedAQI() {
    float pm25 = PMS7003Sensor::getPM2_5();
    float iaq = BME680Sensor::getGasResistance();
    float co2 = SCD41Sensor::getCO2();

    float pm_aqi = calculatePM_AQI(pm25);
    float voc_aqi = calculateVOC_AQI(iaq);
    float co2_aqi = calculateCO2_AQI(co2);

    return fmax(pm_aqi, fmax(voc_aqi, co2_aqi));
}

#endif // ENHANCED_AQI_H