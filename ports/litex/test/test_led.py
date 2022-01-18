# Simple LED demo using litex module
# SPDX-License-Identifier: BSD-2-Clause

from litex import LED
led = LED(0)
led.on()
led.off()
led.toggle()
led.toggle()

