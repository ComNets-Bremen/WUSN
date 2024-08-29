import LoRaSender
import LoRaReceiver

from config import *
from machine import Pin, SPI, SoftSPI
from sx127x import SX127x
#from sx127x import *
from machine import lightsleep
from machine import deepsleep
from time import sleep

p16 = Pin(16, Pin.OUT)   # create output pin GPIO16 on J6 used to control sd card vcc

###p16.off()               # set pin to "off" (low) level sd card off which will cause reverse current flow
p16.on()                 # set pin to "on" (high) level sd card on

#p16.init(Pin.IN)         # Put the gpio in inputmode and create high impedance mode use this instead of p16.off()

device_spi = SoftSPI(
    baudrate = 200000, 
    polarity = 0, phase = 0, bits = 8, firstbit = SPI.MSB,
    sck = Pin(device_config['sck'], Pin.OUT, Pin.PULL_DOWN),
    mosi = Pin(device_config['mosi'], Pin.OUT, Pin.PULL_UP),
    miso = Pin(device_config['miso'], Pin.IN, Pin.PULL_UP))

#Pin(device_config['reset'], Pin.OUT, value=1)
lora = SX127x(device_spi, pins=device_config, parameters=lora_parameters)

#example = 'sender'
example = 'receiver'

print("going to sleep")
sleep(20)
print("lora on")
sleep(10)
lora.sleep()  # LoRa on sleep mode
print("lora off")
#deepsleep(20000)
#lightsleep(20000)

