from lib.lib_other.board_config import heltecV3
from sx1262 import SX1262

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

def msgFN1(parsed_packet) -> list:
    """ add parsed packet to msg_collection
    in the format of: [[sender, receiver, seq, msg, retry_count, rssi, snr]] 
    parsed_packet: parsed packet from DataHandler"""
    _sender = parsed_packet.get_sender()
    sender = 0 if _sender == 3753907262 else (1 if _sender == 2637953060 else (2 if _sender == 4276759790 else (3 if _sender == 1119174399 else None)))

    _receiver = parsed_packet.get_receiver()
    receiver = 0 if _receiver == 3753907262 else (1 if _receiver == 2637953060 else (2 if _receiver == 4276759790 else (3 if _receiver == 1119174399 else None)))

    msg = ([sender, receiver, parsed_packet.get_sequence(), parsed_packet.get_params(), \
            parsed_packet.get_retry_count(), parsed_packet.get_rssi(), parsed_packet.get_snr()])
    
    return msg

def msgFN2(msg_collection, newmsg) -> bool:
    """ check if newmsg is in msg_collection by only comparing the sender, receiver, seq, msg """
    return any([newmsg[:4] == sublist[:4] for sublist in msg_collection])

def msgFN3(sender, receiver, seq, messege, retry_count = 0):
    """ add sending packet to msg_collection
    in the format of: [[sender, receiver, seq, msg, retry_count]] 
    """
    sender = [0 if sender == 3753907262 else (1 if sender == 2637953060 else (2 if sender == 4276759790 else (3 if sender == 1119174399 else None)))]
    receiver = [0 if receiver == 3753907262 else (1 if receiver == 2637953060 else (2 if receiver == 4276759790 else (3 if receiver == 1119174399 else None)))]
    
    msg = ([sender, receiver, seq, messege, retry_count])
    
    return msg

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