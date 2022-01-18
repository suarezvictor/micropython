# Simple Timer demo
#
# SPDX-License-Identifier: BSD-2-Clause

from umachine import Timer
from litex import LED

#waits
Timer(0).init(freq=0.5, mode=Timer.ONE_SHOT, callback=lambda t:LED(0).toggle())

#loops forever
Timer(0).init(period=1000, mode=Timer.PERIODIC, callback=lambda t:LED(0).toggle())

