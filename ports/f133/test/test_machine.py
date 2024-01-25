# Simple Machine test.
# Copyright (C) 2024 Victor Suarez Rovere <suarezvictor@gmail.com>
#
# SPDX-License-Identifier: BSD-2-Clause


# Display sys_clk_freq
import umachine
print("sys_clk_freq: {}MHz".format(umachine.freq()/1e6))

# Display free memory
import gc
gc.collect()
print("free memory: {}KiB".format(gc.mem_free()/1024))
