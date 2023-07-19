from machine import Pin
from lib.lib_communication.utils import get_millis

def irq_handler(pin):
    global irq_triggered, irq_debounce_timer, DEBOUNCE_TIME
    if pin.value() and irq_debounce_timer + DEBOUNCE_TIME < get_millis():
        irq_triggered = True
        irq_debounce_timer = get_millis()

def setup_irq(nodeCfg):
    global irq_pin
    if "gpio_button_irq" in nodeCfg:
        irq_pin = Pin(nodeCfg["gpio_button_irq"], Pin.IN, Pin.PULL_DOWN)
        irq_pin.irq(irq_handler)


""" #IRQ / Interrupt Request
irq_triggered = False
irq_debounce_timer = get_millis()
irq_last_trigger = get_millis()
DEBOUNCE_TIME = 100 # ms

def irq_handler(pin):
    global irq_triggered, irq_debounce_timer
    if pin.value() and irq_debounce_timer + DEBOUNCE_TIME < get_millis():
        irq_triggered = True
        irq_debounce_timer = get_millis()

if "gpio_button_irq" in nodeCfg:
    irq_pin = Pin(nodeCfg["gpio_button_irq"], Pin.IN, Pin.PULL_DOWN)
    irq_pin.irq(irq_handler) """