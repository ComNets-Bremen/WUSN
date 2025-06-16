Test MicroPython
================

Quick and dirty test for main functionality of new boards.

Jens Dede <jd@comnets.uni-bremen.de>

We are assuming that the MoleNet board is connected to the serial port
`/dev/ttyACM0`. Please adapt this according to your system / configuration.

Prerequisites
-------------

- esptool (pip install esptool)
- pyboard.py (-> https://raw.githubusercontent.com/micropython/micropython/refs/heads/master/tools/pyboard.py
- MicroPython VM (->
  https://micropython.org/resources/firmware/ESP32_GENERIC_S3-20250415-v1.25.0.bin)

Setup Environment
-----------------
- Create virtual environment: `python3 -m venv venv`
- Activate virtual environment: `. ./venv/bin/activate`
- Install esptool: `pip install esptool`

Test Board
----------
- Set the board to boot mode (boot button + reset)
- Erase board: `esptool.py --port /dev/ttyACM0 erase_flash`
- Install VM: `esptool.py --port /dev/ttyACM0 --baud 460800 write_flash 0
  ESP32_GENERIC_S3-20250415-v1.25.0.bin`
- Reset board (reset button)
- Upload example code: `./pyboard.py -d /dev/ttyACM0 -f cp src/main.py
  :main.py`
- Create lib dir: `./pyboard.py -d /dev/ttyACM0 -f mkdir lib`
- Upload BME280 lib: `./pyboard.py -d /dev/ttyACM0 -f cp src/lib/BME280.py
  :lib/BME280.py`
- Connect to the serial port `screen /dev/ttyACM0` to check the output and
  restart the device
- Test the next device

Example output
--------------
    *** Testing board with ID aabbccddeeff ***
    ## Step 1: Test LED
    ## Step 1: Done
    ## Step 2: Test BME280
    Temperature: 25.57C
    Humidity   : 40.44%
    Pressure   : 1024.80hPa
    ## Step 2: Done
    ## Step 3: Test WiFi
    WiFis: Uni-Bremen, eduroam
    ## Step 3: Done


Further readings
----------------

- pyboard docs:
  `https://docs.micropython.org/en/latest/reference/pyboard.py.html`
- esptool docs:
  `https://docs.espressif.com/projects/esptool/en/latest/esp32/esptool/basic-commands.html`
