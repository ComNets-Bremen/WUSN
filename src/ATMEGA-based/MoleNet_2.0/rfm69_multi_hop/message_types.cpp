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
 *       Filename:  message_types.cpp
 *
 *    Description:  Strucure of messages of different kinds.  
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

#include "message_types.h"

void IMessage::debug_Header_AND_Message() 
{
    this->debug_Header();
    this->debug_Message(DATA_POS_START);
}

void IMessage::debug_Header() 
{
    if(Serial) {
        Serialprint("{**HEADER [sinkID %d] ", radio->payLoad[pos_SINK_ID]);
        Serialprint("[sourceID %d] HEADER**}",radio->getAddress()  );    
    }   
}

///////////////////////////////
///// RequestPacket ///////////
///////////////////////////////
void RequestPacket::fillData( uint32_t count, ... ) 
{
    // Deduct arguments
    va_list arguments; 
    va_start ( arguments, count ); 
    uint8_t startPositon = va_arg( arguments, uint32_t );
    unsigned char type =   va_arg( arguments, uint32_t );
    va_end ( arguments );    // Cleans up the list 
    
    // fill payload
    radio->payLoad[startPositon+RequestPacket::pos_PACKET_TYPE] = type;         
}

void RequestPacket::debug_Message(const uint8_t &startPositon)
{
    if(Serial) {
        unsigned char type = radio->payLoad[ startPositon+RequestPacket::pos_PACKET_TYPE];
        
        Serialprint(" {**RequestPacket ");
        Serialprint("[type %d]", type);
        Serialprint("RequestPacket**}");
    }    
}

///////////////////////////////
///// Decagon5TMData //////////////
///////////////////////////////
void Decagon5TMData::fillData( uint32_t count, ... ) 
{
    // Deduct arguments
    va_list arguments; 
    va_start ( arguments, count ); 
    uint8_t startPositon = va_arg( arguments, uint32_t );
    uint32_t time =        va_arg( arguments, uint32_t );
    uint16_t dielectric =  va_arg( arguments, uint32_t );
    uint16_t temperature = va_arg( arguments, uint32_t );
    va_end ( arguments );    // Cleans up the list 

    // fill payload
    radio->payLoad[startPositon+Decagon5TMData::pos_PACKET_TYPE] =   static_cast<unsigned char>(Packet::typ_DECAGON5TM_SENSOR); 
    radio->payLoad[startPositon+Decagon5TMData::pos_PACKET_LENGTH] = Decagon5TMData::SIZE; 
    radio->payLoad[startPositon+Decagon5TMData::pos_SOURCE_ID] =     radio->getAddress();
    radio->payLoad[startPositon+Decagon5TMData::pos_EPOCH_0] =       (time >> 24) & 0xFF;
    radio->payLoad[startPositon+Decagon5TMData::pos_EPOCH_1] =       (time >> 16) & 0xFF;
    radio->payLoad[startPositon+Decagon5TMData::pos_EPOCH_2] =       (time >> 8) & 0xFF;
    radio->payLoad[startPositon+Decagon5TMData::pos_EPOCH_3] =       time & 0xFF;
    radio->payLoad[startPositon+Decagon5TMData::pos_DIELECTRIC_0] =  dielectric >> 8;
    radio->payLoad[startPositon+Decagon5TMData::pos_DIELECTRIC_1] =  dielectric & 0xFF;
    radio->payLoad[startPositon+Decagon5TMData::pos_TEMP_0] =        temperature >> 8;
    radio->payLoad[startPositon+Decagon5TMData::pos_TEMP_1] =        temperature & 0xFF;           
}

