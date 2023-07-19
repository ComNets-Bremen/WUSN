print("booted gamma.py")

# Import all the functions from the param_func module and the time module
from lib.param_func import *
import time

# Initialize the LoRa module and the display
lora = initialize_lora()
oled = initialize_dsp()

# Start an infinite loop to receive messages from the LoRa module
while True:
  print("waiting for the msg")
  oled.new_text("waiting for the msg")
  msg, err = lora.recv()
  if msg is not None:
    print(msg)
    oled.new_text(msg)
  time.sleep(5)
