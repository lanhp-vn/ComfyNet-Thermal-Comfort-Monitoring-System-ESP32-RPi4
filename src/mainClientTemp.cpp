#define UID "temp0"

#define HAS_BME

#include <Arduino.h>

#include "networkHandler.h"

#ifdef HAS_BME
#include "sensorBME.h"
struct BMEstruct BME;
#endif

#define NTC_PIN 36

struct NTCstruct {
    float voltage;
    float resistance;
    float temperature;
};

struct NTCstruct NTC;

unsigned long lastTime = 0;

String payload = "";                  // tcp payload
float lastTemp[5] = {0, 0, 0, 0, 0};  // FIFO buffer for temperature

// NTC params
// coupled with 30k resistor
// vcc = 2.22v
// 24.6c = 0.91v

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    // setup hardware
    pinMode(NTC_PIN, INPUT);
    
    #ifdef HAS_BME
    // setup bme
    sensorBME.initBME();
    #endif

    // connect to wifi
    networkHandler.initWifi(true);
}

struct NTCstruct readNTC() {
    struct NTCstruct ntc;
    // read and calculate temperature
    ntc.voltage = analogRead(NTC_PIN) / 1024.0 * 0.9934;
    ntc.resistance = 19305 * ntc.voltage / (3.3 - ntc.voltage);
    ntc.temperature = 1.0 / (1.0 / 298.15 + 1.0 / (-7334.906) * (log(ntc.resistance / 10000.0))) - 272.15;

    // validate temperature:
    // if any element is 0, push current temperature to 1st element of buffer and return that temperature
    for (int i = 0; i < 5; i++) {
        if (lastTemp[i] == 0) {
            lastTemp[i] = ntc.temperature;
            return ntc;
        }
    }
    // else, calculate average of buffer and push current temperature to last element of buffer only if it is within 2 degrees of average
    float avg = 0;
    for (int i = 0; i < 5; i++) {
        avg += lastTemp[i];
    }
    avg /= 5;

    if (ntc.temperature < avg + 2 && ntc.temperature > avg - 2) {
        for (int i = 0; i < 4; i++) {
            lastTemp[i] = lastTemp[i + 1];  // shift left
        }
        lastTemp[4] = ntc.temperature;
    }

    ntc.temperature = avg;

    return ntc;
}

void debugPrint() {
    Serial.println("NTC:");
    Serial.print("Value: ");
    Serial.print(analogRead(NTC_PIN));
    Serial.print(" (0-1023)\t");
    Serial.print("Voltage: ");
    Serial.print(NTC.voltage);
    Serial.print(" V\t");
    Serial.print("Resistance: ");
    Serial.print(NTC.resistance);
    Serial.print(" Ohm\t");
    Serial.print("Temperature: ");
    Serial.print(NTC.temperature);
    Serial.println(" C\t");

    #ifdef HAS_BME
    Serial.println("BME:");
    Serial.print("Temperature: ");
    Serial.print(BME.temperature);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(BME.humidity);
    Serial.print(" %\t");
    Serial.print("Pressure: ");
    Serial.print(BME.pressure);
    Serial.print(" hPa\t");
    Serial.print("Gas: ");
    Serial.print(BME.gas);
    Serial.println(" KOhm\t");
    Serial.println("");
    #endif
}

void loop() {
    // for every half second
    if (millis() - lastTime >= 500) {
        // read sensors
        NTC = readNTC();

        #ifdef HAS_BME
        BME = sensorBME.readBME();
        #endif

        // print to serial
        debugPrint();

        // send tcp packet to 192.168.4.1
        #ifdef HAS_BME
        payload = "?uid=" + String(UID) + "&tempNTC=" + String(NTC.temperature) + "&temp=" + String(BME.temperature) + "&hum=" + String(BME.humidity) + "&pres=" + String(BME.pressure) + "&gas=" + String(BME.gas) + "&";
        #else
        payload = "?uid=" + String(UID) + "&tempNTC=" + String(NTC.temperature) + "&";
        #endif
        networkHandler.sendTcp(payload, "192.168.4.1", 80);
        
        // reset the timer
        lastTime = millis();
    }

    networkHandler.handleWifi();
}