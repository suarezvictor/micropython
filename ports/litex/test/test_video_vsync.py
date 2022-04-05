# Framebuffer double buffer and vertical synchronization test
# Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause


import litex
import math
import utime as time
from framebuf import FrameBuffer, RGB32

doublebuf = True #disable for testing tearing

video = litex.Video(0, doublebuf) #second argument enables double buffer
W, H, depth = video.width(), video.height(), video.bpp()
assert(depth == 32)

aspect = (4/3)/(16/9) #aspect ratio correction

fbuf = [FrameBuffer(video, W, H, RGB32, video.stride())]
i = video.flip() #flip buffer to obtain addres of the 2nd buffer
fbuf += [FrameBuffer(video, W, H, RGB32, video.stride())]

def drawline1(x, color): #alernative drawing
  fbuf[i].line(x, 0, W-x-1, H-1, color)

def drawline2(x, color):
  fbuf[i].line(W//2, H//2, W//2+int(math.sin(2*math.pi*x/60)*H/2*aspect), H//2-int(math.cos(2*math.pi*x/60)*H/2), color)


for x in range(W):
  drawline1(x-(2 if doublebuf else 1), 0) #clears previous drawing
  drawline1(x, 0xFFFFFF)
  i = video.flip() #flip, wait vsync & get next buf to draw

fbuf[0].fill(0); fbuf[1].fill(0);  #clear buffers

t0 = time.ticks_ms()

for x in range(600+1):
  drawline2(x-2, 0)
  drawline2(x, 0xFFFFFF)
  i = video.flip()

t = time.ticks_diff(time.ticks_ms(), t0)

fps = x*1000/t
print("FPS:", fps)
assert(fps >= 30 and fps <= 120)




