/**
 * dht11.cpp
 * 
 * task to get from dht11 sensor and write to queue
 * task to empty the queue after a time 
 * 
*/

#include <Arduino.h>
#include <DHT.h>

#include <string>
#include <sstream>
#include "client.hpp"

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// globals
static const uint8_t msg_queue_len = 5;
static QueueHandle_t msg_queue;
static const size_t MSG_SIZE = 100;

#define DHT11_PIN 21
static DHT dht11(DHT11_PIN, DHT11);
static WiFiClientSecure client;
static long int unixtime = 0;

/**
 * sendTask
 * 
 * Consumer for msg_queue, sends everything over the socket if successfully 
 * connected.
 * 
 */
void sendTask(void * parameter) {
    char temp[MSG_SIZE];
    std::string s = "";
    while (1) {
        Serial.println("[INFO] sendTask: adding the following below");
        while (xQueueReceive(msg_queue, (void*)&temp, 0) == pdTRUE) {
            Serial.println(temp);

            s += std::string(temp);
        }
        if (s.length() != 0) {
            // send over socket
            int success = send(client, HOST, PORT, s.c_str());
            if (!success) {
                // Try to reconnect once, if fails just print an error.
                // This will try again next time the task is called.
                if (!client.connect(HOST, PORT)) {
                    Serial.println("[ERROR] lost connection");
                }
            }
        }
        s = "";
        
        vTaskDelay(6000 / portTICK_PERIOD_MS);
    }
}

/**
 * measureTask
 * 
 * Producer for msg_queue, gets temp, humidity values from dht11 sensor and if 
 * successful stores them in the message queue every 2 seconds.
 * 
 */
void measureTask(void * parameter) {
    Serial.println("[INFO] measureTask starting");
    while (1) {
        Serial.println("[INFO] reading from sensor");
        unsigned long ts = unixtime + (millis()/1000);

        float humi = dht11.readHumidity();
        float tempC = dht11.readTemperature();
        float tempF = isnan(tempC) ? -9999.0 : tempC * 1.8 + 32.0;

        Serial.println("[INFO] done reading from sensor");

        if (isnan(tempC) || isnan(humi)) {
            // Handle read failure
            std::string s = "[ERROR] Failed to read from DHT11 sensor!|";
            Serial.println(s.c_str());

            char buf [MSG_SIZE];
            memset(buf, 0, sizeof(buf));
            strcpy(buf, s.c_str());
            
            if (xQueueSend(msg_queue, &buf, 10) != pdTRUE) {
                Serial.println("measureTask: msg_queue is full");
            }
        }
        else {
            // Insert values into buffer and send to queue
            std::stringstream ss;
            ss << "[" << ts << "]," << humi << "," << tempC << "," << (tempC * 1.8 + 32) << "|";
            std::string result = ss.str();
            char buf [MSG_SIZE];
            memset(buf, 0, sizeof(buf));
            strcpy(buf, result.c_str());
            // Serial.println("[INFO] inserting into queue");
            if (xQueueSend(msg_queue, &buf, 10) != pdTRUE) {
                Serial.println("measureTask: msg_queue is full");
            }

            // Display on serial (shift ctrl p -> "serial")
            Serial.println(result.c_str());
        }

        // wait 2 seconds MINIMUM, sensor needs a delay
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void setup () {
    Serial.begin(9600);

    // setup socket and dht11
    connectToWifi(client, WIFI_NETWORK, WIFI_PASSWORD, WIFI_TIMEOUT_MS);
    client.setInsecure();
    int trys = 0;
    while (unixtime == 0 && trys < 5) {
        trys++;
        unixtime = getUnixTime(client);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    if (unixtime == 0) {
        std::string s = "[ERROR] couldn't get unix time. Time will start at 0.|";
        char buf[MSG_SIZE];
        memset(buf,0, sizeof(buf));
        strcpy(buf, s.c_str());
        if (xQueueSend(msg_queue, &buf, 10) != pdTRUE) {
            Serial.println("setup: msg_queue is full");
        }
    }
    unixtime -= millis()/1000;
    client.setCACert(CA_CERT);
    client.connect(HOST, PORT);
    dht11.begin();

    // setup tasks
    Serial.println("[INFO] creating queue");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    msg_queue = xQueueCreate(msg_queue_len, sizeof(char[MSG_SIZE]));

    Serial.println("[INFO] starting tasks");
     // These two tasks needs big stack size
    xTaskCreatePinnedToCore(sendTask, "print from queue task", 8192, NULL, 1, NULL, app_cpu);
    xTaskCreatePinnedToCore(measureTask, "measure into queue task", 4096, NULL, 2, NULL, app_cpu);

    vTaskDelete(NULL);
}

void loop () {
}
