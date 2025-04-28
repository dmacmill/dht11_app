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
                    chunks = msg.split("|")
                    formatted = []
                    for c in chunks:
                        if c:
                            ts = int(c[1:c.find(']')])
                            hum = int(c[c.find(" ")+1:c.find("%")])
                            tempc = float(c[c.find("%")+1:c.find("°C")])
                            tempf = float(c[c.find("°C")+2:c.find("°F")])
                            formatted.append((ts, hum, tempc, tempf))
                        print(f'{ts}  {hum}%  {tempc}°C  {tempf}°F')
                        self.fifo.appendleft((ts, hum, tempc))
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
