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
 *       Filename:  molenet_eeprom.cpp
 *
 *    Description:  The library will contain external EEPROM 
 *                  operations related to MoleNet Sensor Node.
 *                  Implementation File.         
 *
 *        Version:  1.0
 *        Created:  14/08/2016 19:10:17 
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

#include "external_eeprom.h"

ExternalEEPROM::ExternalEEPROM(RFM69mh* _radio)
    : SPIEEP(ExternalEEPROM::ADDR_WIDTH, 9, ExternalEEPROM::TOTAL_SIZE),
      nextAddress(ExternalEEPROM::STARTADDRESS),
      radio(_radio)      
{
}

void ExternalEEPROM::initialize()
{
    SPIEEP::begin_spi(ExternalEEPROM::EEPROM_CS_PIN);   
}

void ExternalEEPROM::cleanup()
{
    SPIEEP::chip_erase();
    nextAddress = STARTADDRESS;

    if( Serial ) {
        Serialprint("[EEPROM Cleaned]\n");
    }
}

void ExternalEEPROM::savePacket(const uint8_t &startPos, const uint8_t &endPos)
{
    for( uint8_t index = 0; index < (endPos-startPos); ++index ) {
        SPIEEP::write(nextAddress+index, radio->payLoad[endPos-1-index]);  
        // save in reverse order
        // so that nextAddress-1 contain type, nextAddress-2 contain length
    }
    
    nextAddress += (endPos-startPos);
}