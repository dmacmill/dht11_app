#! /usr/bin/env python3

"""
Use SSLSocketProducer to produce into a queue within SSLSocketProducer, 
consume that queue from a DBConsumer object.
"""

import asyncio
from SSLSocketProducer import SSLSocketProducer
from collections import deque

HOST='0.0.0.0'
PORT=8090
CERTFILE = '/home/daniel/Documents/embedded_dev/esp32_projects/dht11_app/dht11_app_ssl_server/server.crt'
KEYFILE =  '/home/daniel/Documents/embedded_dev/esp32_projects/dht11_app/dht11_app_ssl_server/server.key'


async def main():
    fifo = deque()
    ssl_socket_producer = SSLSocketProducer(
        HOST, PORT, CERTFILE, KEYFILE, fifo
    )
    server = await asyncio.start_server(
        ssl_socket_producer.handler, 
        HOST,
        PORT,
        ssl=ssl_socket_producer.get_context()
    )

    async with server:
        await server.serve_forever()

asyncio.run(main())
