# Simple GPIO demo using litex module
# loopback test between GPIO3 and GPIO4
# *DEPRECATED* use umachine.Pin
#
# SPDX-License-Identifier: BSD-2-Clause

from litex import PIN
inp = PIN(3, mode=0)
outp = PIN(4, mode=1)
outp.low()
a = inp.value()
outp.high()
b = inp.value()
assert(a != b)
print(a, b)


