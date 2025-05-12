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
 *       Filename:  molenet_eeprom.h
 *
 *    Description:  The library will contain external EEPROM 
 *                  operations related to MoleNet Sensor Node.
 *                  Header File .        
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

#ifndef MOLENET_EXTERNAL_EEPROM_HPP_INCLUDED
#define MOLENET_EXTERNAL_EEPROM_HPP_INCLUDED

#define DEBUG
#include "serial_print.h"

#include "SPIEEP.h"
#include "rfm69_mh.h"

// 25LC1024 EEPROM
// Used to store Decagon5tmData Payloads
class ExternalEEPROM : public SPIEEP
{
    public:
        ExternalEEPROM(RFM69mh* _radio);
        void initialize(); 
        void cleanup();
        void savePacket(const uint8_t &startPos, const uint8_t &endPos);
        
        enum Settings {
            STARTADDRESS    = 0,
            TOTAL_SIZE      = 131072,        // Address Range max
            ADDR_WIDTH      = 24,
            EEPROM_CS_PIN   = 9
        };   
        uint32_t getNextAddress() {
            return nextAddress; }
            
        void setNextAddress(const uint32_t &addr) {
            nextAddress = addr;
        }
        
    protected:
        uint32_t nextAddress; 
        
    private:
        RFM69mh* radio;
};

#endif  // MOLENET_EXTERNAL_EEPROM_HPP_INCLUDED