void Decagon5TMData::debug_Message(const uint8_t &startPositon)
{
    if(Serial) {
        uint32_t time =        (uint32_t)radio->payLoad[ startPositon+Decagon5TMData::pos_EPOCH_0] << 24 |
                               (uint32_t)radio->payLoad[ startPositon+Decagon5TMData::pos_EPOCH_1] << 16 |
                               (uint32_t)radio->payLoad[ startPositon+Decagon5TMData::pos_EPOCH_2] << 8  |
                               (uint32_t)radio->payLoad[ startPositon+Decagon5TMData::pos_EPOCH_3]; 

        
        uint16_t dielectric =  (uint16_t)radio->payLoad[ startPositon+Decagon5TMData::pos_DIELECTRIC_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+Decagon5TMData::pos_DIELECTRIC_1];
                            
        uint16_t temperature = (uint16_t)radio->payLoad[ startPositon+Decagon5TMData::pos_TEMP_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+Decagon5TMData::pos_TEMP_1];    
                      
        TimeElements tm;
        breakTime(time, tm);        
        Serialprint(" {**Decagon5TMData ");
        Serialprint("[Length %d] ",                       radio->payLoad[startPositon+Decagon5TMData::pos_PACKET_LENGTH] );         
        Serialprint("[sourceID %d]",                      radio->payLoad[startPositon+Decagon5TMData::pos_SOURCE_ID]);
        Serialprint("[RTC %"SCNd32" %d/%d/%d %d:%d:%d] ", time, tm.Day, tm.Month, tm.Year, tm.Hour, tm.Minute, tm.Second );
        Serialprint("[dielectric %d] [temperature %d] ",  dielectric, temperature );
        Serialprint("Decagon5TMData**}");
    }    
}

///////////////////////////////
///// MoisutreTempData ////////
///////////////////////////////
void MoisutreTempData::fillData( uint32_t count, ... ) 
{
    // Deduct arguments
    va_list arguments; 
    va_start ( arguments, count ); 
    uint8_t startPositon = va_arg( arguments, uint32_t );
    uint32_t time =        va_arg( arguments, uint32_t );
    uint16_t moisture =    va_arg( arguments, uint32_t );
    uint16_t temperature = va_arg( arguments, uint32_t );
    va_end ( arguments );    // Cleans up the list 

    // fill payload
    radio->payLoad[startPositon+MoisutreTempData::pos_PACKET_TYPE] =   static_cast<unsigned char>(Packet::typ_MOISTURE_TEMP_SENSOR); 
    radio->payLoad[startPositon+MoisutreTempData::pos_PACKET_LENGTH] = MoisutreTempData::SIZE; 
    radio->payLoad[startPositon+MoisutreTempData::pos_SOURCE_ID] =     radio->getAddress();
    radio->payLoad[startPositon+MoisutreTempData::pos_EPOCH_0] =       (time >> 24) & 0xFF;
    radio->payLoad[startPositon+MoisutreTempData::pos_EPOCH_1] =       (time >> 16) & 0xFF;
    radio->payLoad[startPositon+MoisutreTempData::pos_EPOCH_2] =       (time >> 8) & 0xFF;
    radio->payLoad[startPositon+MoisutreTempData::pos_EPOCH_3] =       time & 0xFF;
    radio->payLoad[startPositon+MoisutreTempData::pos_MOISTURE_0] =    moisture >> 8;
    radio->payLoad[startPositon+MoisutreTempData::pos_MOISTURE_1] =    moisture & 0xFF;
    radio->payLoad[startPositon+MoisutreTempData::pos_TEMP_0] =        temperature >> 8;
    radio->payLoad[startPositon+MoisutreTempData::pos_TEMP_1] =        temperature & 0xFF;           
}

void MoisutreTempData::debug_Message(const uint8_t &startPositon)
{
    if(Serial) {
        uint32_t time =        (uint32_t)radio->payLoad[ startPositon+MoisutreTempData::pos_EPOCH_0] << 24 |
                               (uint32_t)radio->payLoad[ startPositon+MoisutreTempData::pos_EPOCH_1] << 16 |
                               (uint32_t)radio->payLoad[ startPositon+MoisutreTempData::pos_EPOCH_2] << 8  |
                               (uint32_t)radio->payLoad[ startPositon+MoisutreTempData::pos_EPOCH_3]; 

        
        uint16_t moisture =    (uint16_t)radio->payLoad[ startPositon+MoisutreTempData::pos_MOISTURE_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+MoisutreTempData::pos_MOISTURE_1];
                            
        uint16_t temperature = (uint16_t)radio->payLoad[ startPositon+MoisutreTempData::pos_TEMP_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+MoisutreTempData::pos_TEMP_1];    
                      
        TimeElements tm;
        breakTime(time, tm);        
        Serialprint(" {**MoisutreTempData ");
        Serialprint("[Length %d] ",                      radio->payLoad[startPositon+MoisutreTempData::pos_PACKET_LENGTH] );         
        Serialprint("[sourceID %d]",                     radio->payLoad[startPositon+MoisutreTempData::pos_SOURCE_ID]);
        Serialprint("[RTC %"SCNd32" %d/%d/%d %d:%d:%d] ",time, tm.Day, tm.Month, tm.Year, tm.Hour, tm.Minute, tm.Second );
        Serialprint("[moisture %d] [temperature %d] ",   moisture, temperature );
        Serialprint("MoisutreTempData**}");
    }    
}

