# main.py -- put your code here!

import _thread, machine, time
import lib_oled.oled_config as oled_config

oled = oled_config.initialize_oled()

led = machine.Pin(35, machine.Pin.OUT)
x = 0

def blink():
    global x
    while True:
        led.value(not led.value())
        print("Blinking: " + str(x))
        time.sleep(0.5)

def thread2():
    global oled, x
    while True:
        oled.new_text(str(time.time()), 0, 0)
        x= time.time()
        time.sleep(5)



try:
    _thread.start_new_thread(blink, ())
    _thread.start_new_thread(thread2, ())
except KeyboardInterrupt:
    # Wait for threads to finish execution
    _thread.exit()