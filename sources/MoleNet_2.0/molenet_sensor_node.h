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
 *       Filename:  molenet_sensor_node.h
 *
 *    Description:  The main library for MolneNet Sensor Node. It conatins 
 *                  all the functionalities of Multihop routing.
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

#ifndef MOLENET_SENSOR_NODE_HPP_INCLUDED
#define MOLENET_SENSOR_NODE_HPP_INCLUDED

#define DEBUG
#include "serial_print.h"

#include <avr/io.h>
#include <avr/power.h>


#include "LowPower.h"           //from: https://github.com/LowPowerLab/LowPower
#include "rv8523_clock_alarm.h"

#include "serial_commands.h"
#include "message_types.h"
#include "decagon_5tm.h"
#include "external_eeprom.h"
#include "internal_eeprom.h"

#define MOLENET_BAUD_RATE 19200

namespace Pin
{ 
    enum Settings {
        //EEPROM_CS       = 9,             // EEPROM CS        
        RTC_CS          = 8,            // RTC Power
        SENSOR_PW       = 7,            // Sensor Power
        INT             = 3,            // Pin for interrupt
        RXD             = 0,            // Serial receive pin
        TXD             = 1,            // Seriel transmit pin (only for debugging)
        RX_SS           = 8,
        TX_SS           = 11
    };
}    

struct MessageFactory {
    MessageFactory(RFM69mh *radio) : 
        requestPacket(radio),
        decagon5tm(radio),
        moistureTempData(radio),
        dataPlusInfo(radio),
        rxTxInfomration(radio),
        startupConfig(radio) 
    {   }
    
    RequestPacket requestPacket;
    Decagon5TMData decagon5tm;
    MoisutreTempData moistureTempData;
    DATA_PLUS_INFO dataPlusInfo;
    RX_TX_INFORMATION rxTxInfomration;
    StartupConfig startupConfig;  
};

class MoleNet_Node : public Decagon5TM,
                     public RFM69mh,
                     public RV8523_RTC,
                     public ExternalEEPROM,
                     public InternalEEPROM,
                     public SerialCommandHandler
{
    public:
        MoleNet_Node();
        void begin();

        // Messages Sending Functions
        void send_typ_DECAGON5TM_SENSOR(    const uint8_t &nextHopID, const uint8_t &sinkID, const bool &ifConcatinate=true);
        void send_typ_MOISTURE_TEMP_SENSOR( const uint8_t &nextHopID, const uint8_t &sinkID, const bool &ifConcatinate=true);
        void send_typ_DATA_PLUS_INFO(       const uint8_t &nextHopID, const uint8_t &sinkID, const bool &ifConcatinate=true);
        void send_typ_RX_TX_INFORMATION(    const uint8_t &nextHopID, const uint8_t &sinkID, const bool &ifConcatinate=true);       
        void send_typ_STARTUP_CONFIG(       const uint8_t &nextHopID, const uint8_t &sinkID );
        void send_typ_REQ_PACKET(           const uint8_t &nextHopID, Packet::Type packetType);

        // External EEPROM Function
        void send_CompleteEEPROM(           const uint8_t &nextHopID, const uint8_t &sinkID);
        void read_CompleteEEPROM();
        void cleanupEEPROM();

        // Internal EEPROM Function
        virtual void applyStartupConfig();
        virtual void overwriteConfig(unsigned char *_config);      
        
        // Pure Virtual Function Implementation
        virtual void serialCmdLoop();           // from SerialCommandHandler
        virtual void processReceivedData();         
        
        // Misc Functions
        void debugNodeInfo();
        void sleepMode();      
        MessageFactory messages; 

     private:
        void initializeIO();
        void initializeSerial();
        const clock_div_t CLOCKPRESC;   // Reduce -energy consumption, baudrate and delays need to be set accordingly! 
               
        
        uint8_t concatenate(uint8_t currentPayLoadLength);
        bool transmitOrSave(const uint8_t &nextHop, const uint8_t &pktSize, const bool &ifConcatinate);
        void debug_Payload(const uint8_t &POSITION);             
 };

#endif  // MOLENET_SENSOR_NODE_HPP_INCLUDED