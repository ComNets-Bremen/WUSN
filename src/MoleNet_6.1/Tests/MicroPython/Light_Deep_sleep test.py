import machine
import utime

print('idle')
#idle 10s
utime.sleep(10)

print('lightsleep')
#lightsleep 20s
machine.lightsleep(10000)

print('deepsleep')
utime.sleep(1)
# Put the ESP32 into deep sleep for 30s
machine.deepsleep(10000)