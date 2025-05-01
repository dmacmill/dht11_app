# DHT11 App SSL Server

## Overview

The `dht11_app_ssl_server` is a host-side TLS server designed to work with the ESP32 application in the `dht11_app` project. It facilitates secure communication between the ESP32 device and the host by acting as the TLS endpoint. The ESP32 sends temperature and humidity data from the DHT11 sensor to this server, ensuring data integrity and confidentiality.

## Architecture

The server is structured as a simple **Producer/Consumer** model:

- **SSLSocketProducer**: The ESP32 device acts as the producer, sending sensor data (timestamp, humidity, and temperature) to the server in the main thread over a secure TLS connection. Batches of this data are collected, processed, printed out, and stored in a fifo queue modeled as a `collections.deque` for later consumers.
- **DHTDBConsumer**: The separate Consumer thread receives the data from the fifo queue (`collections.deque` object) and periodically sends all the data in batches to the specified sqlite3 file database. This clears the fifo queue.

The server uses Python's `ssl` module to establish a secure connection and listens on a specified port for incoming data from the ESP32.

## Features

- Secure TLS communication with the ESP32.
- Lightweight and easy-to-configure server.
- Designed for real-time data transfer from the DHT11 sensor.

## Usage

1. If you haven't already, produce .crt and .key files (don't share the key!) by calling `openssl req -x509 -newkey rsa:2048 -nodes -keyout server.key -out server.crt -days 365` in the dht11_app_ssl_server directory.
2. When prompted for name, use your local IP. Use whatever other settings you'd like and continue until you have your certificate and key. 
3. Start `./ssl_server` on the host machine.
4. Configure the ESP32 application to connect to the server's IP address and port.
5. Monitor the incoming data on the server.

## `display_data.py`

The `display_data.py` script is a utility for visualizing the data stored by `ssl_server.py`. It reads the data from the server's output and displays it in a user-friendly format.

Run `./display_data.py <path_to_file>` and a matplotlib window should show you your data.


## Requirements

- Python 3.7+
- ESP32 with the `dht11_app` firmware. See instructions in dht11_app folder's README for more to do with that.

## Notes

- Ensure the server's certificate and private key are properly configured for TLS.
- The ESP32 must trust the server's certificate to establish a secure connection.
