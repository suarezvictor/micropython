#Copyright (C) 2024 Victor Suarez Rover <suarezvictor@gmail.com>
#LICENSE: AGPL-3.0

from umachine import SoftI2C

#in the F133 board, SCL is PE12, SDA is PE13 (maps to VGA pin 15 and 12)
i2c = SoftI2C(scl=12, sda=13, freq=100000)
scan = i2c.scan() #VGA DDC returns 0x37, 0x49, 0x50, 0x59

assert(0x50 in scan)
bytes = i2c.readfrom(0x50, 128)
assert(len(bytes) == 128 and sum(bytes) % 256 == 0) #CRC check

for n in range(1, 5):
  i = n * 18 + 36
  if bytes[i] != 0 or bytes[i+1] != 0:
    b = bytes[i:i+18]
    h = b[2] | ((b[4]&0xF0) << 4)
    v = b[5] | ((b[7]&0xF0) << 4)
    clk = (b[0] | (b[1]<<8)) / 100.0
    print('Native resolution: %dx%d, pixel clock %3.1f MHz' % (h, v, clk))

print('Screen size: %dx%dcm' % (bytes[21], bytes[22]))

