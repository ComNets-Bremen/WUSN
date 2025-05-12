# Copyright 2015, 2016 Ideetron B.V.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Modified by Brent Rubell for Adafruit Industries.
# Modified by Alan Peaty for MicroPython port.

import machine
import utime
import urandom
import ubinascii
import ulora_encryption

# SX1276 operating mode settings
_MODE_SLEEP = const(0x00)
_MODE_STDBY = const(0x01)
_MODE_FSTX = const(0x02)
_MODE_TX = const(0x03)
_MODE_FSRX = const(0x04)
_MODE_RX = const(0x05)
_MODE_ACCESS_SHARED_REG = const(0x40)
_MODE_LORA = const(0x80)
# SX1276 registers
_REG_FIFO = const(0x00)
_REG_OPERATING_MODE = const(0x01)
_REG_FRF_MSB = const(0x06)
_REG_FRF_MID = const(0x07)
_REG_FRF_LSB = const(0x08)
_REG_PA_CONFIG = const(0x09)
_REG_FIFO_POINTER = const(0x0D)
_REG_RSSI_CONFIG = const(0x0E)
_REG_RSSI_COLLISION = const(0x0F)
_REG_FEI_MSB = const(0x1D)
_REG_FEI_LSB = const(0x1E)
_REG_PREAMBLE_DETECT = const(0x1F)
_REG_PREAMBLE_MSB = const(0x20)
_REG_PREAMBLE_LSB = const(0x21)
_REG_PAYLOAD_LENGTH = const(0x22)
_REG_MODEM_CONFIG = const(0x26)
_REG_TIMER1_COEF = const(0x39)
_REG_NODE_ADDR = const(0x33)
_REG_IMAGE_CAL = const(0x3B)
_REG_TEMP_VALUE = const(0x3C)
_REG_DIO_MAPPING_1 = const(0x40)
_REG_VERSION = const(0x42)
_REG_FIFO_BASE_ADDR = const(0x80)

class TTN:
    """ TTN Class.
    """
    def __init__(self, dev_address, net_key, app_key, country="EU"):
        """ Interface for The Things Network.
        """
        self.dev_addr = dev_address
        self.net_key = net_key
        self.app_key = app_key
        self.region = country

    @property
    def device_address(self):
        """ Returns the TTN Device Address.
        """
        return self.dev_addr
    
    @property
    def network_key(self):
        """ Returns the TTN Network Key.
        """
        return self.net_key

    @property
    def application_key(self):
        """ Returns the TTN Application Key.
        """
        return self.app_key
    
    @property
    def country(self):
        """ Returns the TTN Frequency Country.
        """
        return self.region