///////////////////////////////
///// DATA_PLUS_INFO   ////////
///////////////////////////////
void DATA_PLUS_INFO::fillData( uint32_t count, ... ) 
{
    // Deduct arguments
    va_list arguments; 
    va_start ( arguments, count ); 
    uint8_t startPositon = va_arg( arguments, uint32_t );
    uint32_t time =        va_arg( arguments, uint32_t );
    uint16_t dielectric =  va_arg( arguments, uint32_t );
    uint16_t temperature = va_arg( arguments, uint32_t );
    uint32_t pageAddress = va_arg( arguments, uint32_t );
    va_end ( arguments );    // Cleans up the list 

    // fill payload
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKET_TYPE] =           static_cast<unsigned char>(Packet::typ_DATA_PLUS_INFO); 
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKET_LENGTH] =         DATA_PLUS_INFO::SIZE;     
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_SOURCE_ID] =             radio->getAddress();
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_0] =               (time >> 24) & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_1] =               (time >> 16) & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_2] =               (time >> 8) & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_3] =               time & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_DIELECTRIC_0] =          dielectric >> 8;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_DIELECTRIC_1] =          dielectric & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_TEMP_0] =                temperature >> 8;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_TEMP_1] =                temperature & 0xFF;   
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EEPROM_CURRENT_PAGE_0] = (pageAddress >> 16) & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EEPROM_CURRENT_PAGE_1] = (pageAddress >> 8) & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EEPROM_CURRENT_PAGE_2] = (pageAddress >> 0) & 0xFF;
    
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKETS_SENT_0] =   radio->txPacketCount >> 8;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKETS_SENT_1] =   radio->txPacketCount & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_SENDING_RETERIES] = radio->RFM69::RETERIES;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKETS_LOST_0] =   radio->lostPacketCount >> 8;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKETS_LOST_1] =   radio->lostPacketCount & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_RTT_0] =            radio->RFM69::RTT >> 8;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_RTT_1] =            radio->RFM69::RTT & 0xFF;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_RSSI_0] =           radio->RSSI >> 8;
    radio->payLoad[startPositon+DATA_PLUS_INFO::pos_RSSI_1] =           radio->RSSI & 0xFF;    

}

void DATA_PLUS_INFO::debug_Message(const uint8_t &startPositon)
{
    if(Serial) {        
        uint32_t time =        (uint32_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_0] << 24 |
                               (uint32_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_1] << 16 |
                               (uint32_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_2] << 8  |
                               (uint32_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_EPOCH_3]; 

            
        uint16_t dielectric =  (uint16_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_DIELECTRIC_0] << 8 |
                               (uint16_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_DIELECTRIC_1];
                                
        uint16_t temperature = (uint16_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_TEMP_0] << 8 |
                               (uint16_t)radio->payLoad[startPositon+DATA_PLUS_INFO::pos_TEMP_1];     

        uint16_t packetsSent = (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_PACKETS_SENT_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_PACKETS_SENT_1];
                                
        uint16_t packetLost =  (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_PACKETS_LOST_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_PACKETS_LOST_1];  
                          
        uint16_t RTT =         (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_RTT_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_RTT_1];
                                
        int16_t RSSI =         radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_RSSI_0] << 8 |
                               radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_RSSI_1];    
         
        uint32_t eepromNextPage  = (uint16_t) 0 << 24 |  
                                   (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_EEPROM_CURRENT_PAGE_0] << 16 |
                                   (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_EEPROM_CURRENT_PAGE_1] << 8  |
                                   (uint16_t)radio->payLoad[ startPositon+DATA_PLUS_INFO::pos_EEPROM_CURRENT_PAGE_2];                               
            
        TimeElements tm;
        breakTime(time, tm);                          
        Serialprint(" {**DATA_PLUS_INFO ");
        Serialprint("[Length %d] ", radio->payLoad[startPositon+DATA_PLUS_INFO::pos_PACKET_LENGTH] );          
        Serialprint("[sourceID %d] ", radio->payLoad[startPositon+DATA_PLUS_INFO::pos_SOURCE_ID] );
        Serialprint("[RTC %"SCNd32" %d/%d/%d %d:%d:%d] ",time,tm.Day, tm.Month, tm.Year, tm.Hour, tm.Minute, tm.Second );
        Serialprint("[dielectric %d] [temperature %d] ",dielectric, temperature );
        Serialprint("[pktSent %d] ", packetsSent );
        Serialprint("[reteries %d] [pktLOST %d]",radio->payLoad[startPositon+ DATA_PLUS_INFO::pos_SENDING_RETERIES ], packetLost );
        Serialprint("[RTT %d] [RSSI %d] ",RTT, RSSI );
        Serialprint("[nextAddr %d] ",eepromNextPage );        
        Serialprint("DATA_PLUS_INFO**}");     
    }           
}

