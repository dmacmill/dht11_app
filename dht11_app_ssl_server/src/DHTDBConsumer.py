import os
import time
import sqlite3

from collections import deque

DB_NAME='../data/sensor_data.db'
MAX_BATCH_SIZE = 20
FLUSH_INTERVAL = 60 # seconds

class DHTDBConsumer:
    def __init__(self, fifo: deque):
        """Takes external fifo deque filled from a producer. Contains handler for filling sqlite server
        periodically with that.

        Args:
            fifo (collections.deque): external fifo deque filled from a producer of DHT data.
        """
        self.fifo = fifo

    def _setup_connection(self):
        """Connect to sqlite3 with path from DB_NAME.
        """
        db_abspath = os.path.abspath(DB_NAME)
        self.conn = sqlite3.connect(db_abspath)
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
        """After connecting with sqlite db file, consume from fifo queue and insert into sensor_data table.
        """
        self._setup_connection()

        batch = []
        last_flush = time.time()
        while True:
            if len(self.fifo) > 0:
                while(len(self.fifo) > 0):
                    batch.append(self.fifo.pop())
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
            time.sleep(1)
