# Framebuffer using double buffers test.
# Copyright (c) 2022 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause


import litex
from framebuf import FrameBuffer, RGB32

video = litex.Video(0, True) #second argument enables double buffer
W, H, depth = video.width(), video.height(), video.bpp()
assert(depth == 32)

#at initialization, video output is from 1st buffer, returned buffer to write is the 2nd
fbuf1 = FrameBuffer(video, W, H, RGB32, video.stride())
video.flip() #flip to 1st buffer, to obtain addres of the 2nd buffer
fbuf2 = FrameBuffer(video, W, H, RGB32, video.stride())

#draw in the buffer 2 (not shown)
fbuf2.fill(0xFF8000) #orange
fbuf2.fill_rect(240,240,240,240,0xFF) #blue color
video.flip() #flip to start displaying 1st buffer

#draw in the buffer 1
fbuf1.fill(0xFF0000) #red
fbuf1.fill_rect(120,120,240,240,0xFF00) #green color
fbuf1.line(120,120,360-1,360-1,0) #black
video.flip() #flip to start displaying 2st buffer

#check timings are reasonable
import utime as time
time.sleep(2) #give monitor time to sync
t0 = time.ticks_ms()
for i in range(60):
  video.flip()
t = time.ticks_diff(time.ticks_ms(), t0)
fps = 60000/t
print("FPS:", fps)
assert(fps >= 30 and fps <= 120)




