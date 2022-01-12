# TIME FUNCTIONS demo
#
# if in REPL use "Paste Mode" (Ctrl-E then Ctrl-D)
# to get the required precison
#
# SPDX-License-Identifier: BSD-2-Clause


import umachine
from utime import *

t0 = ticks_ms()
sleep_ms(1000)
t = ticks_diff(ticks_ms(), t0)
assert(t < 1001 and t > 999)
print("ms precision", t/1000)

t0 = ticks_us()
sleep_us(1000000)
t = ticks_diff(ticks_us(), t0)
assert(t < 1001000 and t > 999000)
print("us precision", t/1000000)

t0 = time_ns()
sleep_ms(1000)
t1 = time_ns()
if t1 < t0: t1 += 2**32 #ticks_diff doesn't work well
t = t1 - t0
assert(t < 1001000000 and t > 999000000)
print("ns precision", t/1000000000)

f = umachine.freq()
t0 = ticks_cpu()
sleep_us(1000000)
t = ticks_diff(ticks_cpu(), t0)
assert(t > 0.999*f and t < 1.001*f)
print("ticks_cpu precision", t/f)


