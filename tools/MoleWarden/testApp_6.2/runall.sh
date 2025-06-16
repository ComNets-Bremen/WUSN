#!/bin/sh

# Run all steps in one file
# Hacky, but works ;-)


esptool.py --port /dev/ttyACM0 erase_flash
esptool.py --port /dev/ttyACM0 --baud 460800 write_flash 0 ESP32_GENERIC_S3-20250415-v1.25.0.bin
sleep 5 # time to restart
./pyboard.py -d /dev/ttyACM0 -f cp src/main.py :main.py
./pyboard.py -d /dev/ttyACM0 -f mkdir lib
./pyboard.py -d /dev/ttyACM0 -f cp src/lib/BME280.py :lib/BME280.py
screen /dev/ttyACM0

