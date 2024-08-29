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
 *       Filename:  message_types.hpp
 *
 *    Description:  Strucure of messages of different kinds.    
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


#ifndef MESSAGE_TYPES_HPP_INCLUDED
#define MESSAGE_TYPES_HPP_INCLUDED

#include <stdarg.h>  //variable number of function arguments

// namespace Molenet 


#if defined (__AVR__) || (__avr__)
    #include "rfm69_mh.h"
#else
    #define Serial 1
    #include <inttypes.h>

    class RFM69{};

    class RFM69mh : RFM69{
    public:
        int getAddress(){}
        unsigned char *payLoad;
        int txPacketCount, lostPacketCount, RSSI;
    };
    #include "serial_print/serial_print.h"
    #include "Time-master/Time.h"
#endif
// {   
    struct Packet {
        enum Type {
            typ_UNDEFINED,
            
            typ_DECAGON5TM_SENSOR, 
            typ_MOISTURE_TEMP_SENSOR,
            typ_DATA_PLUS_INFO,
            typ_RX_TX_INFORMATION,
            typ_STARTUP_CONFIG,
            
            typ_REQ_DECAGON5TM_SENSOR,
            typ_REQ_MOISTURE_TEMP_SENSOR,
            typ_REQ_DATA_PLUS_INFO,
            typ_REQ_RX_TX_INFORMATION,
            typ_REQ_STARTUP_CONFIG,
            
            typ_REQ_COMPLETE_EEPROM_DATA,  
        };
    };
    
    class IMessage {      
    protected:
        RFM69mh *radio;    
    public:
        IMessage(RFM69mh *rfm) : radio(rfm) { }

        void fillHeader(const uint8_t &sinkID)
        {   radio->payLoad[pos_SINK_ID] = sinkID;  } 
        virtual void fillData( uint32_t count, ... ) = 0;
        
        virtual void debug_Message(const uint8_t &startPositon) = 0;
        void debug_Header_AND_Message();
        void debug_Header();
        

        enum Header {
            pos_SINK_ID,
            SIZE,
            DATA_POS_START = SIZE
        };
    };
    
    class RequestPacket : public IMessage {
        public:
            RequestPacket(RFM69mh *rfm) : IMessage(rfm) { }
            virtual void debug_Message(const uint8_t &startPositon);
            virtual void fillData( uint32_t count, ... );
            enum Position {
                pos_PACKET_TYPE,
                SIZE,
                DATA_SIZE = SIZE+Header::SIZE
            };  
    };

    class Decagon5TMData : public IMessage {
        public:
            Decagon5TMData(RFM69mh *rfm) : IMessage(rfm) { }
            virtual void debug_Message(const uint8_t &startPositon);
            virtual void fillData( uint32_t count, ... );
            enum Position {
                pos_PACKET_TYPE,
                pos_PACKET_LENGTH,
                pos_SOURCE_ID,
                pos_EPOCH_0, //Header::SIZE, // Position of year in payload
                pos_EPOCH_1,               // Position of month in payload
                pos_EPOCH_2,               // Position of day in payload
                pos_EPOCH_3,               // Position of hour in payload
                pos_DIELECTRIC_0,            // Start position for VWC value in payload
                pos_DIELECTRIC_1,
                pos_TEMP_0,            // Start position for temperature value in payload
                pos_TEMP_1,
                SIZE,                            // Size of Payload
                DATA_SIZE = SIZE+Header::SIZE
                //MAX_DATA_PACKETS_IN_SINGLE_PAYLOAD = Radio::BUFFER_SIZE / DATA_SIZE
            }; 
    };    
    
    class MoisutreTempData : public IMessage {
        public:
            MoisutreTempData(RFM69mh *rfm) : IMessage(rfm) { }
            virtual void debug_Message(const uint8_t &startPositon);
            virtual void fillData( uint32_t count, ... );
            enum Position {
                pos_PACKET_TYPE,
                pos_PACKET_LENGTH,
                pos_SOURCE_ID,
                pos_EPOCH_0, //Header::SIZE, // Position of year in payload
                pos_EPOCH_1,               // Position of month in payload
                pos_EPOCH_2,               // Position of day in payload
                pos_EPOCH_3,               // Position of hour in payload
                pos_MOISTURE_0,            // Start position for VWC value in payload
                pos_MOISTURE_1,
                pos_TEMP_0,            // Start position for temperature value in payload
                pos_TEMP_1,
                SIZE,                            // Size of Payload
                DATA_SIZE = SIZE+Header::SIZE   //Header::SIZE,
                //MAX_DATA_PACKETS_IN_SINGLE_PAYLOAD = Radio::BUFFER_SIZE / DATA_SIZE
            };
    }; 
    
    class DATA_PLUS_INFO : public IMessage {
        public:
            DATA_PLUS_INFO(RFM69mh *rfm) : IMessage(rfm) { }
            virtual void debug_Message(const uint8_t &startPositon);
            virtual void fillData( uint32_t count, ... );
            enum Position {
                pos_PACKET_TYPE,
                pos_PACKET_LENGTH,
                pos_SOURCE_ID,
                pos_EPOCH_0, //Header::SIZE, // Position of year in payload
                pos_EPOCH_1,               // Position of month in payload
                pos_EPOCH_2,               // Position of day in payload
                pos_EPOCH_3,               // Position of hour in payload
                pos_DIELECTRIC_0,            // Start position for VWC value in payload
                pos_DIELECTRIC_1,
                pos_TEMP_0,            // Start position for temperature value in payload
                pos_TEMP_1,
                pos_PACKETS_SENT_0,
                pos_PACKETS_SENT_1,
                pos_SENDING_RETERIES,
                pos_PACKETS_LOST_0,
                pos_PACKETS_LOST_1,
                pos_RTT_0,
                pos_RTT_1,
                pos_RSSI_0,
                pos_RSSI_1,
                pos_EEPROM_CURRENT_PAGE_0,
                pos_EEPROM_CURRENT_PAGE_1,
                pos_EEPROM_CURRENT_PAGE_2,      //24 bit unsigned int to store max 131072 pages for 1 Mbyte        
                SIZE,                            // Size of Payload
                DATA_SIZE = SIZE+Header::SIZE,   //Header::SIZE,
            };
    };

    class RX_TX_INFORMATION : public IMessage {
        public:
            RX_TX_INFORMATION(RFM69mh *rfm) : IMessage(rfm) { }
            virtual void debug_Message(const uint8_t &startPositon);
            virtual void fillData( uint32_t count, ... );
            enum Position {
                pos_PACKET_TYPE,
                pos_PACKET_LENGTH,
                pos_SOURCE_ID, 
                pos_EPOCH_0, //Header::SIZE, // Position of year in payload
                pos_EPOCH_1,               // Position of month in payload
                pos_EPOCH_2,               // Position of day in payload
                pos_EPOCH_3,               // Position of hour in payload        
                pos_PACKETS_SENT_0,
                pos_PACKETS_SENT_1,
                pos_SENDING_RETERIES,
                pos_PACKETS_LOST_0,
                pos_PACKETS_LOST_1,
                pos_RTT_0,
                pos_RTT_1,
                pos_RSSI_0,
                pos_RSSI_1,
                pos_EEPROM_CURRENT_PAGE_0,
                pos_EEPROM_CURRENT_PAGE_1,
                pos_EEPROM_CURRENT_PAGE_2,      //24 bit unsigned int to store max 131072 pages for 1 Mbyte
                SIZE,
                DATA_SIZE = SIZE+Header::SIZE      
            };
    }; 
    
    class StartupConfig : public IMessage {
        public:
            StartupConfig(RFM69mh *rfm) : IMessage(rfm) { }
            virtual void debug_Message(const uint8_t &startPositon);
            virtual void fillData( uint32_t count, ... );
            void fillData(const uint8_t &startPosition, unsigned char config[], const uint8_t &size);
            enum CONROL_1 {      //Non appylable Configuration
                pos_SET_NODEID,
                pos_SET_GATEWAYID,
                pos_SET_SINKID,
                
                pos_SET_DELAY_BEFORE_SEND,
                pos_SET_DELAY_BEFORE_SLEEP,
                pos_SET_DELAY_AFTER_RECEIVE,
                pos_CLEAR_COUNT_VARIABLES
            };
            
            enum CONTROL_2 {     // Applyable Configurations
                pos_STATE_ALARM_HOUR,
                pos_STATE_ALARM_MINUTE,
                pos_STATE_ALARM_DAY,
                pos_STATE_ALARM_WEEKDAY,
                pos_STATE_ALARM_INCREMENTAL_MINUTE,
                
                pos_SET_ALARM,
                pos_SET_RTC,
                pos_SET_CLEANUP_EEPROM
            };
            
            enum Position {
                pos_PACKET_TYPE,
                pos_PACKET_LENGTH,
                pos_SOURCE_ID,    
             
                pos_CONTROL_1,
                pos_CONTROL_2,
                
                pos_NODEID,
                pos_GATEWAYID,
                pos_SINKID,
                
                pos_ALARM_HOUR,
                pos_ALARM_MINUTE,
                pos_ALARM_DAY,
                pos_ALARM_WEEKDAY,
                
                pos_EPOCH_0,
                pos_EPOCH_1,
                pos_EPOCH_2,
                pos_EPOCH_3,
                
                pos_DELAY_BEFORE_SEND,
                pos_DELAY_BEFORE_SLEEP,
                pos_DELAY_AFTER_RECEIVE,
                SIZE,
                DATA_SIZE = SIZE+Header::SIZE      
            };
    };     
    /*
}*/
#endif  // MESSAGE_TYPES_HPP_INCLUDED