class uLoRa:
    """ uLoRa Interface.
    """
    # Fixed data rates for SX1276 LoRa
    _DATA_RATES = {
        "SF7BW125":(0x74, 0x72, 0x04), "SF7BW250":(0x74, 0x82, 0x04),
        "SF8BW125":(0x84, 0x72, 0x04), "SF9BW125":(0x94, 0x72, 0x04),
        "SF10BW125":(0xA4, 0x72, 0x04), "SF11BW125":(0xB4, 0x72, 0x0C),
        "SF12BW125":(0xC4, 0x72, 0x0C)
    }
    # SPI write buffer
    _BUFFER = bytearray(2)

    def __init__(self, cs, sck, mosi, miso, irq, rst, ttn_config, datarate="SF7BW125", fport=1, channel=None):
        """ Interface for a Semtech SX1276 module. Sets module up for sending to
        The Things Network.
        """
        self._irq = machine.Pin(irq, machine.Pin.IN)
        self._cs = machine.Pin(cs, machine.Pin.OUT, value=1)
        self._rst = machine.Pin(cs, machine.Pin.OUT, value=1)
        # Set up SPI device on Mode 0
        self._device = machine.SPI(
            1,
            baudrate=2000000,
            polarity=0,
            phase=0,
            sck=machine.Pin(sck),
            mosi=machine.Pin(mosi),
            miso=machine.Pin(miso)
        )
        # Verify the version of the SX1276 module
        self._version = self._read_u8(_REG_VERSION)
        print(self._version)
        if self._version != 18:
            raise TypeError("Can not detect LoRa Module. Please check wiring!")
        # Set Frequency registers
        self._rfm_msb = None
        self._rfm_mid = None
        self._rfm_lsb = None
        # Set datarate registers
        self._sf = None
        self._bw = None
        self._modemcfg = None
        self.set_datarate(datarate)
        self._fport = fport
        # Set regional frequency plan
        if "US" in ttn_config.country:
            from ttn_usa import TTN_FREQS
            self._frequencies = TTN_FREQS
        elif ttn_config.country == "AS":
            from ttn_as import TTN_FREQS
            self._frequencies = TTN_FREQS
        elif ttn_config.country == "AU":
            from ttn_au import TTN_FREQS
            self._frequencies = TTN_FREQS
        elif ttn_config.country == "EU":
            from ttn_eu import TTN_FREQS
            self._frequencies = TTN_FREQS
        else:
            raise TypeError("Country Code Incorrect/Unsupported")
        # Set SX1276 channel number
        self._channel = channel
        self._tx_random = urandom.getrandbits(3)
        if self._channel is not None:
            # Set single channel
            self.set_channel(self._channel)
        # Init FrameCounter
        self.frame_counter = 0
        # Set up SX1276 for LoRa Mode
        for pair in ((_REG_OPERATING_MODE, _MODE_SLEEP), (_REG_OPERATING_MODE, _MODE_LORA),
                     (_REG_PA_CONFIG, 0xFF), (_REG_PREAMBLE_DETECT, 0x25),
                     (_REG_PREAMBLE_MSB, 0x00), (_REG_PREAMBLE_LSB, 0x08),
                     (_REG_MODEM_CONFIG, 0x0C), (_REG_TIMER1_COEF, 0x34),
                     (_REG_NODE_ADDR, 0x27), (_REG_IMAGE_CAL, 0x1D),
                     (_REG_RSSI_CONFIG, 0x80), (_REG_RSSI_COLLISION, 0x00)):
            self._write_u8(pair[0], pair[1])
        # Give the uLoRa object ttn configuration
        self._ttn_config = ttn_config

    def send_data(self, data, data_length, frame_counter, timeout=2):
        """ Function to assemble and send data.
        """
        # Data packet
        enc_data = bytearray(data_length)
        lora_pkt = bytearray(64)
        # Copy bytearray into bytearray for encryption
        enc_data[0:data_length] = data[0:data_length]
        # Encrypt data (enc_data is overwritten in this function)
        self.frame_counter = frame_counter
        aes = ulora_encryption.AES(
            self._ttn_config.device_address,
            self._ttn_config.app_key,
            self._ttn_config.network_key,
            self.frame_counter
        )
        enc_data = aes.encrypt(enc_data)
        # Construct MAC Layer packet (PHYPayload)
        # MHDR (MAC Header) - 1 byte
        lora_pkt[0] = 0x40  # MType: unconfirmed data up, RFU / Major zeroed
        # MACPayload
        # FHDR (Frame Header): DevAddr (4 bytes) - short device address
        lora_pkt[1] = self._ttn_config.device_address[3]
        lora_pkt[2] = self._ttn_config.device_address[2]
        lora_pkt[3] = self._ttn_config.device_address[1]
        lora_pkt[4] = self._ttn_config.device_address[0]
        # FHDR (Frame Header): FCtrl (1 byte) - frame control
        lora_pkt[5] = 0x00
        # FHDR (Frame Header): FCnt (2 bytes) - frame counter
        lora_pkt[6] = self.frame_counter & 0x00FF
        lora_pkt[7] = (self.frame_counter >> 8) & 0x00FF
        # FPort - port field
        lora_pkt[8] = self._fport
        # Set length of LoRa packet
        lora_pkt_len = 9
        print("PHYPayload", ubinascii.hexlify(lora_pkt))
        # FRMPayload - MAC Frame Payload Encryption
        lora_pkt[lora_pkt_len:lora_pkt_len+data_length] = enc_data[0:data_length]
        print("PHYPayload with FRMPayload", ubinascii.hexlify(lora_pkt))
        # Recalculate packet length
        lora_pkt_len += data_length
        # Calculate Message Integrity Code (MIC)
        # MIC is calculated over: MHDR | FHDR | FPort | FRMPayload
        mic = bytearray(4)
        mic = aes.calculate_mic(lora_pkt, lora_pkt_len, mic)
        # Load MIC in package
        lora_pkt[lora_pkt_len:lora_pkt_len+4] = mic[0:4]
        # Recalculate packet length (add MIC length)
        lora_pkt_len += 4
        print("PHYPayload with FRMPayload + MIC", ubinascii.hexlify(lora_pkt))
        self.send_packet(lora_pkt, lora_pkt_len, timeout)

    def send_packet(self, lora_packet, packet_length, timeout):
        """ Sends a LoRa packet using the SX1276 module.
        """
        # Set SX1276 to standby
        self._write_u8(_REG_OPERATING_MODE, _MODE_LORA | _MODE_STDBY)
        # Wait for SX1276 to enter standby mode
        utime.sleep_ms(10)
        # Switch interrupt to TxDone (DIO0)
        self._write_u8(_REG_DIO_MAPPING_1, 0x40)
        # Check for multi-channel configuration
        if self._channel is None:
            self._tx_random = urandom.getrandbits(3)
            self._rfm_lsb = self._frequencies[self._tx_random][2]
            self._rfm_mid = self._frequencies[self._tx_random][1]
            self._rfm_msb = self._frequencies[self._tx_random][0]
        # Set up frequency registers
        for pair in (
            (_REG_FRF_MSB, self._rfm_msb), (_REG_FRF_MID, self._rfm_mid),
            (_REG_FRF_LSB, self._rfm_lsb), (_REG_FEI_LSB, self._sf),
            (_REG_FEI_MSB, self._bw), (_REG_MODEM_CONFIG, self._modemcfg),
            (_REG_PAYLOAD_LENGTH, packet_length),
            (_REG_FIFO_POINTER, _REG_FIFO_BASE_ADDR)
        ):
            self._write_u8(pair[0], pair[1])
        # Fill the FIFO buffer with the LoRa payload
        for k in range(packet_length):
            self._write_u8(_REG_FIFO, lora_packet[k])
        # Switch SX1276 to TX operating mode
        self._write_u8(_REG_OPERATING_MODE, _MODE_TX)
        # Wait for TxDone IRQ, poll for timeout
        start = utime.time()
        timed_out = False
        while not timed_out and not self._irq.value:
            if(utime.time - start) >= timeout:
                timed_out = True
        # Switch SX1276 to sleep operating mode
        self._write_u8(_REG_OPERATING_MODE, _MODE_SLEEP)
        if timed_out:
            raise RuntimeError("Timeout during packet send")

    def set_datarate(self, datarate):
        """ Sets the SX1276 datarate.
        """
        try:
            self._sf, self._bw, self._modemcfg = self._DATA_RATES[datarate]
        except KeyError:
            raise KeyError("Invalid or Unsupported Datarate.")

    def set_channel(self, channel):
        """ Sets the SX1276 channel (if single-channel).
        """
        self._rfm_msb, self._rfm_mid, self._rfm_lsb = self._frequencies[channel]
        
    def get_temp(self):
        """ Get temperature reading from SX1276.
        Output is not an absolute temperature in celcius, and requires calibration
        """
        _TEMP_WAIT_TIME = 140
        # Set SX1276 to standby
        self._write_u8(_REG_OPERATING_MODE, _MODE_LORA | _MODE_STDBY)
        # Wait for SX1276 to enter standby mode
        utime.sleep_ms(10)
        # Set SX1276 to FSRx mode
        self._write_u8(_REG_OPERATING_MODE, _MODE_LORA | _MODE_FSRX | _MODE_ACCESS_SHARED_REG)
        _reg_image_cal = self._read_u8(_REG_IMAGE_CAL)
        # Clear and re-set TempMonitorOff flag
        self._write_u8(_REG_IMAGE_CAL, _reg_image_cal & 254)
        utime.sleep_ms(_TEMP_WAIT_TIME)
        self._write_u8(_REG_IMAGE_CAL, _reg_image_cal)
        # Switch SX1276 back to sleep operating mode, and clear access shared register
        # mode
        _temp = self._read_u8(_REG_TEMP_VALUE)
        self._write_u8(_REG_OPERATING_MODE, _MODE_SLEEP & 191)
        return _temp

    def _read_into(self, address, buf):
        """ Read a number of bytes from the specified address into the
        provided buffer.
        """
        # Set MSB to 0 (read)
        # 0x7F = 01111111
        self._BUFFER[0] = address & 0x7F
        self._cs.off()
        self._device.write(self._BUFFER[0:1])
        self._device.readinto(buf)
        self._cs.on()

    def _read_u8(self, address):
        """ Read a single byte from the provided SX1276 register and return it.
        """
        self._read_into(address, self._BUFFER)
        return self._BUFFER[0]

    def _write_u8(self, address, val):
        """ Writes to the SX1276 register given an address and data.
        """
        self._cs.off()
        # Set MSB to 1 (write)
        # 0x80 = 1000000
        self._BUFFER[0] = (address | 0x80)  
        self._BUFFER[1] = val
        self._device.write(self._BUFFER[0:2])
        self._cs.on()
