#include <Arduino.h>
#include <WiFi.h>

class NetworkHandler {
private:
    WiFiClient client;
    bool autoReconnect = false;

public:
    void initWifi(bool autoReconnect = true) {
        // if wifi got disconnected or not connected, reconnect
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Connecting to WiFi...");
            // try to connect
            WiFi.begin(WIFI_SSID, WIFI_PASSWD);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
                digitalWrite(22, !digitalRead(22));
            }
            Serial.println("\nConnected to WiFi");
            Serial.println(WiFi.localIP());
            if (autoReconnect) {
                // auto reconnect
                WiFi.persistent(true);
                WiFi.setAutoReconnect(true);
                // set flag
                this->autoReconnect = true;
            }
        }
    }

    void handleWifi() {
        // if wifi got disconnected, reconnect
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Wifi disconnected, reconnecting..");
            // try to connect
            initWifi(this->autoReconnect);
        }
    }

    void sendTcp(String payload, String ip, int port) {
        // if wifi got disconnected, reconnect
        handleWifi();
        // connect to server
        client.connect(ip.c_str(), port);
        // send payload
        client.print(payload);
        // ensure the data is sent,
        // this is a problem with esp32 where it goes into power saving mode way too soon,
        // turning off the radio before the entire packet is sent, causing packet corruption
        delay(10);
        // close connection
        client.stop();
    }
};

NetworkHandler networkHandler;