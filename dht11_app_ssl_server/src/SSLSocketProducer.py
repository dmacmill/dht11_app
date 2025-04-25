import asyncio
from asyncio.streams import StreamReader, StreamWriter
import ssl
from collections import deque
import time


"""SSLSocketProducer initializes an ssl.Context from given certfile and keyfile
and creates a handler function to receive and produce msgs into a queue.
"""
class SSLSocketProducer:
    def __init__(self, host: str, port: int, certfile: str, keyfile: str, fifo: deque):
        self._host = host # not needed here for asyncio streams
        self._port = port # not needed here for asyncio streams
        self._certfile = certfile
        self._keyfile = keyfile
        self.fifo = fifo

        self.context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        self.context.load_cert_chain(certfile=self._certfile,
                                     keyfile=self._keyfile)

    async def handler(self, reader: StreamReader, writer: StreamWriter):
        addr = writer.get_extra_info('peername')
        # print(f'[INFO] Connection from {addr}')

        try:
            while True:
                msg_bytes = await reader.readline()
                if msg_bytes:
                    msg = msg_bytes.decode('utf-8')
                    print(f'\n----\nReceived: {msg}')
                    timestamp = time.time()
                    humidity = int(msg[10:12])
                    tempC = float(msg[msg.find('Temperature: ')+13:msg.find('°C')])
                    tempF = float(msg[msg.find('~')+3:msg.find('°F')])
                    print(f'{timestamp}  {humidity}%  {tempC}°C  {tempF}°F')
                    self.fifo.appendleft((timestamp, humidity, tempC))
                else:
                    break
        except Exception as e:
            print(f"[ERROR]: {e}")
        finally:
            writer.close()
            await writer.wait_closed()
            # print("closed connection")

    def get_context(self) -> ssl.SSLContext:
        return self.context
