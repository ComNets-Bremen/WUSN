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
 *       Filename:  serial_command.h
 *
 *    Description:  This class provides the functionality to read numbers
 *                  from Serial port. It also defines serial commands
 *                  for MoleNet_Node class. This class can also be 
 *                  used for GUI interface to get names of commands.  
 *                  Implementation File.    
 *
 *        Version:  1.0
 *        Created:  14/08/2016 10:00:00 
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



#if defined (__AVR__) || (__avr__)
    #warning "TEXT NOT ADDED"
    #include "serial_commands.h"
#else
    #include "serial_commands/serial_commands.h"
    
    const std::map<Command::Type, Command::GuiInfo> Command::guiInfo = {
//        {Command::HELP,                             true},
        {Command::DEBUG_NODE_INFO,                                      { false, "DEBUG_NODE_INFO" } },
        {Command::DEBUG_COMPLETE_EEPROM,                                { false, "DEBUG_COMPLETE_EEPROM" } },
        {Command::DEBUG_STARTUP_CONFIG,                                 { false, "DEBUG_STARTUP_CONFIG" } },
        {Command::DEBUG_RX_TX_INFORMATION,                              { false, "DEBUG_RX_TX_INFORMATION" } },
        {Command::SEND_NEW_DECAGON5TM_DATA_DATA_TO_NODE,                { true,  "SEND_NEW_DECAGON5TM_DATA_DATA_TO_NODE" } },
        {Command::SEND_NEW_MOISTURE_TEMP_DATA_TO_NODE,                  { true,  "SEND_NEW_MOISTURE_TEMP_DATA_TO_NODE" } },
        {Command::SEND_NEW_DATA_PLUS_INFO_TO_NODE,                      { true,  "SEND_NEW_DATA_PLUS_INFO_TO_NODE" } },
        {Command::SEND_RX_TX_INFORMATION,                               { true,  "SEND_RX_TX_INFORMATION" } },
        {Command::SEND_STARTUP_CONFIG_TO_NODE,                          { true,  "SEND_STARTUP_CONFIG_TO_NODE" } },
        {Command::REQ_NEW_DECAGON5TM_DATA,                              { true,  "REQ_NEW_DECAGON5TM_DATA" } },
        {Command::REQ_NEW_MOISTURE_TEMP_DATA,                           { true,  "REQ_NEW_MOISTURE_TEMP_DATA" } },
        {Command::REQ_NEW_DATA_PLUS_INFO,                               { true,  "REQ_NEW_DATA_PLUS_INFO" } },
        {Command::REQ_RX_TX_INFORMATION,                                { true,  "REQ_RX_TX_INFORMATION" } },
        {Command::REQ_STATUP_CONFIG,                                    { true,  "REQ_STATUP_CONFIG" } },
        {Command::REQ_COMPLETE_EEPROM,                                  { true,  "REQ_COMPLETE_EEPROM" } },
        {Command::SEND_COMPLETE_EEPROM,                                 { true,  "SEND_COMPLETE_EEPROM" } },
        {Command::BROADCAST_SYNCHRONIZE_SET_TIME_ALARM_CLEAN_EEPROM,    { true,  "BROADCAST_SYNCHRONIZE_SET_TIME_ALARM_CLEAN_EEPROM" } },
        {Command::APPLY_NEW_TIME,                                       { true,  "APPLY_NEW_TIME" } },
        {Command::CLEANUP_EEPROM,                                       { false, "CLEANUP_EEPROM" } },
    };
#endif