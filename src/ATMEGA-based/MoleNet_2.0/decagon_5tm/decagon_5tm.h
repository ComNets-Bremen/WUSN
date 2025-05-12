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
 *       Filename:  decagon_5tm.h
 *
 *    Description:  Library for sensor Decagon 5TM Sensor.
 *                  Header File.         
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

#ifndef MOLENET_DECAGON_5TM_HPP_INCLUDED
#define MOLENET_DECAGON_5TM_HPP_INCLUDED

// #define DEBUG
#include "serial_print.h" 

#include <SoftwareSerial.h>

class Decagon5TM {
    public:
        Decagon5TM(const uint8_t &sensorPowerPin, const uint8_t &rxPinSS, const uint8_t &txPinSS); 
        void exciteSensor();
        bool newDecagon5tmDataAvailable()
        {  serialEventRun();    // Must always stays in last line of loop()
           return ifNewDataAvailable; }
        
        uint16_t getDielectric(); 
        uint16_t getTemperature();
        bool getChecksum();
        
        char computeChecksum(char * response, uint8_t length);
        
        void serialEventRun();
        void debugSensorData();
        
        enum Settings {
            BAUD_RATE                = 1200,   // Fixed from datasheet
            OFF_DELAY                = 200,
            ON_DELAY                 = 0,
            MAX_STRING_SIZE          = 17,
            RAW_DIELECTRIC_MAX_VAL   = 4094,
            RAW_DIELECTRIC_MIN_VAL   = 0,
            RAW_TEMPERATURE_MAX_VAL  = 1022,
            RAW_TEMPERATURE_MIN_VAL  = 0,
            RAW_TEMPERATURE_MAX_SIZE = 5,  // 4 charcters + 1 NULL at end of C array
            RAW_DIELECTIRC_MAX_SIZE  = 5   // 4 charcters + 1 NULL at end of C array
        };          
        
    private:
        uint8_t sensorPowerPin;    
        uint16_t rawTemperatureInt;
        uint16_t rawDielectricInt;
        char sensorType;
        char checksumOfPacket;    
        char checksumCalculated;
        bool ifNewDataAvailable;
        void serial5TMEvent();
        SoftwareSerial *serial5TM;     
};
#endif   // MOLENET_DECAGON_5TM_HPP_INCLUDED