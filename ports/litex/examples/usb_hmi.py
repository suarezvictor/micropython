import litex
from framebuf import FrameBuffer, RGB32

hmi = litex.USB_HMI(13,12,14,15) #D+/D- pin pairs
hmi.start()
video = litex.Video(0)
fbuf = FrameBuffer(video, video.width(), video.height(), RGB32, video.stride())

while True:
  if hmi.process():
    x, y, dx, dy = hmi.mousex(), hmi.mousey(), hmi.dx(), hmi.dy()
    fbuf.fill_rect(x, y, abs(dx), abs(dy), 0xFF if hmi.buttons() else 0xFF00)

