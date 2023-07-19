print("booted delta.py")

# Import all the functions from the param_func module and the time module
from lib.param_func import *
import time

# Initialize the LoRa module and the display
lora = initialize_lora()
oled = initialize_dsp()

# Initialize a counter variable
i = 0

# Start an infinite loop to send messages with the LoRa module
while True:
  # Send a message with the counter value
  lora.send('Hello World! {}'.format(i))
  # Display the same message on the OLED
  oled.new_text("sent lora msg {}".format(i))
  # Increment the counter by one
  i += 1
  # Wait for one second before sending again
  time.sleep(1)
