#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <Arduino.h>
#include <WiFiClientSecure.h>

#include "credentials.hpp"

void connectToWifi(WiFiClientSecure & client, const char wifi_network [], const char wifi_password [], const int & wifi_timeout_ms);
long int getUnixTime(WiFiClientSecure & client);
bool send(WiFiClientSecure & client, const char host [], const int port, const std::string & s);


/* @brief connectToWifi - attempt to connect to given WifiSecureClient with given SSID credentials and timeout > 0
 * Will block for at least wifi_timeout_ms amount of milliseconds. Delays are in 100 ms increments.
 * 
 * @param client - external WifiSecureClient to have connected to the internet
 * @param wifi_network - ssid to connect to.
 * @param wifi_password - password of the ssid
 * @param wifi_timeout_ms - max time in milliseconds to connect successfully to the ssid.
 */
void connectToWifi(WiFiClientSecure & client, const char wifi_network [], const char wifi_password [], const int & wifi_timeout_ms) {
    Serial.println("[INFO] Connecting to wifi....");
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_network, wifi_password);

    unsigned long startAttemptTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifi_timeout_ms) {
        Serial.print(".");
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("connected!");
    }
    else {
        Serial.println("[ERROR] couldn't connect to wifi :<");
        Serial.println(WiFi.localIP());
        Serial.println(WiFi.status());
    }
}

/* @brief getUnixTime - connects to worldtimeapi.org and sends GET request for epoch time in New York
 * Does not use ssl/tls for this part.
 *
 * @param client - an external WifiClientSecure that is already connected to the internet.
 * @return long int - the unixtime
 */
long int getUnixTime(WiFiClientSecure & client) {
    long unixtime = 0;
    if (!client.connect("worldtimeapi.org", 443)) {
        Serial.println("[ERROR] connection to timezone host lost.");
    }
    else {
        Serial.println("[INFO] Connected to worldtimeapi.org!");

        // send GET request
        client.println("GET /api/timezone/America/New_York HTTP/1.1");
        client.println("Host: worldtimeapi.org");
        client.println("User-Agent: ESP32");
        client.println("Connection: close");
        client.println();

        // wait for server response
        while(client.connected()) {
            String line = client.readStringUntil('\n');
            if (line == "\r") {
                break;
            }
        }

        // read and print the body
        while(client.available()) {
            String line = client.readStringUntil('\n');
            if (line.length() > 0) {
                int index = line.indexOf("\"unixtime\":");
                if (index >= 0) {
                    int start = index + 11;
                    int end = line.indexOf(",", start);
                    String unixTimeStr = line.substring(start, end);
                    unixtime = unixTimeStr.toInt();
                    Serial.println(unixtime);
                }
            }
            Serial.println(line);
        }
        client.stop();
        Serial.println("[INFO] worldtimeapi.org connection closed");
    }
    return unixtime;
}

/* @brief send - sends a string over the socket to the host
 * 
 * @param client - an external WifiClientSecure that is already connected to the internet.
 * @param s - the string to send over the socket
 * @return int - true if successful, false if not
 */
bool send(WiFiClientSecure & client, const char host [], const int port, const std::string & s) {
    bool success = true;
    // send over socket
    if (!client.connect(host, port)) {
        success = false;
    }
    else {
        client.print(s.c_str());
        client.stop();
    }
    return success;
}

#endif // CLIENT_HPP_