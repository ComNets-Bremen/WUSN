# MoleNet: A Wireless Underground Sensor Network Platform
![MoleNet Logo](https://github.com/ComNets-Bremen/WUSN/blob/master/images/molenet_512.png)

Wireless Underground Sensor Network platform (PCB, Arduino-Source, MicroPython-Source and example
server)

This is an active project and the files in this directory (and the structure of
the project itself) are subject to change!

# New platform coming up!

Right now, the PCB and the overall platform received several major changes. Stay tuned for more information! We will update this Readme soon!

## Prerequisites

The following Arduino libraries are required:

- LowPower: <https://github.com/LowPowerLab/LowPower>
- RFM69: <https://www.github.com/lowpowerlab/rfm69>
- RV8523: <https://github.com/watterott/Arduino-Libs>
- spieep: <https://bitbucket.org/spirilis/spieep/>

## MoleNet (rev. 1.0)

![The WUSN node](https://github.com/ComNets-Bremen/WUSN/blob/master/images/WUSN_512.jpg)

### Settings for the Arduino IDE

Arduino IDE:

- Tools -> Board: "Arduino Pro or Pro Mini"
- Tools -> Processor: "ATmega328 (3.3V, 8MHz)"
- Tools -> Port: (USB port to which the programmer is attached)

Baud-rates:

- reading from MoleNet node: 1200 Baud (don't change!!!)
- reading from MoleNet simple Gateway: 115200 Baud
- setting the RTC: 9600 Baud (could also be changed)

### Data Format (simple Gateway)

The app just listens for packets and print the payload with correct formatting
The data format is as follows:

    #[PACKETNUMBER][NODEID] 'D'YEAR/MONTH/DAY 't'HOUR:MINUTE 'W'VWCRAWVALUE 'T'TEMPRAWVALUE CHECKSUM SUCCESSINDICATOR   ['RX_RSSI:'VALUE] - 'ACK sent.'

example:

    #[2][2] D16/3/30 t15:36 W0468 T0594 68 1    [RX_RSSI:-28] - ACK sent.

### Set the RTC time (Set_RTC)
The program prints current time of RTC to serial port every second. The time
can be changed by sending the new value using the serial line and terminate it
with a carriage return. Possible commands are:

- y : change the year, enter full year (e.g., 2016)
- M : change the month
- d : change the day
- h : change the hour
- m : change the minute
- s : change the second
- w : change the weekday (0: Monday, 1: Tuesday...)

### Manually trigger a measurement

Pull pin `Int1` at the RTC to ground. One tip is enough
