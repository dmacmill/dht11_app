#include <Arduino.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <sstream>


#define WIFI_NETWORK ""
#define WIFI_PASSWORD "" // CHANGE ME
#define WIFI_TIMEOUT_MS 20000

#define HOST "xxx.xxx.xxx.xxx"
#define PORT 8090

const char * CA_CLIENT_KEY = "";
const char * CA_CERT = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDizCCAnOgAwIBAgIUY3E3C9kfxcYD7Zpc88Sn9955O3wwDQYJKoZIhvcNAQEL\n" \
"BQAwVTELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAk1BMSEwHwYDVQQKDBhJbnRlcm5l\n" \
"dCBXaWRnaXRzIFB0eSBMdGQxFjAUBgNVBAMMDTE5Mi4xNjguMS4xNTIwHhcNMjUw\n" \
"NDE5MjMyNDA5WhcNMjYwNDE5MjMyNDA5WjBVMQswCQYDVQQGEwJVUzELMAkGA1UE\n" \
"CAwCTUExITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEWMBQGA1UE\n" \
"AwwNMTkyLjE2OC4xLjE1MjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"ALRXTOVDAB3OFWA31T4R9s+vpbjPXIot6NJhVdYPD/Ay+uaTfhiD3gGwyTX4X+8U\n" \
"bq0KmPXosZdtN/WdTxiL3+gGkf+Yaa2pmslJPN5yKvmG/qG95nDtevKBfyblnYGG\n" \
"MNHSIBWnBmP+mv0+wndV2seAENMmqWZ4F2lnhNfkXf24SZNAz30rY+lr/lz6tl87\n" \
"srULeKpxaotSpKaJFIlkkCukF6A+5a4ks1bnaCp49Qn7n2js5q0FU/3g1S8eeDlB\n" \
"W0vE62Yl8Z7jF/FZJxvmP4KCGRrsH6lxGolQHEugzOTHlMFx9A9dtA/FYKK4g8NB\n" \
"N5J7FnqFsB7pJFrcffnY8vcCAwEAAaNTMFEwHQYDVR0OBBYEFJPADdlo+JZ+DN/9\n" \
"506mLNUJIOg+MB8GA1UdIwQYMBaAFJPADdlo+JZ+DN/9506mLNUJIOg+MA8GA1Ud\n" \
"EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAHyc5hDa2hoznwFBeffYMxL2\n" \
"xtGbDV5kA7tkUcahOv0gzYoU/GbKxeq4EstO1Hlc4XUeNwJ4iEGntE3hdqnSknIS\n" \
"1I306hlUTBy/yVmvB2tgRp/3+rQSvlcTYzQfSevijOL0zKAa82fDztFlAOoRNKLo\n" \
"Jw7/h5ZJlRSIKFQUEkIw7nj+NLhRq+gGPFCsHCfCSEAcHZeDf358XjT+dxPUOAKO\n" \
"ECSqtlpYssivRQMwfV4c1S7H92eag/VYApNf6NZuhtjeOC1MhPggWLjV/+XG2j2F\n" \
"29LV4nZwdhoDdvIYtnTGaiziz414e2icEGjkqxmaeEa5erykAIGwbavPUJB3qWQ=\n" \
"-----END CERTIFICATE-----\n";


#define DHT11_PIN 21

DHT dht11(DHT11_PIN, DHT11);

WiFiClientSecure client;

std::string s = "";

long unixtime = 0;

void connectToWifi() {
    Serial.println("Connecting to wifi....");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS) {
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

void getUnixTime() {
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
}

void setup() {
    Serial.begin(9600);
    
    connectToWifi();
    client.setInsecure();
    while (unixtime == 0) {
        getUnixTime();
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