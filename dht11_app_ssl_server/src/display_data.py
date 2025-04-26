#! /usr/bin/env python3

import argparse
import matplotlib.pyplot as plt
import sqlite3

DB_FILE = "../data/sensor_data.db"

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("db_file", 
                        help="sqlite .db file to access, table sensor_data with schema (timestamp, humidity, temp)",
                        type=argparse.FileType('r'))
    args = parser.parse_args()

    conn = sqlite3.connect(args.db_file.name)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM sensor_data")
    data = cursor.fetchall()

    x_axis = list(range(1, len(data)+1))
    hlist = [x[1] for x in data]
    tlist = [x[2] for x in data]


    plt.figure(figsize=(20, 10))
    plt.plot(x_axis, hlist, label="humidity (%)", color="green")
    plt.plot(x_axis, tlist, label="temperature (deg C)", color="red")
    plt.xlabel("measurement")
    plt.ylabel("deg C / humidity %")
    plt.legend()
    plt.show()