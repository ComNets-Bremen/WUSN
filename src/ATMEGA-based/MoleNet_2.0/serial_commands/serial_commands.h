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
 *                  Header File.     
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

#ifndef MOLENET_SERIAL_COMMANDS_HPP_INCLUDED
#define MOLENET_SERIAL_COMMANDS_HPP_INCLUDED

#if defined (__AVR__) || (__avr__)
    #define PASS_INFO_TO_GUI
    #include <Arduino.h>
#else
    #include <vector>
    #include <string>    
    #include <map>
    #define PASS_INFO_TO_GUI \
        struct GuiInfo { bool requireDestID; std::string text; }; \
        static const std::map<Command::Type, Command::GuiInfo> guiInfo;

#endif

struct Command {
    enum Type {

        DEBUG_NODE_INFO = 0,
        DEBUG_COMPLETE_EEPROM,
        DEBUG_STARTUP_CONFIG,
        DEBUG_RX_TX_INFORMATION,
        
        SEND_NEW_DECAGON5TM_DATA_DATA_TO_NODE,        
        SEND_NEW_MOISTURE_TEMP_DATA_TO_NODE,
        SEND_NEW_DATA_PLUS_INFO_TO_NODE,
        SEND_RX_TX_INFORMATION,
        SEND_STARTUP_CONFIG_TO_NODE,        
        
        REQ_NEW_DECAGON5TM_DATA,
        REQ_NEW_MOISTURE_TEMP_DATA,
        REQ_NEW_DATA_PLUS_INFO,
        REQ_RX_TX_INFORMATION,             
        REQ_STATUP_CONFIG,   
        
        REQ_COMPLETE_EEPROM,
        SEND_COMPLETE_EEPROM,    

        BROADCAST_SYNCHRONIZE_SET_TIME_ALARM_CLEAN_EEPROM,
        APPLY_NEW_TIME,
        CLEANUP_EEPROM
    };   
    PASS_INFO_TO_GUI
};

#if defined (__AVR__) || (__avr__)
    class SerialCommandHandler {
        public:
            template<class T>
            T readSerial();
            
            template<class T>
            T readSerial(const T &MIN, const T &MAX);
            virtual void serialCmdLoop() = 0;
    };
    
    // Lesson of 8 hours of debug
    // Never use 115200 if reading from serial port on 8MHz 3.3V arduino
    template<class T>
    T SerialCommandHandler::readSerial() {
        while(Serial.available() > 0) {
            T value = Serial.parseInt();
            if(Serial.read() == '\n')
            return value;
        }
        return -1;
    }    
    
    template<class T>
    T SerialCommandHandler::readSerial(const T &MIN, const T &MAX) {
        T cmd = -1;
        Serial.print("Value ");
        while( 1 ) {
            cmd = readSerial<T>();
            if( cmd != -1 ) {
                if(cmd < MIN || cmd > MAX){
                    Serial.print("Invalid input: Range("); Serial.print(MIN); Serial.print("-");Serial.print(MAX);
                    Serial.print(")");
                }
                else {
                    Serial.println(cmd);
                    return cmd;
                }
            }
        }
    }    
#endif

#endif // MOLENET_SERIAL_COMMANDS_HPP_INCLUDED