# Copyright (c) 2023 Victor Suarez Rovere <suarezvictor@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause

DEBUG = 0 #enable this to run in a unix platform
FRAME_WIDTH = 640
FRAME_HEIGHT = 480

from framebuf import FrameBuffer, RGB32
if DEBUG:
  fbuf = FrameBuffer(bytearray(FRAME_WIDTH*FRAME_HEIGHT*4), FRAME_WIDTH, FRAME_HEIGHT, RGB32, FRAME_WIDTH)
else:
  import litex
  video = litex.Video(0)
  fbuf = FrameBuffer(video, video.width(), video.height(), RGB32, video.stride())
  video.flush() #makes sure screen gets completely clear

bgcolor = 0xFF0000
fbuf.fill(bgcolor)

def accel_ellipse_fill(x0, y0, x1, y1, rgba):
  global fbuf
  cx = (x0 + x1) // 2
  cy = (y0 + y1) // 2
  w = (x1-x0) // 2
  h = (y1-y0) // 2
  fbuf.ellipse(cx, cy, w, h, rgba, True)

def accel_rectangle_fill(x0, y0, x1, y1, rgba):
  global fbuf
  fbuf.fill_rect(x0, y0, x1-x0+1, y1-y0+1, rgba)

def accel_rectangle(x0, y0, x1, y1, rgba):
  fbuf.rect(x0, y0, x1-x0+1, y1-y0+1, rgba)

def draw_char(ch, x, y, width, height, rgba):
  if ch == ':':
    x0 = x+4*width
    y0 = y+2*height
    accel_ellipse_fill(x0, y0, x0+2*width, y0+4*height, rgba)
    y0 += 6*width
    accel_ellipse_fill(x0, y0, x0+2*width, y0+4*height, rgba)
    return

  if ch == '.':
    x0 = x+4*width
    y0 = y+12*height
    accel_ellipse_fill(x0, y0, x0+2*width, y0+2*height, rgba)
    return

  if ch < '0' or ch > '9':
    return
 
  """
    012345
     WWWW  0
    WW  WW 2
    WW  WW 4
     WWWW  6
    WW  WW 8
    WW  WW 10
     WWWW  12
  """

  class MaskPos:
    def __init__(self, mask, x, y, w, h):
        self.mask, self.x, self.y, self.w, self.h = mask, x, y, w, h

  segments=[
    MaskPos("1011011111", 1,  0, 4, 2), #A (top)
    MaskPos("1111100111", 4,  1, 2, 6), #B (top right)
    MaskPos("1101111111", 4,  7, 2, 6), #C (bottom right)
    MaskPos("1011011011", 1, 12, 4, 2), #D (bottom)
    MaskPos("1010001010", 0,  7, 2, 6), #E (bottom left)
    MaskPos("1000111011", 0,  1, 2, 6), #F (top left)
    MaskPos("0011111011", 1,  6, 4, 2), #G (middle)
  ]

  chindex = ord(ch)-ord('0')

  for i in range(len(segments)):
    s = segments[i]
    if s.mask[chindex] != '0':
      x0 = x+width*s.x
      y0 = y+height*s.y
      x1 = x0+width*s.w
      y1 = y0+height*s.h
      if x0 >= 0 and x1 < FRAME_WIDTH and y0 >= 0 and y1 < FRAME_HEIGHT:
        accel_rectangle_fill(x0, y0, x1, y1, rgba)

  corners=[
    MaskPos("1000111011", 0,  0, 2, 2), #top-left
    MaskPos("1011000111", 4,  0, 2, 2), #top-right
    MaskPos("0010011000", 4,  6, 2, 2), #center-right
    MaskPos("1001011011", 4, 12, 2, 2), #bottom-right
    MaskPos("1010001010", 0, 12, 2, 2), #bottom-left
    MaskPos("0010110001", 0,  6, 2, 2), #center-left
  ]

  for i in range(len(corners)):
    c = corners[i]
    if c.mask[chindex] != '0':
      x0 = x+width*c.x
      y0 = y+height*c.y
      x1 = x0+width*c.w
      y1 = y0+height*c.h
      if x0 >= 0 and x1 < FRAME_WIDTH and y0 >= 0 and y1 < FRAME_HEIGHT:
        accel_ellipse_fill(x0, y0, x1, y1, rgba)

  #optionally draw some lines around digits
  #accel_rectangle(x-2, y-2, x+width*6+2, y+height*15-5, rgba)


def draw_digits(digits, x0, y0, col, sz = 7):
  x = x0
  y = y0
  pos = [0,2,3, 5,7,8, 10,12,13, 15,17]
  digits = digits[:len(pos)]
  for i, d in enumerate(digits):
    x = x0 + pos[i]*4*sz
    draw_char(d, x, y, sz, sz, col)

import utime

def get_time():
  current_time = utime.localtime(utime.ticks_ms()//1000)
  return "{:02d}:{:02d}:{:02d}".format(current_time[3], current_time[4], current_time[5])

prev_time = get_time()

while True:
  utime.sleep_ms(100)
  current_time = get_time()
  if current_time != prev_time:
    print("time:", current_time)
    draw_digits(prev_time, 100, 100, bgcolor)
    draw_digits(current_time, 100, 100, 0x4080FF)
    prev_time = current_time
    if DEBUG:
      if not fbuf.debug_update():
        break
    else:
      video.flush()

