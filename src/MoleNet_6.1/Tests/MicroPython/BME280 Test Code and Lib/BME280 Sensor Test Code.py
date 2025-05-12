from machine import Pin,SoftI2C
from time import sleep
import BME280

# ESP32 - Pin assignment
i2c = SoftI2C(scl=Pin(8), sda=Pin(9), freq=10000)

bme = BME280.BME280(i2c=i2c)
temp = bme.temperature
hum = bme.humidity
pres = bme.pressure
print('Temperature: ', temp)
print('Humidity: ', hum)
print('Pressure: ', pres)
