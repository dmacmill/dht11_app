import threading
import time
import sqlite3

from collections import deque

DB_NAME='sensor_data.db'
MAX_BATCH_SIZE = 50
FLUSH_INTERVAL = 60 # seconds

class DHTDBConsumer:
    def __init__(self, fifo):
        self.msg_queue = fifo
        

    def _setup_connection(self):
        self.conn = sqlite3.connect(DB_NAME)
        self.cursor = self.conn.cursor()
        self.cursor.execute('''
            CREATE TABLE IF NOT EXISTS sensor_data (
                                timestamp REAL,
                                temperature REAL,
                                humidity REAL
                            )
        ''')
        self.conn.commit()

    def handler(self):
        self._setup_connection()

        batch = []
        last_flush = time.time()
        while True:
            if len(self.msg_queue) > 0:
                batch.append(self.msg_queue.pop())
            # time or size-based flush
            if (len(batch) >= MAX_BATCH_SIZE or time.time() - last_flush >= FLUSH_INTERVAL):
                if batch:
                    self.cursor.executemany(
                        "INSERT INTO sensor_data (timestamp, temperature, humidity) VALUES (?, ?, ?)",
                        batch
                    )
                    self.conn.commit()
                    print(f"Inserted batch of {len(batch)} records.")
                    batch.clear()
                    last_flush = time.time()
