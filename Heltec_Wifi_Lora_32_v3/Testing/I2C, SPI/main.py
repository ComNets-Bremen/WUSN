""" import machine, onewire, ds18x20, time, lib.oled_config as oled_config

ds_pin = machine.Pin(36)
ds_sensor = ds18x20.DS18X20(onewire.OneWire(ds_pin))
oled = oled_config.initalize_oled()

roms = ds_sensor.scan()
print('Found DS devices: ', roms)

while True:
  ds_sensor.convert_temp()
  time.sleep_ms(750)
  for rom in roms:
    print(rom)
    print(ds_sensor.read_temp(rom))
    oled.new_text(str(ds_sensor.read_temp(rom)) + ' C', 0, 0)

  time.sleep(5) """

from machine import Pin
from time import sleep
from lib.oled_config import initalize_oled
import dht

oled = initalize_oled()

sensor_pin =(Pin(35, Pin.IN))
dht_sensor = dht.DHT22(sensor_pin)

while True:
    dht_sensor.measure()
    oled.new_text(str(dht_sensor.temperature()) + ' C', 0, 0)
    oled.new_text(str(dht_sensor.humidity()) + ' %', 0, 10)
    sleep(5)