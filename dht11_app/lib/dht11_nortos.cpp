/**
 * This is the old main.cpp file for this app, no longer necessary. 
 * Is here for storage mostly.
 * 
 */

#include <DHT.h>
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
    int trys = 0;
    while (unixtime == 0 && trys < 5) {
        trys++;
        unixtime = getUnixTime(client);
        delay(1000);
    }
    if (unixtime == 0) {
        s += "[ERROR] couldn't get unix time. Time will start at 0.|";
    }
    unixtime -= millis()/1000;
    client.setCACert(CA_CERT);
    dht11.begin();
}

void loop() {
    unsigned long ts = unixtime + (millis()/1000);
    float humi = dht11.readHumidity();
    float tempC = dht11.readTemperature();
    float tempF = dht11.readTemperature(true);

    if (isnan(tempC) || isnan(tempF) || isnan(humi)) {
        // Handle read failure
        s += "[ERROR] Failed to read from DHT11 sensor!|";
        Serial.println("[ERROR] Failed to read from DHT11 sensor!");
        // send over socket
        if (s.length() >= 200) {
            int success = send(client, HOST, PORT, s);
            if (success) {
                s = "";
            }
            else{
                s += "[ERROR] lost connection|";
            }
        }
    }
    else {
        // Setup "packet"
        std::stringstream ss;
        // ss << "[" << ts << "] " << humi << "%" << tempC << "°C" << tempF << "°F|";
        ss << "[" << ts << "]," << humi << "," << tempC << "," << tempF << "|";
        std::string result = ss.str();

        s.append(result);

        // send over socket
        if (s.length() >= 200) {
            int success = send(client, HOST, PORT, s);
            if (success) {
                s = "";
            }
            else {
                s += "[ERROR] lost connection|";
            }
        }

        // Display on serial (shift ctrl p -> "serial")
        Serial.println(result.c_str());
    }

    // wait 2 seconds, sensor needs a delay
    delay(2000);
}