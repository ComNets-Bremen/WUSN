#sd card read and write for 10 second continously
import time
import os
from machine import SDCard, Pin

# Define SD card pins 
SD_CLK = 12
SD_MISO = 13
SD_MOSI = 11
SD_CS = 10

# Initialize SD card object
sd = SDCard(slot=2, sck=Pin(SD_CLK), miso=Pin(SD_MISO), mosi=Pin(SD_MOSI), cs=Pin(SD_CS))

try:
  # Print SD card information 
  print(sd.info())

  # Mount the SD card
  os.mount(sd, "/sd")

  # Define write data size 
  WRITE_DATA_SIZE = 1024  # Bytes

  # Function to write data to a file
  def write_to_file(filename, data_size):
    with open(filename, "wb") as f:
      # Generate random data to write
      data = bytearray(os.urandom(data_size))
      start_time = time.time()
      while time.time() - start_time < 10:
        f.write(data)

  # Function to read a file
  def read_from_file(filename):
    try:
      with open(filename, "rb") as f:
        start_time = time.time()
        while time.time() - start_time < 10:
          data = f.read(WRITE_DATA_SIZE)
          # Process the read data here (if needed)
          pass
    except OSError:
      print("File not found:", filename)

  # Define filename
  filename = "/sd/test_data.bin"

  # Write data
  write_to_file(filename, WRITE_DATA_SIZE)
  print("Wrote data to", filename)

  # Read the file
  read_from_file(filename)
  print("Read data from", filename)

  # Delete the file
  os.remove(filename)
  print("Deleted", filename)

finally:
  # Always unmount the SD card even if exceptions occur
  os.umount("/sd")

print("Done!")