#include <Arduino.h>

#define NTC
#define WIND

#define NTC_PIN 4
#define ENCODER_PIN 27

#ifdef WIND
    unsigned int counter = 0;
    unsigned long lastTime = 0;
    unsigned long lastMicros = 0;

    // setup interrupt on pin 14
    void IRAM_ATTR handleInterrupt() {
        if (micros() - lastMicros >= 300) {
            counter++;
            lastMicros = micros();
        } // else ignore as debounce
    }
#endif

// NTC params
// coupled with 30k resistor
// vcc = 2.22v
// 24.6c = 0.91v

// WIND params
// calibration: 6.9m/s = 420hz

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");
    
    #ifdef WIND
        pinMode(ENCODER_PIN, INPUT);
        attachInterrupt(ENCODER_PIN, handleInterrupt, RISING);
    #endif
    #ifdef NTC
        pinMode(NTC_PIN, INPUT);
    #endif
}

void loop() {
    #ifdef NTC
        float voltage = analogRead(NTC_PIN) / 1024.0 * 0.9834;
        float resistance = 19305 * voltage / (3.3 - voltage);
        float temperature = 1.0 / (1.0 / 298.15 + 1.0 / (-7334.906)* (log(resistance / 10000.0))) - 273.15;
        Serial.print("Value: ");
        Serial.print(analogRead(NTC_PIN));
        Serial.println(" (0-1023)");
        Serial.print("Voltage: ");
        Serial.print(voltage);
        Serial.println(" V");
        Serial.print("Resistance: ");
        Serial.print(resistance);
        Serial.println(" Ohm");
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
        Serial.println("");
    #endif
    #ifdef WIND
        if (millis() - lastTime >= 1000) {
            float windspeed = counter / 66.0 * 1.5;
            // print the number of interrupts
            Serial.println(counter);
            Serial.print("Windspeed: ");
            Serial.print(windspeed);
            Serial.println(" m/s");
            // reset the counter
            counter = 0;
            // reset the timer
            lastTime = millis();
        }
    #endif
}