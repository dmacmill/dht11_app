import ssl
from asyncio.streams import StreamReader, StreamWriter
from collections import deque


class SSLSocketProducer:
    def __init__(self, host: str, port: int, certfile: str, keyfile: str, fifo: deque):
        """Creates ssl/tls socket with default settings and certifications given.

        Args:
            host (str): host to bind to (should be "0.0.0.0")
            port (int): port to bind to (known to be 8090 on my esp32)
            certfile (str): full path to .crt file
            keyfile (str): full path to .key file
            fifo (deque): external deque object for handler to appendleft DHT data to
        """
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
                            # todo: simplify the message itself, make it just 1745970664,48,22,71.6
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
