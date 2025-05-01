#! /usr/bin/env python3
import socket
import ssl
import time

HOST = '0.0.0.0'
PORT = 8090
CERTFILE = '/home/daniel/Documents/embedded_dev/esp32_projects/dht11_app/dht11_app_ssl_server/server.crt'
KEYFILE = '/home/daniel/Documents/embedded_dev/esp32_projects/dht11_app/dht11_app_ssl_server/server.key'

# Create the underlying TCP socket
bind_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
bind_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
bind_socket.bind((HOST, PORT))
bind_socket.listen(5)
print(f"Listening on {HOST}:{PORT}...")

# Create SSL context
context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)

# Optional: restrict to TLS 1.2 only
# context.options |= ssl.OP_NO_TLSv1
# context.options |= ssl.OP_NO_TLSv1_1
# context.minimum_version = ssl.TLSVersion.TLSv1_2
# context.maximum_version = ssl.TLSVersion.TLSv1_2

# Optional: allow only TLS 1.3 (Python 3.7+ and OpenSSL 1.1.1+)
# context.minimum_version = ssl.TLSVersion.TLSv1_3
# context.maximum_version = ssl.TLSVersion.TLSv1_3

# Load cert and key
context.load_cert_chain(certfile=CERTFILE, keyfile=KEYFILE)

past = time.time()

try:
    while True:
        # Accept raw socket connection
        client_sock, addr = bind_socket.accept()
        print(f"Connection from {addr}")

        # Wrap the socket with SSL
        try:
            with context.wrap_socket(client_sock, server_side=True) as ssl_sock:
                data = ssl_sock.recv(65536)
                if len(data) > 0:
                    data_str = data.decode('utf-8')
                    now = time.time()
                    delta = now - past

                    chunks = data_str.split("|")
                    formatted = []
                    for c in chunks:
                        if c:
                            if c.find('[ERROR]') != -1:
                                print(f"{c}")
                            else:
                                print(f"{c}")
                                ts_str = c[1:c.find(']')]
                                (hum_str, tempc_str, tempf_str) = c.split("]")[1].split(",")[1:]
                                formatted.append(
                                    (int(ts_str), int(hum_str), float(tempc_str), float(tempf_str))
                                )

                    print("\n\n")
                    print(f"------{now}")
                    print(f"------{delta}")
                    print(f"Received: {formatted}")
                    print("-----------------------\n")
                    past = now
        except ssl.SSLError as e:
            print(f"SSL error: {e}")
        except Exception as e:
            print(f"Error: {e}")
finally:
    print("shutting down")
    bind_socket.close()
