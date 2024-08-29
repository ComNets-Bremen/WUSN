#Power measurement using LoRa sleep
#NOTE: add files sx127x.py; config.py; LoRaSender.py; LoRaReceiver.py
import LoRaSender
import LoRaReceiver

from config import *
from machine import Pin, SPI, SoftSPI
from sx127x import SX127x
from machine import lightsleep
from machine import deepsleep
from time import sleep

device_spi = SoftSPI(
    baudrate = 200000, 
    polarity = 0, phase = 0, bits = 8, firstbit = SPI.MSB,
    sck = Pin(device_config['sck'], Pin.OUT, Pin.PULL_DOWN),
    mosi = Pin(device_config['mosi'], Pin.OUT, Pin.PULL_UP),
    miso = Pin(device_config['miso'], Pin.IN, Pin.PULL_UP))

#Pin(device_config['reset'], Pin.OUT, value=1) #If external pullup is used comment this line
lora = SX127x(device_spi, pins=device_config, parameters=lora_parameters)

#example = 'sender'
example = 'receiver'
print("lora on")
sleep(20)
lora.sleep()  # call the method on the instance
print("lora off")
# print("going to deepsleep") #Either use lightsleep or deepsleep 
# deepsleep(20000) #Either use lightsleep or deepsleep
print("going to lightsleep") #Either use lightsleep or deepsleep 
lightsleep(10000)