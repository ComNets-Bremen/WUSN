# WUSN: MoleNet

Wireless Underground Sensor Network platform (PCB, Arduino-Source and example
server)

This is an active project and the files in this directory (and the structure of
the project itself) are subject to change!

![The WUSN node](https://github.com/ComNets-Bremen/WUSN/blob/master/images/WUSN_512.jpg)

## Prerequisites

The following Arduino libraries are required:

- LowPower: <https://github.com/LowPowerLab/LowPower>
- RFM69: <https://www.github.com/lowpowerlab/rfm69>
- RV8523: <https://github.com/watterott/Arduino-Libs>
- spieep: <https://bitbucket.org/spirilis/spieep/>

## Setting up the Arduino IDE for the MoleNet

Arduino IDE:

- Tools -> Board: "Arduino Pro or Pro Mini"
- Tools -> Processor: "ATmega328 (3.3V, 8MHz)"
- Tools -> Port: (USB port to which the programmer is attached)

Baud-rates:

- reading from MoleNet node: 1200 Baud (don't change!!!)
- reading from MoleNet simple Gateway: 115200 Baud
- setting the RTC: 9600 Baud (could also be changed)
