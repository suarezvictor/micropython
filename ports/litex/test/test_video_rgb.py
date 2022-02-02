# Simple Framebuffer test using micropython-nano-gui
# https://github.com/peterhinch/micropython-nano-gui
# Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause

import sys
sys.path.append('./micropython-nano-gui')
import cmath, math

from color_setup import SSD, SSDRGB565, RGBWriter
from gui.core.colors import *
from gui.core.fplot import PolarGraph, PolarCurve
from gui.fonts import freesans20 as largefont, arial35 as hugefont
from gui.widgets.scale import Scale
from gui.widgets.textbox import Textbox

ssd = SSD()
ssd.fill(SSD.rgb(0xFF, 0x80, 0)) #orange background

wri = RGBWriter(ssd, hugefont, WHITE, RED)
tb = Textbox(wri, 250, 50, 200, 4, clip=False)
tb.append("Hello world from LiteX SoC builder!")

wri = RGBWriter(ssd, largefont, WHITE, BLACK)
scale = Scale(wri, 50, 50, width=320, height=90, pointercolor=RED)
scale.value(0.45)

def populate(rot, nmax):
    f = lambda theta : cmath.rect(1.15 * math.sin(5 * theta), theta) * rot # complex
    for n in range(nmax + 1): yield f(2 * cmath.pi * n / nmax)  # complex z

g = PolarGraph(wri, 200, 300, height=250, fgcolor=WHITE, gridcolor=LIGHTGREEN, bgcolor=BLUE)
curve = PolarCurve(g, GREEN, populate(1, 150))

#while ssd.debug_update(): pass

 

