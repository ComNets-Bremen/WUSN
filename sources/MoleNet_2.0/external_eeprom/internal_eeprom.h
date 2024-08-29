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
 *    Description:  The library will contain internal EEPROM 
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

#ifndef MOLENET_INTERNAL_EEPROM_HPP_INCLUDED
#define MOLENET_INTERNAL_EEPROM_HPP_INCLUDED

#include "external_eeprom.h"
#include "message_types.h"
#include <EEPROM.h>  // Internal EEPROM

class InternalEEPROM
{
    public:
        InternalEEPROM(ExternalEEPROM *ext);
        void saveStartupConfig();
        void loadStartupConfig();
        virtual void applyStartupConfig() = 0;
        virtual void overwriteConfig(unsigned char *_config) =0;
        unsigned char config[StartupConfig::SIZE];    
        
    private:
        ExternalEEPROM *externalEEPROM;
};

#endif //  MOLENET_INTERNAL_EEPROM_HPP_INCLUDED