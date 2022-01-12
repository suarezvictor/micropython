# ADC128S022 SPI ADC demo
# Copyright (C) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause

import umachine as machine
import litex
import ustruct as struct

SOFTSPI = False #select SoftSPI or hardware SPI
CSpin = 3
SPS = 50000 #samples per second, 50K to 200K

if SOFTSPI:
  spi = machine.SoftSPI(baudrate=SPS*16, sck=CSpin+1, mosi=CSpin+2, miso=CSpin+3)
else:
  spi = machine.SPI(0, baudrate=SPS*16)

cs = litex.PIN(CSpin, 1) #mode 1=OUT

cs.low() #CS control is automatic in hardware spi (no need for this)
spi.write(b'\x00') #set next channel to 0
cs.high()

for chan in range(8):
  cs.low() #only for SoftSPI
  nextchan = (chan+1) % 8
  buf = spi.read(2, nextchan*8) #read 2 bytes while sending next channel (2nd byte ignored)
  cs.high()
  value, = struct.unpack('>H', buf) #unsigned 16-bit big endian
  print("channel", chan, "=", value)


