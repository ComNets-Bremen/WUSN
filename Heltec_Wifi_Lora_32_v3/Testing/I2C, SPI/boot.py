from lib.led import gentle_blink
gentle_blink()

from lib.oled_config import initalize_oled
oled = initalize_oled()
oled.poweroff()
del oled