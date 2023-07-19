from machine import Pin, I2C, reset
from lib.lib_lora import sx1262
from time import sleep_ms
from lib.lib_oled import ssd1306

# Define a dictionary of MAC IDs for different devices
mac_ids = {
 'alpha': b'\xf4\x12\xfa\x81d\xfc',
 'beta': b'\xf4\x12\xfa\x80i ',
 'gamma': b'\xf4\x12\xfa\x80i\xa0',
 'delta': b'\xf4\x12\xfa\x81f\x0c'
}

""" # Define a function to convert a message to 16 bytes length
def to16xLengthBytes(msg):
 
 # If the message is not bytes, encode it as bytes
 if type(msg) is not bytes:
  msg = msg.encode()
 
 # Pad the message with zeros until it reaches 16 bytes length
 return msg + b'\x00' * ((16 - (len(msg) % 16)) % 16) """


""" DISPLAY CONFIG """

# Define a dictionary of pins for the display
dsp_pins = {
 'rst': Pin(21, Pin.OUT),
 'sda': Pin(17, Pin.OUT, Pin.PULL_UP),
 'scl': Pin(18, Pin.OUT, Pin.PULL_UP)
}

# Define a function to initialize the display
def initialize_dsp():
 
  # Reset the display by setting the rst pin low and then high
  dsp_pins['rst'].value(0)
  sleep_ms(5)
  dsp_pins['rst'].value(1)

  try:
    # Try to create an I2C object for the display using the sda and scl pins
    dsp_i2c = I2C(sda=dsp_pins['sda'], scl=dsp_pins['scl'])
    oled = ssd1306.SSD1306_I2C(128, 64, dsp_i2c)

  except:
    # If there is an error, reset the board
    reset()

  # Create an OLED object using the I2C object and the display dimensions

  oled.fill(0)
  oled.show()

  # Return the OLED object
  return oled


""" LORA CONFIG """

# Define a dictionary of pins for the LoRa module
lora_pins = {
 'spi_bus': -1,
 'clk': 9,
 'mosi': 10,
 'miso': 11,
 'cs': 8,
 'irq': 14,
 'rst': 12,
 'gpio': 13
}

# Define a dictionary of parameters for the LoRa module
lora_params = {
 'freq': 434,
 'bw': 500.0,
 'sf': 12,
 'cr': 8,
 'syncWord': 0x12,
 'power': -5,
 'currentLimit': 60.0,
 'preambleLength': 8,
 'implicit': False,
 'implicitLen': 0xFF,
 'crcOn': True,
 'txIq': False,
 'rxIq': False,
 'tcxoVoltage': 1.7,
 'useRegulatorLDO': False,
 'blocking': True
}


# Define a function to initialize the LoRa module
def initialize_lora():
  # Create a SX1262 object using the pins dictionary
  lora = sx1262.SX1262(
    spi_bus=lora_pins['spi_bus'],
    clk=lora_pins['clk'],
    mosi=lora_pins['mosi'],
    miso=lora_pins['miso'],
    cs=lora_pins['cs'],
    irq=lora_pins['irq'],
    rst=lora_pins['rst'],
    gpio=lora_pins['gpio']
  )

  # Set the parameters for the LoRa module using the params dictionary
  lora.begin(
    freq=lora_params['freq'],
    bw=lora_params['bw'],
    sf=lora_params['sf'], 
    cr=lora_params['cr'],
    syncWord=lora_params['syncWord'],
    power=lora_params['power'],
    currentLimit=lora_params['currentLimit'],
    preambleLength=lora_params['preambleLength'],
    implicit=lora_params['implicit'],
    implicitLen=lora_params['implicitLen'],
    crcOn=lora_params['crcOn'],
    txIq=lora_params['txIq'],
    rxIq=lora_params['rxIq'],
    tcxoVoltage=lora_params['tcxoVoltage'],
    useRegulatorLDO=lora_params['useRegulatorLDO'],
    blocking=lora_params['blocking']
  )

  # Return the SX1262 object
  return lora