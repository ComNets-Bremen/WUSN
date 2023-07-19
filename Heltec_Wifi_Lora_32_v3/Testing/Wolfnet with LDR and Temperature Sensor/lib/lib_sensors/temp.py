from machine import Pin
from time import sleep
import onewire, ds18x20, random

sensor_pin =(Pin(33, Pin.IN))
temp_sensor = ds18x20.DS18X20(onewire.OneWire(sensor_pin))
temp_data = temp_sensor.scan()[0]

def get_temp():
    sleep(1)
    # return round(temp_sensor.read_temp(temp_data), 2)
    return random.randint(2000, 2300)/100