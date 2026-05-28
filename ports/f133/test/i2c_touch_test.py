# (C)
# Apache License 2.0

#ILITEK ILI2511 Touch panel test
#see https://github.com/Infineon/touch-ctp-ili2511
import utime as time

I2C_ADDR = 0x41

from umachine import SoftI2C
i2c = SoftI2C(scl=12, sda=13, freq=100000)
scan = i2c.scan() #ILITEK IC reports 0x41, VGA DDC returns 0x37, 0x49, 0x50, 0x59
assert(I2C_ADDR in scan)

#Print firmware info
"""
MTB_CTP_ILI2511_CHIP_ID_POS                       (0U)
MTB_CTP_ILI2511_MAJOR_FIRMWARE_VER_POS            (1U)
MTB_CTP_ILI2511_MINOR_FIRMWARE_VER_POS            (2U)
MTB_CTP_ILI2511_RELEASE_FIRMWARE_VER_POS          (3U)
MTB_CTP_ILI2511_CUSTOMER_FIRMWARE_VER_BYTE_4_POS  (4U)
MTB_CTP_ILI2511_CUSTOMER_FIRMWARE_VER_BYTE_5_POS  (5U)
MTB_CTP_ILI2511_CUSTOMER_FIRMWARE_VER_BYTE_6_POS  (6U)
MTB_CTP_ILI2511_CUSTOMER_FIRMWARE_VER_BYTE_7_POS  (7U)
MTB_CTP_ILI2511_FIRMWARE_NUM_BYTES                (8U)
"""

assert(i2c.writeto(I2C_ADDR, b'\x40') == 1)
data = i2c.readfrom(I2C_ADDR, 8) # returns 0600000000000001
print("0x40:", data)
assert(len(data) == 8 and data[0]==0x06) 


assert(i2c.writeto(I2C_ADDR, b'\x20') == 1)
data = i2c.readfrom(I2C_ADDR, 9) # returns 0040004017290a0000
assert(len(data) == 9)
print("0x20:", data)


#Print touch info (INT goes low)
while True:
    assert(i2c.writeto(I2C_ADDR, b'\x10') == 1)
    time.sleep_ms(10)
    data = i2c.readfrom(I2C_ADDR, 32) #6*5+2
    assert(len(data) == 32)
    print("0x10:", data)
    for i in range(6): #coordinates range is 0 to 0x3FFF
        x = int.from_bytes(data[i*5+1:i*5+3], 'big') & 0x3FFF
        y = int.from_bytes(data[i*5+3:i*5+5], 'big') & 0x3FFF
        press = (data[i*5+1] & 0x80) != 0
        if press: print("ID", i, "at (", x, ",", y, ")")
        else: print("ID", i, "release")



