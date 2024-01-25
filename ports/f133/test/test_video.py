import f133
from framebuf import FrameBuffer, RGB32

video = f133.Video(0)
W, H, depth = video.width(), video.height(), video.bpp()
print("Resolution: %dx%d, bpp %d" % (W, H, depth))
assert(depth == 32)
fbuf = FrameBuffer(video, W, H, RGB32, video.stride())

fbuf.fill_rect(120,120,240,240,0x00FF00)     #full green, 0x07E0 for 16-bit
fbuf.fill_rect(240,240,240,240,0x0000FF)     #full red, 0x001F for 16-bit
fbuf.ellipse(300,300,100,100,0xFF0000,True)  #full blue, 0xF800 for 16-bit
fbuf.line(480-1,120,120,480-1,0x00FFFF)      #yellow

