#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "Adafruit_Sensor.h"
#include "Adafruit_BME680.h"

struct BMEstruct {
    float temperature;
    float humidity;
    float pressure;
    float gas;
};

class SensorBME {
private:
    Adafruit_BME680 bme;  // I2C
public:
    void initBME() {
        if (!bme.begin()) {
            Serial.println("Could not find a valid BME680 sensor, check wiring!");
            while (1);
        }
        // Set up oversampling and filter initialization
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme.setGasHeater(320, 150);  // 320*C for 150 ms
    }

    BMEstruct readBME() {
        struct BMEstruct bmeData;
        if (!bme.performReading()) {
            Serial.println("Failed to poll sensor");
            return bmeData;
        }
        bmeData.temperature = bme.temperature;
        bmeData.humidity = bme.humidity;
        bmeData.pressure = bme.pressure / 100.0;
        bmeData.gas = bme.gas_resistance / 1000.0;
        return bmeData;
    }
};

SensorBME sensorBME;