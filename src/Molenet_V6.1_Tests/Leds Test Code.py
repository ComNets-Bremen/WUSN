from machine import Pin
import utime


led1 = Pin(2, Pin.OUT)
led2 = Pin(38, Pin.OUT)


led1.on()    
utime.sleep(1)   
led1.off()       
utime.sleep(1)
led2.on()    
utime.sleep(1)   
led2.off()       
utime.sleep(1) 
