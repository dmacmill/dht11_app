# DHT11 ESP32 Project

This project demonstrates how to interface a DHT11 temperature and humidity sensor with an ESP32 microcontroller. The application reads data from the sensor and displays it via serial output.

## Features
- Retrieves current epoch time from worldtimeapi.org
- Reads temperature and humidity data from the DHT11 sensor.
- Outputs data to the serial monitor.
- Periodically sends data in batches to the specified HOST and PORT ssl/tls socket. Data is in the rough form:

`[1746119310],57,24.1,75.38|[1746119312],57,24.4,75.92|[1746119314],58,24.7,76.46|[1746119316],58,24.6,76.28|[1746119318],58,24.6,76.28|[1746119320],58,24.4,75.92|[1746119322],58,24.7,76.46|[1746119324],57,24.1,75.38|`

## Prerequisites
- ESP32 feather development board.
- DHT11 sensor with data pin plugged to pin 21.
- Arduino IDE or PlatformIO installed on your system.

## Setup Instructions
1. Clone this repository to your local machine:
    ```bash
    git clone https://github.com/your-repo/dht11_app.git
    cd dht11_app
    ```

2. **Add your `credentials.hpp` file**:  
    Create a file named `credentials.hpp` in the `include/` folder. This file should contain any necessary credentials (e.g., Wi-Fi SSID and password). Below is an example template for the file:
    ```cpp
    #ifndef CREDENTIALS_HPP
    #define CREDENTIALS_HPP

    #define WIFI_NETWORK "your-ssid"
    #define WIFI_PASSWORD "your-password"
    #define WIFI_TIMEOUT_MS 20000

    #define HOST "192.168.x.xxx"
    #define PORT 8090

    const char * CA_CERT = \
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDizCCAnOgAwIBAgIUY3E3C9kfxcYD7Zpc88Sn9955O3wwDQYJKoZIhvcNAQEL\n" \
    "BQAwVToiliKEturTLESCVVMxCzAJBgNVBAgMAk1BMSEwHwYDVQQKDBhJbnRlcm5l\n" \
    "dCBXaWRnaXRzIFB0eSBMdGQxFjAUBgNVBAMMDTE5Mi4xNjguMS4xNTIwHhcNMjUw\n" \
    "NDE5MjMyNDA5WhcNMjYwNDE5MjMyNDA5WjBVMQswCQYDVQQGEwJVUzELMAkGA1UE\n" \
    "CAwCTUExITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDEWMBQGA1UE\n" \
    "AwwNMTkyLjE2OC4xLjE1MjCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
    "ALRXTOVDAB3OFWA31T4R9s+vpbjPXIot6NJhVdYPD/Ay+uaTfhiD3gGwyTX4X+8U\n" \
    "bq0KmPXosZdtN/WdTxiL3+gGkf+Yaa2pmslJPN5yKvmG/qG95nDtevKBfyblnYGG\n" \
    "MNHSIBWnBmP+mv0+wndV2seAENMmskibidinhNfkXf24SZNAz30rY+lr/lz6tl87\n" \
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

    #endif // CREDENTIALS_HPP
    ```

3. Open the project in your preferred IDE (e.g., Arduino IDE or PlatformIO).

4. Connect your ESP32 board to your computer via USB.

5. Compile and upload the code to your ESP32.

6. Open the serial monitor to view the temperature and humidity readings.

## Notes
- Ensure the DHT11 sensor is connected to the correct GPIO pin as specified in the code.
- The `credentials.hpp` file is excluded from version control to protect sensitive information.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments
- DHT sensor library by Adafruit.
- ESP32 development community.
