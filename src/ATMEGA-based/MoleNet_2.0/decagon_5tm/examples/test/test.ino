/******************************************************************************
 * MoleNet - A Wireless Underground Sensor Network
 *
 * Copyright (C) 2016, Communication Networks, University of Bremen, Germany
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * This file is part of MoleNet
 *
 ******************************************************************************
 * ============================================================================
 *
 *       Filename:  test.ino
 *
 *    Description:  Example for Decagon5TM Sensor Library
 *                  Arduino Sketch
 *
 *        Version:  1.0
 *        Created:  06/07/2016 12:00:41 
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Muhammad Haseeb (MSc. Student-IAE University of Bremen), 
 *                  mhaseeb@uni-bremen.de, 
 *                  hasee.b@hotmail.com
 *   Organization:  COMNETS, Communication Networks Department
 *                  University of Bremen, Bremen, Germany
 *
 * ============================================================================
 */
 
#include "decagon_5tm.h"

namespace Pin { enum Configuration {
      SENSOR_POWER  = 7,
      RX_5TM_Serial = 8,    // Software Serial
      TX_5TM_Serial = 11
};}

namespace Arduino { enum Settings {
      BAUD_RATE = 115200            // Baud rate of Terminal in Arduino
};}

Decagon5TM sensor(Pin::SENSOR_POWER,Pin::RX_5TM_Serial, Pin::TX_5TM_Serial );
void setup()
{
    Serial.begin(Arduino::BAUD_RATE);     
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

bool exciteSen = true;

void loop()
{
    if(exciteSen)
    {
        sensor.exciteSensor();
        exciteSen = false;
    }
    if( sensor.newSensorDataAvailable() )
    {
        sensor.debugSensorData();
        exciteSen = true;
    }
    
}

void serialEventRun(void) {
    sensor.serialEventRun();
}
