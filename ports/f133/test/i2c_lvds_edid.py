# Copyright (C) 2026 Victor Suarez Rovere <suarezvictor@gmail.com>
# SPDX-License-Identifier: AGPL-3.0-only

from umachine import SoftI2C

#in the CPU board, SCL2 is PE0, SDA2 is PE1 (maps to display pins 6 and 7)
i2c = SoftI2C(scl=0, sda=1, freq=100000)
scan = i2c.scan() #LVDS DDC returns 0x50 (tested N156BGE-L21 display)

assert(0x50 in scan)
data = i2c.readfrom_mem(0x50, 0, 128) #read from position 0
assert(len(data) == 128 and sum(data) % 256 == 0)

print("Model:", data[0x4d:0x4d+13].decode("utf-8"))
print('Screen size: %d x %dcm' % (data[21], data[22]))

#resolutions
for n in range(1, 5):
  if n == 1: print("\nNative resolutions:")
  i = n * 18 + 36
  if data[i] != 0 or data[i+1] != 0:
    b = data[i:i+18]
    h = b[2] | ((b[4]&0xF0) << 4)
    v = b[5] | ((b[7]&0xF0) << 4)
    clk = (b[0] | (b[1]<<8)) / 100.0
    print('\t%4d x %4d\t%3.1f MHz pixel clock' % (h, v, clk))

