#! /usr/bin/env python3

import ctypes
from time import sleep

# Load the shared library
driver_mcp3008 = ctypes.CDLL('./out/libquern_farm_api.so')

# Call the function from the C++ library
device = ctypes.c_char_p(b"/dev/spidev0.0")
driver = driver_mcp3008.configureDriver(device, 10000)

while(True):
    val = [driver_mcp3008.getValue(driver, ch) for ch in range(8)]
    print(val)
    sleep(0.01)
