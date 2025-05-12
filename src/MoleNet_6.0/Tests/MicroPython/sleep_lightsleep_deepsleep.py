#Power measurement 
from machine import deepsleep
from machine import lightsleep
from time import sleep

print('Im awake. Going to sleep in 10 seconds')
sleep(20)
# print('Going to 10 sec light sleep now') #Either use lightsleep or deepsleep 
# lightsleep(20000) #Either use lightsleep or deepsleep
sleep(10)
print('Going to 10 sec deep sleep now')
deepsleep(10000)

