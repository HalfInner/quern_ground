#! /usr/bin/env python3

import ctypes
from time import sleep

# Load the shared library
my_library = ctypes.CDLL('./out/libquern_farm_api.so')

# Call the function from the C++ library
device = ctypes.c_char_p(b"/dev/spidev0.0")
driver = my_library.configureDriver(device, 10000)

while(True):
    val = my_library.getValue(driver, 7)
    val = my_library.getValue(driver, 8)
    print(val)
    sleep(0.01)
