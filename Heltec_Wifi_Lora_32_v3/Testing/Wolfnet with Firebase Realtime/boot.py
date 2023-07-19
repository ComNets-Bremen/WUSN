# boot.py -- run on boot-up
print("reading boot.py")

from lib.lib_led.led import gentle_blink
gentle_blink()

import network, time

ap = network.WLAN(network.STA_IF)
ap.active(True)

while (not ap.isconnected()):
    ap.connect("PRIME-ROG-STRIX", "eiscreamburger")
    time.sleep(5)


print("Wi-Fi connected")
print("IP address:", ap.ifconfig()[0])