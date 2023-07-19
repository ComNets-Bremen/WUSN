from machine import Pin, ADC

pin = Pin(1, Pin.IN)
adc = ADC(pin)

def get_light_intensity() -> float:
    """ Returns light intensity in lux """
    # intensity = abs((adc.read() - 520) / 3575.808) * 100
    intensity = (adc.read() / 4096) * 100
    return round(intensity, 2)