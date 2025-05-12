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
 *       Filename:  decagon_5tm.cpp
 *
 *    Description:  Library for sensor Decagon 5TM Sensor.
 *                  Implementation File.         
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

Decagon5TM::Decagon5TM(const uint8_t &sensorPowerPin, const uint8_t &rxPinSS, const uint8_t &txPinSS) :
    sensorPowerPin(sensorPowerPin),
    sensorType(NULL),
    checksumOfPacket(NULL),
    checksumCalculated(NULL),
    ifNewDataAvailable(false)
{
    pinMode(sensorPowerPin,OUTPUT);
    
    pinMode(rxPinSS, INPUT);
    pinMode(txPinSS, OUTPUT);
    
    // This Serial port is used to get Data from Sensor 5TM
    serial5TM = new SoftwareSerial(rxPinSS,txPinSS);
    serial5TM->begin(Decagon5TM::BAUD_RATE);  
    while(!serial5TM) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

void Decagon5TM::exciteSensor()
{
    ifNewDataAvailable = false;
    digitalWrite(sensorPowerPin,LOW);
    delay(Decagon5TM::OFF_DELAY);
    digitalWrite(sensorPowerPin,HIGH);    //switch sensor on and read the data
    delay(Decagon5TM::ON_DELAY);    
}

void Decagon5TM::serial5TMEvent()
{
    // Read serial Data available of Sensor in sensorStream C array
    char sensorStream[Decagon5TM::MAX_STRING_SIZE] = "";   
    byte size = serial5TM->readBytes(sensorStream, Decagon5TM::MAX_STRING_SIZE);
    
    sensorStream[size] = 0;                // Add the final 0 to end the C array

    // Split the sensorStream into rawDielectric & rawTemperature C array
    char rawDielectric[Decagon5TM::RAW_DIELECTIRC_MAX_SIZE] = "";
    char rawTemperature[Decagon5TM::RAW_TEMPERATURE_MAX_SIZE] = "";

    uint8_t i;
    uint8_t nullOffset = 0;
    
    for( uint8_t j = 0; j < size; ++j)
        if((uint8_t)sensorStream[j] == 0)
            ++nullOffset;           // Ignore leading 'NULL' in string
        else
            break;
    
    for(i = nullOffset; i < size; ++i)
        if( sensorStream[i] != ' ')
            rawDielectric[i-nullOffset] = sensorStream[i];
        else
        {
            rawDielectric[i-nullOffset] = NULL;
            i+=3;      // Skips two chaarcters
            break;
        }

    for( uint8_t j = 0; i < size; ++i) 
        if( sensorStream[i] !=  '\r')
            rawTemperature[j++] = sensorStream[i];
        else
        {
            rawTemperature[j] = NULL;
            sensorType = sensorStream[++i];
            checksumOfPacket = sensorStream[++i];   
            break;      
        }
    // Calculate Checksum
    checksumCalculated = computeChecksum( &sensorStream[1], --i);
            
    // Convert rawDielectric & rawTemperature C array to uint16_t 
    rawDielectricInt = atoi(rawDielectric);
    rawTemperatureInt = atoi(rawTemperature);
    ifNewDataAvailable = true;
}

char Decagon5TM::computeChecksum(char * response, uint8_t length)
{
    //calculate the checksum of the received data
    int sum = 0, i;
    uint8_t crcsum;
    crcsum = 0;
    for (i = 0; i < length; i++)
        sum += response[i];
    crcsum = sum % 64 + 32;
    return crcsum;
}

uint16_t Decagon5TM::getDielectric() 
{  
    ifNewDataAvailable = false;
    return rawDielectricInt;  
}

uint16_t Decagon5TM::getTemperature()
{
    ifNewDataAvailable = false;
    return rawTemperatureInt; 
}
bool Decagon5TM::getChecksum()
{  
    ifNewDataAvailable = false; 
    return checksumOfPacket & checksumCalculated;  
}

void Decagon5TM::serialEventRun(void) {
    if (serial5TM->available()) serial5TMEvent();
}

void Decagon5TM::debugSensorData()
{
    if( Serial ) {
        Serialprint("[Dielectric %d] ",         rawDielectricInt);
        Serialprint("[Temperature %d] ",        rawTemperatureInt);
        Serialprint("[SensorTyp %d] ",          sensorType);
        Serialprint("[checksumOfPacket %d] ",   checksumOfPacket);
        Serialprint("[checksumCalculated %d] ", checksumCalculated);
    }
}