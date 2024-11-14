import utime
import machine
from machine import I2C, Pin, deepsleep
import array
import ubinascii
import dht
from ulora import TTN, uLoRa

# Heltec ESP32 LoRa V2 development board SPI pins
LORA_CS = const(48)
LORA_SCK = const(14)
LORA_MOSI = const(47)
LORA_MISO = const(21)
LORA_IRQ = const(46)
LORA_RST = const(45)
LORA_DATARATE = "SF12BW125"
LORA_FPORT = const(1)

# The Things Network (TTN) device details (available in TTN console)
# TTN device address, 4 Bytes, MSB (REPLACE WITH YOUR OWN!!!)
TTN_DEVADDR = bytearray([0x00 ...])
# TTN network session key, 16 Bytes, MSB (REPLACE WITH YOUR OWN!!!)
TTN_NWKEY = bytearray([0x00 ...])
# TTN application session key, 16 Bytes, MSB (REPLACE WITH YOUR OWN!!!)
TTN_APP = bytearray([0x00 ...]) 
TTN_CONFIG = TTN(TTN_DEVADDR, TTN_NWKEY, TTN_APP, country="EU")

lora = uLoRa(
    cs=LORA_CS,
    sck=LORA_SCK,
    mosi=LORA_MOSI,
    miso=LORA_MISO,
    irq=LORA_IRQ,
    rst=LORA_RST,
    ttn_config=TTN_CONFIG,
    datarate=LORA_DATARATE,
    fport=LORA_FPORT
)


for i in range(10):
    data = b'Hello World'
    print("Sending packet...", lora.frame_counter, ubinascii.hexlify(data))
    lora.send_data(data, len(data), lora.frame_counter)
    print(len(data), "bytes sent!")
    lora.frame_counter += 1
    utime.sleep(5)
