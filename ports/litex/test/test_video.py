# Simple Framebuffer test.
# Requirements:
#  - For 16-bit mode, format at SoC generation should be "rgb565"
# Copyright (c) 2021 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause


import litex
from framebuf import FrameBuffer, RGB32, RGB565

video = litex.Video(0)
W, H, depth = video.width(), video.height(), video.bpp()
print("Resolution: %dx%d, bpp %d" % (W, H, depth))
assert(depth == 32 or depth == 16)
fbuf = FrameBuffer(video, W, H, RGB32 if depth == 32 else RGB565, video.stride())

fbuf.fill(0xFF8000) #orange, 0xFC00 for 16-bit
fbuf.fill_rect(240,240,240,240,0xFF) #full blue, 0x001F for 16-bit
fbuf.fill_rect(120,120,240,240,0xFF00) #full green, 0x07E0 for 16-bit
fbuf.fill_rect(240,240,120,120,0xFF0000) #full red, 0xF800 for 16-bit
fbuf.line(120,120,360-1,360-1,0) #black
