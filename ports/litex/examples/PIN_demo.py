# Simple GPIO demo usung Pin class
# loopback test between GPIO3 and GPIO4
#
# SPDX-License-Identifier: BSD-2-Clause

from umachine import Pin
inp = Pin(3, Pin.IN)
outp = Pin(4, Pin.OUT)
outp.off()
a = inp.value()
outp.on()
b = inp.value()
assert(a != b)
print(a, b)


