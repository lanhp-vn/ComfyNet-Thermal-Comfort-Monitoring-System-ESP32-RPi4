#include <Arduino.h>
//esp32
#include <WiFi.h>
//esp8266
// #include <ESP8266WiFi.h>

WiFiServer server(80);
IPAddress ip(192, 168, 4, 1); // server ip
IPAddress subnet(255, 255, 255, 0);

String payload = "";
unsigned long lastMillis = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");
    
    // setup wifi as AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_SSID, WIFI_PASSWD, 13, 0, 10); // maximum of 10 clients
    WiFi.softAPConfig(ip, ip, subnet);
    Serial.println("AP started: ");
    Serial.println(WiFi.softAPIP());

    // start listening for tcp packets
    server.begin();
    Serial.println("Server started");
}

void loop() {
    WiFiClient client = server.available();
    // client.setNoDelay(true); // prevent collision with other clients
    if (client) {
        Serial.println("Client connected");
        Serial.println("IP: ");
        Serial.println(client.remoteIP());
        lastMillis = millis();
        while (client.connected() && (millis() - lastMillis < 1000)) { // 1 second timeout
            if (client.available()) {
                // push to string Payload
                char c = client.read();
                payload += c;
            }
        }
        Serial.println(payload);
        Serial.println("Time taken: " + String(millis() - lastMillis) + " ms");
        payload = "";
        client.stop();
        Serial.println("Client disconnected");
        digitalWrite(2, !digitalRead(2)); // blink led
    }
}