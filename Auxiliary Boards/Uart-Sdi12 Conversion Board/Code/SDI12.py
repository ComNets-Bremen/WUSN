#Pins:
#SDI_Marking: 37
#TX: 43
#Rx: 44
#RX_Enable: 36 Pull high to activate RX
#TX_Enable: 35 Pull low to activate TX

#Timing: Enable tx -> 12ms break (pull line high) followed by 8.33ms marking(line low) then data -> in less than 7.5ms enable RX
#https://www.sdi-12.org/current_specification/SDI-12_version-1_4-Jan-30-2021.pdf page 41
#puling optocopler high via Jumper

from machine import UART, Pin
import utime

class SDI12:
    def __init__(self):
        print("Init SDI12")
        
        #define pins, start with TX activated
        self.RX = Pin(44)
        self.TX = Pin(43)
        self.SDI_Marking = Pin(37, Pin.OUT, value=1)
        self.RX_Enable = Pin(36, Pin.OUT, value=0)
        self.TX_Enable = Pin(35, Pin.OUT, value=0)

        #define UART
        self.sdi12 = UART(1, baudrate=1200, bits=7, parity=0, stop=1, tx=self.TX, rx=self.RX, invert=UART.INV_RX)

        self.command()
        utime.sleep(1)
        self.command("0I!")
        
    def write(self, msg):
        print("writing")
        #enter write mode
        self.RX_Enable = Pin(36, Pin.OUT, value=0)
        self.TX_Enable = Pin(35, Pin.OUT, value=0)
        #12ms break
        self.SDI_Marking.value(0)
        utime.sleep_ms(13)
        #8.33ms marking
        self.SDI_Marking.value(1)
        utime.sleep_ms(9)
        #transmit command
        self.sdi12.write(msg)
        self.sdi12.flush() #wait for all data to be sent, if errors occur, try utime.sleep_ms(50)
        
    def read(self):
        print("reading")
        #enter reading mode
        self.RX_Enable = Pin(36, Pin.OUT, value=1)
        self.TX_Enable = Pin(35, Pin.OUT, value=1)
        utime.sleep_ms(500)
        #read
        response = self.sdi12.read()
        print(response)

    def command(self, command = "?!"):
        #send SDI12 command
        self.write(command)
        #read sensor answer
        self.read()
        
sdi12 = SDI12()