///////////////////////////////
///// RX_TX_INFORMATION////////
///////////////////////////////
void RX_TX_INFORMATION::fillData( uint32_t count, ... ) 
{
    // Deduct arguments
    va_list arguments; 
    va_start ( arguments, count ); 
    uint8_t startPositon = va_arg( arguments, uint32_t );
    uint32_t time = va_arg( arguments, uint32_t );
    uint32_t pageAddress = va_arg( arguments, uint32_t );
    va_end ( arguments );    // Cleans up the list 

    // fill payload
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKET_TYPE] = static_cast<unsigned char>(Packet::typ_RX_TX_INFORMATION); 
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKET_LENGTH] = RX_TX_INFORMATION::SIZE;    
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_SOURCE_ID] = radio->getAddress();
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_0] = (time >> 24) & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_1] = (time >> 16) & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_2] = (time >> 8) & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_3] =  time & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EEPROM_CURRENT_PAGE_0] = ( pageAddress >> 16 ) & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EEPROM_CURRENT_PAGE_1] = ( pageAddress >> 8 ) & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EEPROM_CURRENT_PAGE_2] = ( pageAddress >> 0 ) & 0xFF;
    
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKETS_SENT_0] = radio->txPacketCount >> 8;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKETS_SENT_1] = radio->txPacketCount & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_SENDING_RETERIES] = radio->RFM69::RETERIES;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKETS_LOST_0] = radio->lostPacketCount >> 8;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKETS_LOST_1] = radio->lostPacketCount & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_RTT_0] =  radio->RFM69::RTT >> 8;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_RTT_1] =  radio->RFM69::RTT & 0xFF;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_RSSI_0] = radio->RSSI >> 8;
    radio->payLoad[startPositon+RX_TX_INFORMATION::pos_RSSI_1] = radio->RSSI & 0xFF;    
}

