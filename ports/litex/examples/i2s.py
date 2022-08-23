#see https://github.com/miketeachman/micropython-i2s-examples/blob/master/examples/wavplayer.py
import math
import ustruct
from umachine import I2S
from umachine import Pin

sck_pin = Pin(1)   # Serial clock output
ws_pin = Pin(1)    # Word clock output
sd_pin = Pin(1)    # Serial data output

i2s_bits = 24
i2s_freq = 44100

# create a buffer containing the pure tone samples
samples_per_cycle = i2s_freq // 172
sample_size_in_bytes = 2 if i2s_bits == 16 else 4
samples = bytearray(samples_per_cycle * sample_size_in_bytes)
#print("bytearray len", len(samples), samples_per_cycle, sample_size_in_bytes)
fullscale = 2**(i2s_bits-1)
fmt = "<h" if i2s_bits == 16 else "<l"


for i in range(samples_per_cycle):
    sample = int((fullscale - 1) * math.sin(2 * math.pi * i / samples_per_cycle))
    ustruct.pack_into(fmt, samples, i * sample_size_in_bytes, sample)
        


def i2s_callback(s):
	global samples
	num_written = audio_out.write(samples) # send samples
	#print("written", num_written)


audio_out = I2S(0,
                sck=sck_pin, ws=ws_pin, sd=sd_pin,
                mode=I2S.TX,
                bits=i2s_bits,
                format=I2S.MONO,
                rate=i2s_freq,
                ibuf=16384)

audio_out.irq(i2s_callback) #non-blocking

