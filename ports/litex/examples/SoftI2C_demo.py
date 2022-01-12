# Simple SoftI2C demo
#
# SPDX-License-Identifier: BSD-2-Clause

from umachine import Pin, SoftI2C
scl = Pin(0, Pin.OUT)
sda = Pin(1, Pin.OUT)
i2c = SoftI2C(scl, sda, freq=100000)
scan = i2c.scan()
assert(len(scan) != 0)
print(scan)
#registers on scanned devices
#can then be read and written



