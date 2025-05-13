# Documentation

This directory contains the documentation of MoleNet. For the previous
versions, please refer to the [ATMEGA](ATMEGA) or [stm32](stm32) directory, respectively.


# The Hardware

![The MoleNet 6.1 hardware](/images/MoleNet_6.1.jpg)

## Available Components

- LoRa (SX1276)
- BME280
- SD-card
- qwiic
- LEDs
- SDI-12
- Additional pins (analog and digital)

## Pins

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
