#see https://github.com/miketeachman/micropython-i2s-examples/blob/master/examples/wavplayer.py
import uos as os
from umachine import I2S
from umachine import SDCard

sd = SDCard()
vfs = os.VfsFat(sd)
os.mount(vfs, "/sd")
os.chdir("/sd")

wav = open("wav_music-16k-32bits-mono.wav", "rb")
_ = wav.seek(44) #skip header
samples = bytearray(1024*4)

def i2s_callback(s):
	num_read = wav.readinto(samples)
	if num_read == 0:
		audio_out.irq(None)
		print("done")
		return
	#32 to 24 bits conversion
	audio_out.shift(buf=samples, bits=32, shift=-8)
	audio_out.write(samples) #send samples

audio_out = I2S(0, sck=-1, ws=-1, sd=-1, mode=I2S.TX,
	bits=24, format=I2S.MONO, rate=44100, ibuf=len(samples))

audio_out.irq(i2s_callback)

