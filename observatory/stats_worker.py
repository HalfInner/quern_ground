import sqlite3

import ctypes
from time import sleep
import time

driver_mcp3008 = ctypes.CDLL('/home/pi/programming/quern_ground/out/libquern_farm_api.so')
device = ctypes.c_char_p(b"/dev/spidev0.0")
driver = driver_mcp3008.configureDriver(device, 10000)

connection = sqlite3.connect('/tmp/database.db')
cur = connection.cursor()

while True:
    
    ts = time.monotonic()
    humidity = driver_mcp3008.getValue(driver, 7)
    cur.execute(
        "INSERT INTO stats (timestamp, probe, metric) VALUES (?, ?, ?)", 
        (ts, 'humidity', humidity)
    )

    connection.commit()
    time.sleep(0.1)

connection.close()

