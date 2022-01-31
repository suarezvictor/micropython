# Simple Framebuffer test.
# Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
sys.path.append('./micropython-nano-gui')
from color_setup import SSD
from gui.core.writer import Writer
from gui.core.colors import *
from uctypes import bytearray_at, addressof

#FIXME: move to color_setup.py
class RGBWriter(Writer):
    def __init__(self, device, font, fgcolor=WHITE, bgcolor=None, use_default_font=False, verbose=False):
        super().__init__(device, font, verbose)
        self.fgcolor = fgcolor
        self.bgcolor = bgcolor
        self.use_default_font = use_default_font

    def _printchar(self, char, invert=False, recurse=False):
        s = self._getstate()
        self._get_char(char, recurse)
        if self.glyph is None:
            return  # All done
        if self.use_default_font:
            self.device.text(char, s.text_col, s.text_row, self.bgcolor if invert else self.fgcolor)
            s.text_col += 8+1 #fixed width plus space
        else:
            glyph, ht, wd = self.font.get_ch(char)
            div, mod = divmod(wd, 8)
            gbytes = div + 1 if mod else div  # No. of bytes per row of glyph
            data = glyph[(wd - 1) // 8]  # Last byte of row 0
            if self.bgcolor is not None:
                for row in range(ht):  # Glyph row
                    for col in range(wd -1, -1, -1):  # Glyph column
                        gbyte, gbit = divmod(col, 8)
                        data = glyph[row * gbytes + gbyte]
                        self.device.pixel(s.text_col+col, s.text_row+row, self.fgcolor if data & (1 << (7 - gbit)) else self.bgcolor)
            else:
                for row in range(ht):  # Glyph row
                    for col in range(wd -1, -1, -1):  # Glyph column
                        gbyte, gbit = divmod(col, 8)
                        data = glyph[row * gbytes + gbyte]
                        if data & (1 << (7 - gbit)):  # Pixel is lit (1)
                            self.device.pixel(s.text_col+col, s.text_row+row, self.fgcolor)
            s.text_col += wd
        self.cpos += 1

ssd = SSD()


def test_simple(fbuf):
  fbuf.fill(0xFC00) #orange background
  fbuf.text('MicroPython!', 0, 0, 0xffff)
  fbuf.hline(0, 9, 96, 0xffff)
  fbuf.fill_rect(240,240,240,240,0x001F) #full blue
  fbuf.fill_rect(120,120,240,240,0x07E0) #full green
  fbuf.fill_rect(240,240,120,120,0xF800) #full red
  fbuf.line(120,120,360-1,360-1,0) #black

def test2(fbuf):
  from gui.widgets.scale import Scale
  from gui.fonts import freesans20 as font
  wri = RGBWriter(ssd, font)
  wri.set_clip(True, True, False)
  scale = Scale(wri, 20, 20, width = 124, pointercolor=WHITE)
  scale.value(1)
  scale.show()

test_simple(ssd)
test2(ssd)

with open("fb16.data", "wb") as f:
  f.write(ssd)

def rgb565torgba32(p):
  return (int((p & 0x001F)*255/31) << 16) | (int((p & 0x07E0)*255/63) << 3) | (int((p & 0xF800)*255/31) >> 11) | 0xFF000000

import array
fb32 = array.array('I', [rgb565torgba32(p) for p in ssd.buffer])
with open("fb32.data", "wb") as f:
  f.write(fb32)

