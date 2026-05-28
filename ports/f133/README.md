MicroPython port to the Alwinner F133 SoC
================================================

Supported features:  
- REPL (Python prompt) over UART  
- Minimal umachine support (reset, freq, identifier)
- Time functions (except RTC)  
- Video Framebuffer (see test/test_video.py)  
- Software I2C
  
Execute examples in RAW-REPL mode (through Pyboard)
---------------------------------------------------

With Micropython firmware loaded on the SoC, tests can be run with:

```bash
$ cd test
$ python3 ../../../tools/pyboard.py -d /dev/ttyUSBX test_machine.py
```


