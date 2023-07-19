# Import the Pin class and the sleep function
from machine import Pin
from time import sleep

# Create a Pin object for the LED and set it as an output
led = Pin(35, Pin.OUT)

# Turn on the LED
def ledOn():
  led.value(1)

# Turn off the LED
def ledOff():
  led.value(0)

# Blink the LED gently (one second on and off)
def gentleBlink():
  ledOn()
  sleep(1)
  ledOff()
  sleep(1)

# Blink the LED rapidly (0.1 second on and off)
def rapidBlink():
  ledOn()
  sleep(0.1)
  ledOff()
  sleep(0.1)

# Blink the LED in SOS pattern (three short, three long, three short signals)
def sos():
  for i in range(3):
    ledOn()
    sleep(0.1)
    ledOff()
    sleep(0.1)

  sleep(0.5)

  for i in range(3):
    ledOn()
    sleep(0.5)
    ledOff()
    sleep(0.1)

  sleep(0.5)

  for i in range(3):
    ledOn()
    sleep(0.1)
    ledOff()
    sleep(0.1)

  sleep(2.5)
