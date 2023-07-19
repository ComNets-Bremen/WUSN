from lib.board_config import heltecV3
from sx1262 import SX1262

""" lora_params = {
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
 'blocking': False
} """


def initialize_lora():
    """ starting lora device for SX1262 """
    
    lora = SX1262(
        spi_bus=heltecV3['lora_spi_bus'],
        clk=heltecV3['lora_clk'],
        mosi=heltecV3['lora_mosi'],
        miso=heltecV3['lora_miso'],
        cs=heltecV3['lora_cs'],
        irq=heltecV3['lora_irq'],
        rst=heltecV3['lora_rst'],
        gpio=heltecV3['lora_gpio'],
    )

    lora.begin(freq= 434, sf=7, cr=5, blocking=True)

    return lora