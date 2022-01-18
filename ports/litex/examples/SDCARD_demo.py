#print name and contents of first file
#works in 4-bit or SPI mode

import uos as os

if os.getcwd() != "/sd": #detects if the card is already mounted
    from umachine import SDCard
    sd = SDCard()
    print("size (GB):", sd.info()[0]/1e9)
    vfs = os.VfsFat(sd)
    os.mount(vfs, "/sd")
    os.chdir("/sd")

ls = os.listdir()
print(ls)

with open(ls[0], "rt") as f: #assumes text file
  for l in f: #iterate lines
    print(l.rstrip())

