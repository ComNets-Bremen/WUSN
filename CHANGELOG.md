# Changelog


## Hardware

The different layouts are located in the [PCB-Layouts](PCB-Layouts) directory.
Each directory (should) contain a `*_schematic.pdf` file with the corresponding
schematic.


### MoleNet v1.0

First version of MoleNet. This version is based on an ATMEGA328P-PU and is
programmed using the Arduino IDE. Further, it has an RFM69VW radio transceiver,
an EEPROM, an SDI-12 sensor interface and a connector for an external RTC
module.

### MoleNet v2.0

This version has additional GPIO (analog and digital) available. Further, the
EEPROM access has been improved.

### MoleNet v2.1

Minor improvements for the SDI-12 sensor interface

### MoleNet v3.0

A redesign now based on the ATMEGA1284P, using an on board analog battery monitor, an MCP7940M RTC, improved SDI-12 interface, power enable pins etc.
This version offers more computing capabilities compared to the previous ones.

### MoleNet v5.0

Redesign with the STM32L073RZT6 and an on-board SD-card interface,
well-optimized to low power applications.

### MoleNet v5.1

Minor improvements, add connections for jumpers to completely disconnect the
transceiver module

### MoleNet v5.2

Add more jumpers for the LEDs

### MoleNet v5.2.2

Change in USART interface, new HF connector

### MoleNet v6.0

Redesign based on ESP32-S3, first version:

- The ESP supports MicroPython and Arduino
- All major components can be disconnected
- Direct ESP32 support for RTC
- SDI-12 interface, SD-card reader, native USB-port

**Known issues**

- Missing resistor on LED D7 (center)
- SDI-12 interface not working

### MoleNet v6.1

- Fixed the broken LED
- Add a QWIC connector for I2c-based devices
- Add an on-board BME280 temperature and humidity sensor


### MoleNet v6.2

- Fixed the SDI-12 interface
- Fixed some labels (reset button, BME enable pins)

**Known issues**

- Reset pin of LoRa not working. Bugfix: Keep the reset jumper on J5 empty

### MoleNet v6.3

Minor fixes

**Known issues**

- fixed the LoRa reset


## Software

TODO

