import litex
hmi = litex.USBHMI(0)
hmi.start()

while True:
  if hmi.process():
    print("x,y", hmi.mousex(), hmi.mousey())