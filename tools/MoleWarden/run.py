#!/usr/bin/env python3
"""
Jens Dede <jd@comnets.uni-bremen.de>, University of Bremen, 2025

Skeleton to detect an ESP32 connected to the computer. Idea: Offer options like

- Test the hardware
- Store when the hardware was tested the last time (including the serial
  numbers)
- Maybe put a meaningful name to the boards
- Many other convenience ideas ;-)
"""

from esptool.cmds import detect_chip
import serial.tools.list_ports


def erase_chip(port):
    with detect_chip(port) as esp:
            description = esp.get_chip_description()
            features = esp.get_chip_features()
            print(f"Detected ESP on port {port}: {description}")
            print(f"Features: {", ".join(features)}")
            print(esp.CHIP_NAME)
            esp = esp.run_stub()
            print("ESP Erase")
            esp.erase_flash()
            esp.hard_reset()


last_serial_number = 0

while True:
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if not port.vid==0x303A:
            # unsupported device
            continue
        if port.pid==0x4001:
            print("In normal mode")
        elif port.pid==0x1001:
            print("Programming mode")
        else:
            print(f"Unsupported mode: {port.vid}")
            continue

        if not port.serial_number:
            print("No serial number. Aborting.")
            continue
        serial_number = port.serial_number.replace(":", "")[:12]
        manufacturer  = port.manufacturer
        product       = port.product
        port          = port.device

        print(f"Found relevant device with serial {serial_number} on port {port}")

        if last_serial_number == serial_number:
            print("Skipping: No change!")
            continue
        last_serial_number = serial_number

        erase_chip(port)


