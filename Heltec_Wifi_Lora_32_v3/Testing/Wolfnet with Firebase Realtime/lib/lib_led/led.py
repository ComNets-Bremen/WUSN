from lib.board_config import heltecV3
from time import sleep

led = heltecV3['led']

def on():
    """ turn ON the LED in your board """
    led.value(1)

def off():
    """ turn OFF the LED in your board """
    led.value(0)
    
def gentle_blink():
    """ will blink the LED on your board for 0.5 second """
    led.value(1)
    sleep(0.5)
    led.value(0)
    sleep(0.5)

def blink():
    """ will blink the LED on your board for 0.1 second """
    led.value(1)
    sleep(0.1)
    led.value(0)
    sleep(0.1)