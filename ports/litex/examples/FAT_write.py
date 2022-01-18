# SD write using FAT
# creates a file, get it listed, then removes it

import uos as os
from umachine import SDCard
sd = SDCard()

fname = "testfile.txt"

vfs = os.VfsFat(sd)
os.mount(vfs, "/sd")
os.chdir("/sd")

prevlist = os.listdir()
print("Previous files:", prevlist)

with open(fname, "wt") as f:
  f.write("Hello!\n")

newlist = os.listdir()
print("New files:", newlist)
assert(prevlist != newlist)

#delete test file
os.remove(fname)
assert(os.listdir() == prevlist)



