#Program currently works on Micropython 1.23
#This script wakes the esp up every 5 min, reads data from 5 SDI12 sensors and saves it to sd Card, then enters deepsleep for another 5 min
#LoRaWAN functionality is implemented, but not used yet. BME280 is also implemented.
import utime
import machine
from machine import I2C, Pin, deepsleep, SoftI2C, RTC
import array
import ubinascii
from ulora import TTN, uLoRa
import BME280
import os
from SDI12 import SDI12

class MoleNet:
    def __init__(self):
        #set status led to indicate sleep
        self.status_led = Pin(2, Pin.OUT, value=1)
        
        #setup RTC, Thonny automatically sync time with computer
        self.rtc = RTC()
        
        #setup bme280
        try:
            i2c = SoftI2C(scl=Pin(8), sda=Pin(9), freq=10000)
            self.bme = BME280.BME280(i2c=i2c)
        except Exception as exc:
            print(exc)
            
        #setup SDI12
        self.num_sensors = 5
        rx = Pin(18)
        tx = Pin(17)
        marking = Pin(37, Pin.OUT, value=1)
        rx_enable = Pin(36, Pin.OUT, value=0)
        tx_enable = Pin(35, Pin.OUT, value=0)
        power_sdi12 = Pin(1, Pin.OUT, value=1)
        self.sdi12 = SDI12(rx, tx, marking, rx_enable, tx_enable)
            
        #setup sd card   
        sd = machine.SDCard(slot=2, sck=12, miso=13, mosi=11, cs=10, freq=200_000)
        utime.sleep(1)
        os.mount(sd, '/sd') # mount

        
        # SPI pins
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
        TTN_DEVADDR = bytearray([0x00, 0x00, 0x00, 0x00])
        # TTN network session key, 16 Bytes, MSB (REPLACE WITH YOUR OWN!!!)
        TTN_NWKEY = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
        # TTN application session key, 16 Bytes, MSB (REPLACE WITH YOUR OWN!!!)
        TTN_APP = bytearray([0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]) 
        TTN_CONFIG = TTN(TTN_DEVADDR, TTN_NWKEY, TTN_APP, country="EU")
        

        self.lora = uLoRa(
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
    def measure_soil(self):
        for i in range(1,self.num_sensors+1):
            val = self.sdi12.measure(i)
            with open("/sd/soil_data.csv","a") as f:
                f.write("{};{};{}\n".format(i, self.rtc.datetime(),val))
            utime.sleep_ms(50)
        
    def measure_air(self):
        temp = self.bme.temperature
        hum = self.bme.humidity
        pres = self.bme.pressure
        return temp, hum, pres

    def send(self, data):
        print("Sending packet...", self.lora.frame_counter, ubinascii.hexlify(data))
        self.lora.send_data(data, len(data), self.lora.frame_counter)
        print(len(data), "bytes sent!")
        self.lora.frame_counter += 1
        
molenet = MoleNet()
#molenet.send(b'hi')
#molenet.measure_air()
molenet.measure_soil()
molenet.status_led.off()
machine.deepsleep(300000)