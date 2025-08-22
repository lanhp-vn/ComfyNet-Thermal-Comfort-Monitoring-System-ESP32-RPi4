#define UID "wind0"

#define HAS_BME

#include <Arduino.h>

#include "networkHandler.h"

#ifdef HAS_BME
#include "sensorBME.h"
struct BMEstruct BME;
#endif

#define ENCODER_PIN 27

unsigned int counter = 0;
float counterAvg = 0;
float windspeed = 0;
unsigned long lastTime = 0;
unsigned long lastMicros = 0;

// setup interrupt on pin 14
void IRAM_ATTR handleInterrupt() {
    if (micros() - lastMicros >= 300) {
        counter++;
        lastMicros = micros();
    }  // else ignore as debounce
}

String payload = "";                   // tcp payload
float lastCount[5] = {0, 0, 0, 0, 0};  // FIFO buffer for temperature

// Wind params
// calib: 1.0 - 7; 1.1 - 9; 1.3 - 12; 1.5 - 15; 1.6 - 17; 1.7 - 20; 1.9 - 24;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    // setup hardware
    pinMode(ENCODER_PIN, INPUT);
    attachInterrupt(ENCODER_PIN, handleInterrupt, RISING);
    pinMode(22, OUTPUT);

    #ifdef HAS_BME
    // setup bme
    sensorBME.initBME();
    #endif

    // connect to wifi
    networkHandler.initWifi();
}

void computeAvgCounter() {
    // validate the counter
    // if any element is 0, push current value to 1st element of buffer and return that value
    for (int i = 0; i < 5; i++) {
        if (lastCount[i] == 0) {
            lastCount[i] = counter;
            counterAvg = counter;
            return;
        }
    }

    // else, calculate average of buffer and push current value to last element of buffer only if it is within 5 degrees of average
    float avg = 0;
    for (int i = 0; i < 5; i++) {
        avg += lastCount[i];
    }
    avg /= 5;
    for (int i = 0; i < 4; i++) {
        lastCount[i] = lastCount[i + 1];  // shift left
    }
    lastCount[4] = counter;

    counterAvg = avg;
    return;
}

void debugPrint() {
    // print the number of interrupts
    Serial.print(counter);
    Serial.print("\t");
    Serial.println(counterAvg);

    Serial.print("Windspeed: ");
    Serial.print(windspeed);
    Serial.println(" m/s");

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
    // for every second
    if (millis() - lastTime >= 1000) {
        // read sensor
        computeAvgCounter();

        #ifdef HAS_BME
        BME = sensorBME.readBME();
        #endif

        // calculate windspeed
        windspeed = (counterAvg - -10.851063829787236) / 17.872340425531924;

        // edge case: if counterAVG is < 2.5, set windspeed to 0
        if (counterAvg < 2.5) {
            windspeed = 0;
        }

        // print to serial
        debugPrint();

        // send tcp packet to 192.168.4.1
        #ifdef HAS_BME
        payload = "?uid=" + String(UID) + "&windspeed=" + String(windspeed) + "&temperature=" + String(BME.temperature) + "&humidity=" + String(BME.humidity) + "&pressure=" + String(BME.pressure) + "&gas=" + String(BME.gas) + "&";
        #else
        payload = "?uid=" + String(UID) + "&windspeed=" + String(windspeed) + "&";
        #endif
        networkHandler.sendTcp(payload, "192.168.4.1", 80);

        // reset the counter
        counter = 0;
        // reset the timer
        lastTime = millis();
        // blink led
        digitalWrite(22, !digitalRead(22));
    }

    networkHandler.handleWifi();
}