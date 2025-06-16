#!/bin/sh

# Run all steps in one file
# Hacky, but works ;-)

PORT=/dev/ttyACM0
BINARY=ESP32_GENERIC_S3-20250415-v1.25.0.bin

# Check for tools
for f in esptool.py screen
do
    type $f 2>&1 > /dev/null
    if [ $? -ne 0 ] ; then
        echo \"$f\" does not exist. Please install it to run this script
        exit 1
    fi
done

# Check for files
for f in pyboard.py $BINARY
do
    if [ ! -e $f ] ; then
        echo \"$f\" does not exist. Please download it to run this script
        exit 1
    fi
done

# Start the test
set -e # Abort in case of errors
esptool.py --port $PORT erase_flash
esptool.py --port $PORT --baud 460800 write_flash 0 $BINARY
#esptool.py --port $PORT --no-stub flash_id
echo "Reset"
sleep 5 # time to restart
./pyboard.py -d $PORT -f cp src/main.py :main.py
./pyboard.py -d $PORT -f mkdir lib
./pyboard.py -d $PORT -f cp src/lib/BME280.py :lib/BME280.py
screen $PORT

