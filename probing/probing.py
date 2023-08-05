#! /usr/bin/env python3

import sys
from time import sleep
import board
import adafruit_mcp4728

MCP4728_ADDRESS = 0x64


def main():
    # i2c = board.I2C()
    # i = 0
    # while(True):
    #     i2c = board.I2C()
    #     # i2c.init()
    #     # i2c.
    #     mcp4728 = adafruit_mcp4728.MCP4728(i2c, MCP4728_ADDRESS)
    #     base_value = i % 2 ** 12
    #     mcp4728.channel_a.value = base_value
    #     mcp4728.channel_b.value = base_value
    #     mcp4728.channel_c.value = base_value
    #     mcp4728.channel_d.value = base_value
    #     i += 300
    #     sleep(1)

    
    spi = board.SPI()
    spi.configure()
    if not spi.try_lock():
        print('Cannot lock spi')
    data = [0xaa]
    spi.write(data)
    spi.unlock()
    return 0

if __name__ == "__main__":
    sys.exit(main())