# Import the rapidBlink function and the unique_id function
from lib.led import rapidBlink
from machine import unique_id

# Get the device ID and initialize the device name
device_id = unique_id()
device_name = ''

# Define a function to run a script based on the device name
def run(name):
  global device_name
  device_name = name
  print("the device is {}".format(device_name))
  exec(open('run/{}.py'.format(device_name)).read())

# Import the mac_ids dictionary
from lib.param_func import mac_ids

# Check if the device ID matches one of the MAC IDs
if device_id == mac_ids['alpha']:
  run('alpha')
elif device_id == mac_ids['beta']:
  run('beta')
elif device_id == mac_ids['gamma']:
  run('gamma')
elif device_id == mac_ids['delta']:
  run('delta')
else:
  # Print an error message and blink the LED rapidly
  print("Unknown device")
  while True:
    rapidBlink()