# This is the boot.py file that runs on boot-up
print("Reading boot file")

# Allocate 100 bytes of memory for emergency exceptions
from micropython import alloc_emergency_exception_buf
alloc_emergency_exception_buf(100)

# Blink the LED gently to indicate the boot process
from lib.led import gentleBlink
gentleBlink()

print("Complete boot file")
