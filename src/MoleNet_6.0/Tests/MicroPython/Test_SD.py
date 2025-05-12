#This program test SD Card functionality
from machine import Pin
import machine
import os
import utime

sd = machine.SDCard(slot=2, sck=12, miso=13, mosi=11, cs=10, freq=200_000)
utime.sleep(1)
os.mount(sd, '/sd') # mount

with open("/sd/test.text","w") as f:
    f.write("This is a test")

with open("/sd/test.text","r") as f:
    for row in f:
        print(row)
        
print(os.listdir('/sd/'))    # list directory contents

os.umount('/sd')    # eject
print("Success")