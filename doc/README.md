# Documentation

This directory contains the documentation of MoleNet. For the previous
versions, please refer to the [ATMEGA](ATMEGA) or [stm32](stm32) directory, respectively.


# The Hardware

![The MoleNet 6.1 hardware](/images/MoleNet_6.1.jpg The MoleNet PCB)

--------------

MoleNet contains various different on-board parts. The following tables lists
the connections and other relevant information for the major parts. For further
details, refer to [schematics of the board](../PCB-Layouts/). The table lists
the part of the board revision 6.3.

| **Part** | **Part Number**      | **Description**                                                  | **Pins**                                                                                                                            |
|----------|----------------------|------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| SW1      | SKHHARA010           | Reset button (enable) for ESP32                                  | EN                                                                                                                                  |
| SW2      | SKHHARA010           | Boot mode button for ESP32                                       | GPIO0                                                                                                                               |
| U1       | ESP32-S3-WROOM-1-N16 | ESP32-S3                                                         |                                                                                                                                     |
| U3       | TLV76133DCYR         | Voltage regulator 3V3                                            |                                                                                                                                     |
| U5       | Ra-01H               | LoRa Module (SX1276)                                             | RESET=GPIO15, NSS=GPIO48, MOSI=GPIO47, MISO=GPIO21, SCK=GPIO14, DIO0=GPIO46                                                         |
| U7       | BME280               | Sensor for humidity, barometric pressure and ambient temperature | I2C, SCL=GPIO8, SDA=GPIO9, address=0x76                                                                                             |
| D6       | NCD0805R1            | User LED                                                         | GPIO38                                                                                                                              |
| D7       | NCD0805R1            | User LED                                                         | GPIO2                                                                                                                               |
| D8       | NCD0805R1            | Power LED                                                        | Power                                                                                                                               |
| J1       |                      | Enable SD-card via jumpers                                       | Power=GPIO7 (optional), MOSI=GPIO11, MISO=GPIO13, SCK=GPIO12, CS=GPIO10                                                             |
| J2       |                      | Enable BME280 connection via jumpers                             |                                                                                                                                     |
| J3       |                      | Power out                                                        | GND, 3V3, Vin                                                                                                                       |
| J4       |                      | Enable LEDs via jumpers                                          |                                                                                                                                     |
| J5       |                      | Enable LoRa via jumpers                                          |                                                                                                                                     |
| J6       |                      | USB connector                                                    |                                                                                                                                     |
| J7       |                      | Power in                                                         |                                                                                                                                     |
| J8       |                      | Misc. pins (ADC etc.)                                            |                                                                                                                                     |
| J9       |                      | Unused GPIOs                                                     |                                                                                                                                     |
| J10      |                      | SDI-12 connector                                                 | Uses the following GPIOs: Power=GPIO1, SDI_RX_ENABLE=GPIO36, SDI_RX=GPIO18, SDI_TX_ENABLE=GPIO35, SDI_TX=GPIO17, SDI_MARKING=GPIO37 |
| J11      |                      | Remaining DIOs from LoRa transceiver                             |                                                                                                                                     |
| J12      |                      | Power jumper, for power measurements. Connect via jumper         |                                                                                                                                     |
| J13      |                      | Micro SD card holder                                             |                                                                                                                                     |
| J14      | SMA-KWE902           | SMA connector for LoRa                                           |                                                                                                                                     |
| J15      |                      | qwiic connector for I2C devices                                  | SCL=GPIO8, SDA=GPIO9 (shared with BME280)                                                                                           |
| J16      |                      | Power SDI-12 interface via jumper. Alternatively: GPIO1          |                                                                                                                                     |



## Available Components

- LoRa (SX1276)
- BME280
- SD-card
- qwiic
- LEDs
- SDI-12
- Additional pins (analog and digital)

## Pins

For the usage of the pins, please refer to the hardware section in this
document or the schematics of the corresponding versions available in [the PCB-Layouts directory](../PCB-Layouts/).

# Setup using Arduino

1) Install **Arduino IDE** -> [arduino.cc](https://www.arduino.cc/en/software/)
2) Install the **esp32** package by Espressif via the **Boards Manager** in the IDE
3) Select the **ESP32S3 Dev Module** as the board
4) Select the corresponding USB port
5) Set USB CDC On Boot: Enabled
6) Set USB Mode: USB-OTG (Tiny USB)

**Attention: Different board types!**

Different ports are created at various times when the MoleNet platform is connected to the computer. One port is to flash the firmware, while the second is to access the MoleNet Platform’s serial port. The first is the default after a reset. The second is usually entered if the boot button is pressed during reset or when powering up the board.

Usually, you have to setup the board twice according to the settings above. Be
aware that the port identifier can change depending on your operating system.

# Setup using MicroPython
