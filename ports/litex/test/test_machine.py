import gc
import umachine
print("Machine identifier:", umachine.identifier())
print("Clock frequency:", umachine.freq()/1.e6, "MHz")
gc.collect()
print("Free memory:", gc.mem_free()/1024, "KiB")