void RX_TX_INFORMATION::debug_Message(const uint8_t &startPositon)
{
    if(Serial) {        
        uint32_t time = (uint32_t)radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_0] << 24 |
                        (uint32_t)radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_1] << 16 |
                        (uint32_t)radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_2] << 8  |
                        (uint32_t)radio->payLoad[startPositon+RX_TX_INFORMATION::pos_EPOCH_3]; 

        uint16_t packetsSent = (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_PACKETS_SENT_0] << 8 |
                               (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_PACKETS_SENT_1];
                                
        uint16_t packetLost = (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_PACKETS_LOST_0] << 8 |
                              (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_PACKETS_LOST_1];  
                          
        uint16_t RTT = (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_RTT_0] << 8 |
                       (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_RTT_1];
                                
        int16_t RSSI = radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_RSSI_0] << 8 |
                       radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_RSSI_1];    
         
        uint32_t eepromNextPage  = (uint16_t) 0 << 24 |  
                                   (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_EEPROM_CURRENT_PAGE_0] << 16 |
                                   (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_EEPROM_CURRENT_PAGE_1] << 8  |
                                   (uint16_t)radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_EEPROM_CURRENT_PAGE_2]; 
        TimeElements tm;
        breakTime(time, tm);   
                                   
        Serialprint(" {**INFORMATION ");
        Serialprint("[Length %d] ", radio->payLoad[startPositon+RX_TX_INFORMATION::pos_PACKET_LENGTH] );        
        Serialprint("[sourceID %d] ",radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_SOURCE_ID] );
        Serialprint("[RTC %"SCNd32" %d/%d/%d %d:%d:%d] ",time,tm.Day, tm.Month, tm.Year, tm.Hour, tm.Minute, tm.Second );
        Serialprint("[reteries %d] [pktSent %d] [pktLOST %d]",radio->payLoad[ startPositon+RX_TX_INFORMATION::pos_SENDING_RETERIES ], packetsSent, packetLost );
        Serialprint("[RTT %d] [RSSI %d] ",RTT, RSSI );
        Serialprint("[nextAddr %d] ",eepromNextPage );
        Serialprint("INFORMATION**}");
    }              
}

///////////////////////////////
///// StartupConfig    ////////
///////////////////////////////
void StartupConfig::fillData( uint32_t count, ... ) 
{
    Serialprint("Use other fillData function\n");
}

void StartupConfig::fillData(const uint8_t &startPositon, unsigned char config[], const uint8_t &SIZE)
{
    radio->payLoad[startPositon+StartupConfig::pos_PACKET_TYPE] = static_cast<unsigned char>(Packet::typ_STARTUP_CONFIG); 
    radio->payLoad[startPositon+StartupConfig::pos_PACKET_LENGTH] = StartupConfig::SIZE;     
    radio->payLoad[startPositon+StartupConfig::pos_SOURCE_ID] = radio->getAddress();
    
    for(uint8_t i = StartupConfig::pos_CONTROL_1; i < SIZE; ++i)   // SIZE must be StartupConfig::Size
        radio->payLoad[startPositon+i] = config[i];
    
    radio->payLoad[startPositon+StartupConfig::pos_CONTROL_1] = 0;  // clear control register in order to save any modifcation by requested node
    radio->payLoad[startPositon+StartupConfig::pos_CONTROL_2] = 0;    
}

void StartupConfig::debug_Message(const uint8_t &startPositon)
{
    if(Serial) {
        uint32_t epochTime = (uint32_t)radio->payLoad[startPositon+StartupConfig::pos_EPOCH_0] << 24 |
                             (uint32_t)radio->payLoad[startPositon+StartupConfig::pos_EPOCH_1] << 16 |
                             (uint32_t)radio->payLoad[startPositon+StartupConfig::pos_EPOCH_2] << 8  |
                             (uint32_t)radio->payLoad[startPositon+StartupConfig::pos_EPOCH_3];  
                        
        Serialprint(" {**STARTUPCONFIG ");
        Serialprint("[Length %d] ", radio->payLoad[startPositon+StartupConfig::pos_PACKET_LENGTH] );
        Serialprint("[nodeID %d ] [nextHopID %d] [sinkID %d] ",radio->payLoad[startPositon+StartupConfig::pos_NODEID], radio->payLoad[startPositon+StartupConfig::pos_GATEWAYID], radio->payLoad[startPositon+StartupConfig::pos_SINKID] );
        Serialprint("[delayBeforeSend %d] [delayBeforeSleep %d] ",radio->payLoad[startPositon+StartupConfig::pos_DELAY_BEFORE_SEND], radio->payLoad[startPositon+StartupConfig::pos_DELAY_BEFORE_SLEEP] );
        Serialprint("[Alarm %d|%d|%d|%d] ",radio->payLoad[startPositon+StartupConfig::pos_ALARM_WEEKDAY], radio->payLoad[startPositon+StartupConfig::pos_ALARM_DAY],
             radio->payLoad[startPositon+StartupConfig::pos_ALARM_HOUR], radio->payLoad[startPositon+StartupConfig::pos_ALARM_MINUTE] );
        Serialprint("[RTC %"SCNd32"] ", epochTime);
        Serialprint("STARTUPCONFIG**}");              
   }           
}