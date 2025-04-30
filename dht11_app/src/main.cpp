#include <Arduino.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <sstream>

#include "client.hpp"
#include "credentials.hpp"

#define DHT11_PIN 21
DHT dht11(DHT11_PIN, DHT11);

WiFiClientSecure client;
long int unixtime = 0;
std::string s = "";

void setup() {
    Serial.begin(9600);
    
    connectToWifi(client, WIFI_NETWORK, WIFI_PASSWORD, WIFI_TIMEOUT_MS);
    client.setInsecure();
    while (unixtime == 0) {
        unixtime = getUnixTime(client);
        delay(1000);
    }
    unixtime -= millis()/1000;
    client.setCACert(CA_CERT);
    dht11.begin();
}

void loop() {
    // get timestamp
    unsigned long ts = unixtime + (millis()/1000);
    // read humidity
    float humi = dht11.readHumidity();
    // read temp in C
    float tempC = dht11.readTemperature();
    // read temp in F
    float tempF = dht11.readTemperature(true);

    if (isnan(tempC) || isnan(tempF) || isnan(humi)) {
        Serial.println("[ERROR] Failed to read from DHT11 sensor!");
    }
    else {
        // Setup "packet"
        std::stringstream ss;
        ss << "[" << ts << "] " << humi << "%" << tempC << "°C" << tempF << "°F|";
        std::string result = ss.str();

        s.append(result);

        // send over socket
        if (s.length() >= 200) {
            if (!client.connect(HOST, PORT)) {
                Serial.println("[ERROR] connection to host lost.");
            }
            else {
                client.print(s.c_str());
                client.stop();
                s = "";
            }
        }

        // Display on serial (shift ctrl p -> "serial")
        Serial.println(result.c_str());
    }

    // wait 2 seconds, sensor needs a delay
    delay(2000);
}