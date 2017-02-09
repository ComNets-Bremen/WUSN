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
 *       Filename:  molenet_sensor_node.cpp
 *
 *    Description:  The main library for MolneNet Sensor Node. It conatins 
 *                  all the functionalities of Multihop routing.
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

#include "molenet_sensor_node.h"

MoleNet_Node::MoleNet_Node() :
    Decagon5TM(Pin::SENSOR_PW,Pin::RX_SS, Pin::TX_SS ),
    CLOCKPRESC(clock_div_1), 
    ExternalEEPROM( static_cast<RFM69mh*>(this)),
    InternalEEPROM( static_cast<ExternalEEPROM*>(this)),
    messages( static_cast<RFM69mh*>(this) )
{
}

void MoleNet_Node::begin()
{
    loadStartupConfig();
   
    initializeSerial();
    initializeIO();
    RV8523_RTC::initializeRTC();   
    RFM69mh::initialize(config[StartupConfig::pos_NODEID]);
    ExternalEEPROM::initialize();
    
   applyStartupConfig(); 
}

void MoleNet_Node::send_typ_DECAGON5TM_SENSOR(const uint8_t &nextHop, const uint8_t &sinkID, const bool &ifConcatinate)
{
    if( !Decagon5TM::getChecksum() ) {          // if checksum is correct, only then proceed
        PRINT_ERROR_SENSOR_CHECKSUM_INCORRECT
        return;
    }

    PRINT_PACKET_SENT( RFM69::_address, nextHop)
    messages.decagon5tm.fillHeader( sinkID );    
    messages.decagon5tm.fillData( (uint32_t)4 , 
                                  (uint32_t)Decagon5TMData::DATA_POS_START,
                                  (uint32_t)RV8523_RTC::getClockTime(),
                                  (uint32_t)Decagon5TM::getDielectric(),
                                  (uint32_t)Decagon5TM::getTemperature() );
    messages.decagon5tm.debug_Header_AND_Message();

    transmitOrSave(nextHop, Decagon5TMData::DATA_SIZE, ifConcatinate);         
}

void MoleNet_Node::send_typ_MOISTURE_TEMP_SENSOR(const uint8_t &nextHop, const uint8_t &sinkID, const bool &ifConcatinate)
{
    PRINT_PACKET_SENT( RFM69::_address, nextHop)    
    messages.moistureTempData.fillHeader( sinkID );    
    messages.moistureTempData.fillData( 
                                  (uint32_t)4 , 
                                  (uint32_t)MoisutreTempData::DATA_POS_START,
                                  (uint32_t)RV8523_RTC::getClockTime(),
                                  (uint32_t)analogRead(A0),
                                  (uint32_t)analogRead(A1) );
    messages.moistureTempData.debug_Header_AND_Message();

    transmitOrSave(nextHop, MoisutreTempData::DATA_SIZE, ifConcatinate);         
}
void MoleNet_Node::send_typ_DATA_PLUS_INFO(const uint8_t &nextHop, const uint8_t &sinkID, const bool &ifConcatinate)
{
    if( !Decagon5TM::getChecksum() )           // if checksum is correct, only then proceed
        return;
    
    PRINT_PACKET_SENT( RFM69::_address, nextHop)  
    messages.dataPlusInfo.fillHeader( sinkID );
    messages.dataPlusInfo.fillData( 
                                  (uint32_t)5 , 
                                  (uint32_t)DATA_PLUS_INFO::DATA_POS_START,
                                  (uint32_t)RV8523_RTC::getClockTime(),
                                  (uint32_t)Decagon5TM::getDielectric(),
                                  (uint32_t)Decagon5TM::getTemperature(),
                                  (uint32_t)ExternalEEPROM::nextAddress );
    messages.dataPlusInfo.debug_Header_AND_Message();
          
    transmitOrSave(nextHop, DATA_PLUS_INFO::DATA_SIZE, ifConcatinate);          
}




void MoleNet_Node::send_typ_RX_TX_INFORMATION(const uint8_t &nextHop, const uint8_t &sinkID, const bool &ifConcatinate)
{
    PRINT_PACKET_SENT( RFM69::_address, nextHop)  
    messages.rxTxInfomration.fillHeader( sinkID );    
    messages.rxTxInfomration.fillData(
                                (uint32_t)3 , 
                                (uint32_t)RX_TX_INFORMATION::DATA_POS_START,
                                (uint32_t)RV8523_RTC::getClockTime(),
                                (uint32_t)ExternalEEPROM::nextAddress );
    messages.rxTxInfomration.debug_Header_AND_Message();
     
    transmitOrSave(nextHop, RX_TX_INFORMATION::DATA_SIZE, ifConcatinate);       
}

void MoleNet_Node::send_typ_STARTUP_CONFIG(const uint8_t &nextHop, const uint8_t &sinkID)
{
    PRINT_PACKET_SENT( RFM69::_address, nextHop)  
    messages.startupConfig.fillHeader( sinkID );    
    messages.startupConfig.fillData(
                                StartupConfig::DATA_POS_START,
                                config,
                                StartupConfig::SIZE );
    messages.startupConfig.debug_Header_AND_Message();
    
    if(!RFM69mh::transmitData(sinkID, StartupConfig::DATA_SIZE, false))
        PRINT_UNREACHABLE    
}

void MoleNet_Node::send_typ_REQ_PACKET(const uint8_t &nextHop, Packet::Type packetType)
{
    PRINT_PACKET_SENT( RFM69::_address, nextHop)  
    messages.requestPacket.fillHeader( nextHop );
    messages.requestPacket.fillData(
                                (uint32_t)2, 
                                (uint32_t)RequestPacket::DATA_POS_START,
                                (uint32_t)packetType );
    messages.requestPacket.debug_Header_AND_Message();
    
    if(!RFM69mh::transmitData( nextHop, RequestPacket::DATA_SIZE, false) )
        PRINT_UNREACHABLE     
}

void MoleNet_Node::send_CompleteEEPROM(const uint8_t &nextHop, const uint8_t &sinkID)
{
    if( ExternalEEPROM::nextAddress == 0 ) {
        PRINT_EEPROM_EMPTY
        return;
    }
    
    while( ExternalEEPROM::nextAddress > 0 ) {
        PRINT_PACKET_SENT( RFM69::_address, nextHop)    
        messages.decagon5tm.fillHeader( sinkID );  
        
        const uint8_t SIZE = this->concatenate(IMessage::SIZE);
        if(!RFM69mh::transmitData(nextHop, SIZE, false ) ) {
            PRINT_UNREACHABLE 
            break; // break loop if transmission failed
        }
        else {
            ExternalEEPROM::nextAddress -= (SIZE-IMessage::SIZE);
        }
        delay(1000); // Small delay for receiver to process data
    }
    
}

bool MoleNet_Node::transmitOrSave(const uint8_t &nextHop, const uint8_t &pktSize, const bool &ifConcatinate) 
{
    uint8_t size = 0;
    if( ifConcatinate )
        size = this->concatenate(pktSize);
    else
        size = pktSize;
    
    if(!RFM69mh::transmitData(nextHop, size, false) ) {
        PRINT_UNREACHABLE   
        this->ExternalEEPROM::savePacket(IMessage::DATA_POS_START, pktSize);
    }
    else {
        ExternalEEPROM::nextAddress -= (size-pktSize);
    }
    this->saveStartupConfig();        
}

void MoleNet_Node::processReceivedData()  // pure virutal function
{ 
    RFM69mh::RSSI = RFM69mh::RFM69::RSSI;   // save RSSI
    uint8_t SENDERID = RFM69mh::RFM69::SENDERID; // save sender id incase we need to reply
    RFM69mh::extract();                     // save packet locally  
    PRINT_PACKET_RECEIVED(RFM69mh::RFM69::SENDERID, config[StartupConfig::pos_NODEID])  // senderID, receiverID    
    
    if (RFM69::ACKRequested())
    {   
        RFM69mh::RFM69::sendACK();              // and imediatly send Acknowledgement
        
        ++RFM69mh::rxPacketCount;
        RFM69mh::debugRxInfo();

        if( RFM69mh::payLoad[IMessage::pos_SINK_ID] == this->config[StartupConfig::pos_NODEID] )
        {
            uint8_t position = IMessage::DATA_POS_START;
            // Handle Request Packet first
            if( RFM69mh::payLoad[position+RequestPacket::pos_PACKET_TYPE] == Packet::typ_REQ_DECAGON5TM_SENSOR )
            {
                messages.requestPacket.debug_Header_AND_Message();                
                uint32_t currentTime = millis();
                this->exciteSensor();
                while( (millis()-currentTime) < 2000) {  // 2 seconds timeout
                    if( this->newDecagon5tmDataAvailable() ) {
                        this->send_typ_DECAGON5TM_SENSOR(SENDERID,SENDERID);
                        return;
                    }
                }
                PRINT_ERROR_SENSOR_NOT_FOUND
            }
            else if( RFM69mh::payLoad[position+RequestPacket::pos_PACKET_TYPE] == Packet::typ_REQ_MOISTURE_TEMP_SENSOR )
            {
                messages.requestPacket.debug_Header_AND_Message();                   
                this->send_typ_MOISTURE_TEMP_SENSOR(SENDERID,SENDERID);
            }     
            else if( RFM69mh::payLoad[position+RequestPacket::pos_PACKET_TYPE] == Packet::typ_REQ_DATA_PLUS_INFO )
            {
                messages.requestPacket.debug_Header_AND_Message();   
                this->send_typ_DATA_PLUS_INFO(SENDERID,SENDERID);
            }                
            else if( RFM69mh::payLoad[position+RequestPacket::pos_PACKET_TYPE] == Packet::typ_REQ_RX_TX_INFORMATION )
            {
                messages.requestPacket.debug_Header_AND_Message();   
                this->send_typ_RX_TX_INFORMATION(SENDERID,SENDERID);
            }               
            else if( RFM69mh::payLoad[position+RequestPacket::pos_PACKET_TYPE] == Packet::typ_REQ_STARTUP_CONFIG )
            {
                messages.requestPacket.debug_Header_AND_Message();   
                this->send_typ_STARTUP_CONFIG(SENDERID,SENDERID);
            }     
            else if( RFM69mh::payLoad[position+RequestPacket::pos_PACKET_TYPE] == Packet::typ_REQ_COMPLETE_EEPROM_DATA)
            {
                this->send_CompleteEEPROM( SENDERID, SENDERID );
            }
            else {
                // Process Concatinated Data Packets
                while( position < RFM69mh::payLoadLength ) {
                    uint8_t currentPktlength;
                    
                    if( RFM69mh::payLoad[position+Decagon5TMData::pos_PACKET_TYPE] == Packet::typ_DECAGON5TM_SENSOR ) {
                        currentPktlength = RFM69mh::payLoad[position+Decagon5TMData::pos_PACKET_LENGTH]; // take currentPktlength
                        messages.decagon5tm.debug_Message(position);
                    }
                    else if( RFM69mh::payLoad[position+MoisutreTempData::pos_PACKET_TYPE] == Packet::typ_MOISTURE_TEMP_SENSOR ) {
                        currentPktlength = RFM69mh::payLoad[position+MoisutreTempData::pos_PACKET_LENGTH]; // take currentPktlength                    
                        messages.moistureTempData.debug_Message(position);
                    }
                    else if( RFM69mh::payLoad[position+DATA_PLUS_INFO::pos_PACKET_TYPE] == Packet::typ_DATA_PLUS_INFO ) {
                        currentPktlength = RFM69mh::payLoad[position+DATA_PLUS_INFO::pos_PACKET_LENGTH]; // take currentPktlength                     
                        messages.dataPlusInfo.debug_Message(position);
                    }                
                    else if( RFM69mh::payLoad[position+RX_TX_INFORMATION::pos_PACKET_TYPE] == Packet::typ_RX_TX_INFORMATION ) {
                        currentPktlength = RFM69mh::payLoad[position+RX_TX_INFORMATION::pos_PACKET_LENGTH]; // take currentPktlength                     
                        messages.rxTxInfomration.debug_Message(position);
                    }   
                    else if( RFM69mh::payLoad[position+StartupConfig::pos_PACKET_TYPE] == Packet::typ_STARTUP_CONFIG ) {
                        currentPktlength = RFM69mh::payLoad[position+StartupConfig::pos_PACKET_LENGTH]; // take currentPktlength                     
                        messages.startupConfig.debug_Message(position);
                        overwriteConfig(&RFM69mh::payLoad[position]);
                        applyStartupConfig();
                        position+=currentPktlength;
                        continue;  // No need to save this packet
                    } 
                    else {
                        Serialprint("[Invalid packet Type received]"); return;
                    }
                        this->ExternalEEPROM::savePacket(position, position+currentPktlength); // save Packet 
                        this->saveStartupConfig();                
                        position+=currentPktlength;
                    
                }  
            }
        }
        else {    // Forward it to next Node
            if(!RFM69mh::transmitData( config[StartupConfig::pos_GATEWAYID], RFM69mh::payLoadLength) )
                PRINT_UNREACHABLE       //TODO must be save in eeprom if failed during forwading            
        }          
    }
    else
    {
        if( RFM69mh::payLoad[IMessage::pos_SINK_ID] == RF69_BROADCAST_ADDR )    // Process BroadCasted Packets
        {
            if( RFM69mh::payLoad[IMessage::DATA_POS_START+StartupConfig::pos_PACKET_TYPE] == Packet::typ_STARTUP_CONFIG )
            {          
                messages.startupConfig.debug_Message(IMessage::DATA_POS_START);
                overwriteConfig(&RFM69mh::payLoad[IMessage::DATA_POS_START]);
                applyStartupConfig();
            }
        }
    }        
}

void MoleNet_Node::cleanupEEPROM()
{ 
    ExternalEEPROM::cleanup();
    saveStartupConfig();
}

uint8_t MoleNet_Node::concatenate(uint8_t currentPayLoadLength)
{
    if( ExternalEEPROM::nextAddress == 0 ) {
        PRINT_EEPROM_EMPTY
        return currentPayLoadLength;
    }
    
    int32_t addr = ExternalEEPROM::nextAddress;
    uint8_t payloadPosition = currentPayLoadLength;
    while( (currentPayLoadLength + SPIEEP::read(addr-2)) <= RF69_MAX_DATA_LEN )  // less than maximum packet size 61
    {
        uint8_t size = SPIEEP::read(addr-2);
        for(int32_t i = addr-1 ; i >= (addr-size); --i)
        {
            RFM69mh::payLoad[currentPayLoadLength++] =  SPIEEP::read(i);
        }        
        addr -=size;
        Serial.println("");
        debug_Payload(payloadPosition);
        payloadPosition += size;
        if(addr == 0) break;        
    }
    return currentPayLoadLength;
}
void MoleNet_Node::read_CompleteEEPROM() {
    if( ExternalEEPROM::nextAddress == 0 ) {
        PRINT_EEPROM_EMPTY
        return;
    }
    
    uint8_t size;
    
    int32_t addr = ExternalEEPROM::nextAddress;
    while( addr > 0 ) 
    {
        // Find size of one packet in eeprom
        size = SPIEEP::read(addr-2);
        
        const uint8_t POSITION = 0;
        uint8_t index = POSITION;
        
        Serial.print("i: "); Serial.println(addr-1);
        for(int32_t i = addr-1; i >= (addr-size); --i, ++index ) {
            RFM69mh::payLoad[index] = SPIEEP::read(i);
        }
        
        // Debug
        debug_Payload(POSITION);
        addr -= size; 
    }
}

void MoleNet_Node::debug_Payload(const uint8_t &POSITION)
{
    if( RFM69mh::payLoad[POSITION+Decagon5TMData::pos_PACKET_TYPE] == Packet::typ_DECAGON5TM_SENSOR ) {
        messages.decagon5tm.debug_Message(POSITION);
    }
    else if( RFM69mh::payLoad[POSITION+MoisutreTempData::pos_PACKET_TYPE] == Packet::typ_MOISTURE_TEMP_SENSOR ) {
        messages.moistureTempData.debug_Message(POSITION);
    }
    else if( RFM69mh::payLoad[POSITION+DATA_PLUS_INFO::pos_PACKET_TYPE] == Packet::typ_DATA_PLUS_INFO ) {
        messages.dataPlusInfo.debug_Message(POSITION);
    }                
    else if( RFM69mh::payLoad[POSITION+RX_TX_INFORMATION::pos_PACKET_TYPE] == Packet::typ_RX_TX_INFORMATION ) {
        messages.rxTxInfomration.debug_Message(POSITION);
    }       
}

void MoleNet_Node::overwriteConfig(unsigned char *_config)
{
    // Control_1 Registers
    if(  bitRead( _config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_SET_NODEID ) )
    {
        config[StartupConfig::pos_NODEID] = _config[StartupConfig::pos_NODEID]; 
        RFM69mh::RFM69::setAddress( _config[StartupConfig::pos_NODEID] );
    }
    
    if(  bitRead( _config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_SET_GATEWAYID ) )
    {
        config[StartupConfig::pos_GATEWAYID] = _config[StartupConfig::pos_GATEWAYID]; 
    }
    
    if(  bitRead( _config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_SET_SINKID ) )
    {
        config[StartupConfig::pos_SINKID] = _config[StartupConfig::pos_SINKID]; 
    }
    
    if(  bitRead( _config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_SET_DELAY_BEFORE_SEND ) )
    {
        config[StartupConfig::pos_DELAY_BEFORE_SEND] = _config[StartupConfig::pos_DELAY_BEFORE_SEND]; 
    }
    
    if(  bitRead( _config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_SET_DELAY_BEFORE_SLEEP ) )
    {
        config[StartupConfig::pos_DELAY_BEFORE_SLEEP] = _config[StartupConfig::pos_DELAY_BEFORE_SLEEP]; 
    }
    
    if(  bitRead( _config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_SET_DELAY_AFTER_RECEIVE ) )
    {
        config[StartupConfig::pos_DELAY_AFTER_RECEIVE] = _config[StartupConfig::pos_DELAY_AFTER_RECEIVE]; 
    } 
    
    // Control_2 Registers
    if(  bitRead( _config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_ALARM ) )
    {
        config[StartupConfig::pos_ALARM_HOUR] = _config[StartupConfig::pos_ALARM_HOUR]; 
        config[StartupConfig::pos_ALARM_MINUTE] = _config[StartupConfig::pos_ALARM_MINUTE]; 
        config[StartupConfig::pos_ALARM_DAY] = _config[StartupConfig::pos_ALARM_DAY]; 
        config[StartupConfig::pos_ALARM_WEEKDAY] = _config[StartupConfig::pos_ALARM_WEEKDAY]; 
        
    }
    
    if(  bitRead( _config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_RTC ) )
    {
        config[StartupConfig::pos_EPOCH_0] = _config[StartupConfig::pos_EPOCH_0];
        config[StartupConfig::pos_EPOCH_1] = _config[StartupConfig::pos_EPOCH_1];
        config[StartupConfig::pos_EPOCH_2] = _config[StartupConfig::pos_EPOCH_2];
        config[StartupConfig::pos_EPOCH_3] = _config[StartupConfig::pos_EPOCH_3];
    }
    
    // Control_3 Registers

    
    // Copy Applyable configurations
    config[StartupConfig::pos_CONTROL_1] = _config[StartupConfig::pos_CONTROL_1]; // Everything is set above
    config[StartupConfig::pos_CONTROL_2] = _config[StartupConfig::pos_CONTROL_2];
    
    saveStartupConfig();
}

void MoleNet_Node::applyStartupConfig()
{
    // Control 1 Register Actions
    if( bitRead(config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_CLEAR_COUNT_VARIABLES ) )
    {
        RFM69mh::txPacketCount = 0;
        RFM69mh::rxPacketCount = 0;
        RFM69mh::RFM69::RETERIES = 0;
        RFM69mh::lostPacketCount = 0;
        RFM69mh::RFM69::RTT = 0;
        RFM69mh::RSSI = 0;
        bitClear(config[StartupConfig::pos_CONTROL_1], StartupConfig::pos_CLEAR_COUNT_VARIABLES );
    }
    
    // Control 2 Regesiter Actions
    if( bitRead(config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_RTC ) ) 
    {
        // Seting new time in RTC
        uint32_t time = (uint32_t)config[StartupConfig::pos_EPOCH_0] << 24 |
                        (uint32_t)config[StartupConfig::pos_EPOCH_1] << 16 |
                        (uint32_t)config[StartupConfig::pos_EPOCH_2] << 8  |
                        (uint32_t)config[StartupConfig::pos_EPOCH_3];
        TimeElements tm;
        breakTime(time,tm);
        setClockTime(tm.Second, tm.Minute, tm.Hour, tm.Day, tm.Month, tm.Year);
        Serial.println("\nSetting new Time");
        bitClear(config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_RTC );
        saveStartupConfig();
    }
    
    if( bitRead(config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_ALARM ) ) 
    {
        // Setting new Alarm in RTC
        beginAlarm(1, config[StartupConfig::pos_CONTROL_2], 
                      config[StartupConfig::pos_ALARM_MINUTE], 
                      config[StartupConfig::pos_ALARM_HOUR], 
                      config[StartupConfig::pos_ALARM_DAY], 
                      config[StartupConfig::pos_ALARM_WEEKDAY] );
        
        //bitClear(config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_ALARM );
        saveStartupConfig();
    }
    
    // Cleanup EEPROM
    if( bitRead(config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_CLEANUP_EEPROM ) ) 
    {
        this->cleanupEEPROM(); 
        bitClear(config[StartupConfig::pos_CONTROL_2], StartupConfig::pos_SET_CLEANUP_EEPROM );
        //saveStartupConfig(); already saved in cleanupEEPROM
    }
}

void MoleNet_Node::initializeIO()
{
    // Initialize I/O
    pinMode(SCL,            OUTPUT);
    pinMode(Pin::TXD,       OUTPUT);   
    pinMode(Pin::RXD,       INPUT);
    pinMode(Pin::INT,       INPUT_PULLUP);
}

void MoleNet_Node::initializeSerial()
{
    Serial.begin(MOLENET_BAUD_RATE);
    while (!Serial) {
     // wait for serial port to connect. Needed for native USB port only
    }    
}

void MoleNet_Node::debugNodeInfo()
{
    Serialprint("[NODE_INFO] ");
    Serialprint("[nodeID %d] ",    config[StartupConfig::pos_NODEID]);
    Serialprint("[nextHopID %d] ", config[StartupConfig::pos_GATEWAYID]);
    Serialprint("[sinkID %d] ",    config[StartupConfig::pos_SINKID]);
    Serialprint("[nextAddr %d] ",  ExternalEEPROM::nextAddress);
    
    RV8523_RTC::debugCurrentTime();
    RV8523_RTC::debugAlarm();
    Serialprint("\n");
}

void MoleNet_Node::sleepMode()
{
    PRINT_SLEEP_MODE_ACTIVATED
    delay(1000);
    clock_prescale_set(CLOCKPRESC);   // Reduce clock to save energy
    ADCSRA = 0;                       // Disable ADC to save energy
    RFM69mh::RFM69::sleep();    // Send radio to sleep    
    ExternalEEPROM::SPIEEP::sleepmode();       // Send EEPROM to deep sleep

    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF,BOD_OFF);   
}


void MoleNet_Node::serialCmdLoop()
{
   int cmd = SerialCommandHandler::readSerial<int>(); 
   Command::Type cmdType;
   if( cmd != -1 )
   {
       cmdType = static_cast<Command::Type>( cmd);
       Serialprint("\n[NODE_COMMAND %d] ",cmdType);

       if (cmdType == Command::DEBUG_NODE_INFO) {
           this->debugNodeInfo();
       } 
       else if (cmdType == Command::DEBUG_COMPLETE_EEPROM) { 
           this->read_CompleteEEPROM();
       }       
       else if(cmdType == Command::DEBUG_STARTUP_CONFIG) {
         for(uint8_t i = 0; i < StartupConfig::SIZE; ++i)
             this->payLoad[i] = this->config[i];
             messages.startupConfig.debug_Message(0);
       }   
       else if(cmdType == Command::DEBUG_RX_TX_INFORMATION) {
            messages.rxTxInfomration.fillData(
                                        (uint32_t)3 , 
                                        (uint32_t)RX_TX_INFORMATION::DATA_POS_START,
                                        (uint32_t)RV8523_RTC::getClockTime(),
                                        (uint32_t)ExternalEEPROM::nextAddress );
            messages.rxTxInfomration.debug_Message(IMessage::DATA_POS_START);

       }            
       
       
       
       else if (cmdType == Command::SEND_NEW_DECAGON5TM_DATA_DATA_TO_NODE) {
           uint32_t currentTime = millis();
           this->exciteSensor();
            while( (millis()-currentTime) < 2000) {  // 2 seconds timeout
                if( this->newDecagon5tmDataAvailable() ) {
                    uint8_t id = SerialCommandHandler::readSerial(0,255);
                    this->send_typ_DECAGON5TM_SENSOR(id,id);                     
                    return;
                }
            }
            PRINT_ERROR_SENSOR_NOT_FOUND
       }         
       else if( cmdType == Command::SEND_NEW_MOISTURE_TEMP_DATA_TO_NODE) {
           uint8_t id = SerialCommandHandler::readSerial(0,255);
           this->send_typ_MOISTURE_TEMP_SENSOR(id,id); 
       }
       else if( cmdType == Command::SEND_NEW_DATA_PLUS_INFO_TO_NODE) {
           uint8_t id = SerialCommandHandler::readSerial(0,255);
           this->send_typ_DATA_PLUS_INFO(id,id); 
       }       
       else if(cmdType == Command::SEND_RX_TX_INFORMATION) {
           uint8_t id = SerialCommandHandler::readSerial(0,255);
           this->send_typ_RX_TX_INFORMATION(id, id); 
       }       
       else if(cmdType == Command::SEND_STARTUP_CONFIG_TO_NODE) {
           uint8_t id = SerialCommandHandler::readSerial(0,255);
           this->send_typ_STARTUP_CONFIG(id, id); 
       }       
       
       
       else if(cmdType == Command::REQ_NEW_DECAGON5TM_DATA) {
           this->send_typ_REQ_PACKET(SerialCommandHandler::readSerial(0,255), Packet::typ_REQ_DECAGON5TM_SENSOR);
       }
       else if(cmdType == Command::REQ_NEW_MOISTURE_TEMP_DATA) {
           this->send_typ_REQ_PACKET(SerialCommandHandler::readSerial(0,255), Packet::typ_REQ_MOISTURE_TEMP_SENSOR);
       }       
       else if(cmdType == Command::REQ_NEW_DATA_PLUS_INFO) {
           this->send_typ_REQ_PACKET(SerialCommandHandler::readSerial(0,255), Packet::typ_REQ_DATA_PLUS_INFO);
       }       
       else if(cmdType == Command::REQ_RX_TX_INFORMATION) {
           this->send_typ_REQ_PACKET(SerialCommandHandler::readSerial(0,255), Packet::typ_REQ_RX_TX_INFORMATION);
       }
       else if(cmdType == Command::REQ_STATUP_CONFIG) {
           this->send_typ_REQ_PACKET(SerialCommandHandler::readSerial(0,255), Packet::typ_REQ_STARTUP_CONFIG);
       }
       
       
       else if( cmdType == Command::REQ_COMPLETE_EEPROM ) {
           this->send_typ_REQ_PACKET(SerialCommandHandler::readSerial(0,255), Packet::typ_REQ_COMPLETE_EEPROM_DATA);
       }  
       else if (cmdType == Command::SEND_COMPLETE_EEPROM) {
           uint8_t id = SerialCommandHandler::readSerial(0,255);
           this->send_CompleteEEPROM(id, id);    
       }
       
       
       else if(cmdType == Command::BROADCAST_SYNCHRONIZE_SET_TIME_ALARM_CLEAN_EEPROM) {

            messages.startupConfig.fillHeader( RF69_BROADCAST_ADDR );
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_PACKET_TYPE] =  static_cast<unsigned char>(Packet::typ_STARTUP_CONFIG);

            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_NODEID] = 0;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_GATEWAYID] = 0;

            
            
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_CONTROL_1] = B01000000;  // Clear all variables
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_CONTROL_2] = B11110000;  // Set time and incrmenetal alram and clean up eeprom
            
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_HOUR] = 0;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_MINUTE] = SerialCommandHandler::readSerial(0,59);
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_DAY] = 0;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_WEEKDAY] = 0;
            
            beginAlarm(1, this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_CONTROL_2], 
                      this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_MINUTE], 
                      this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_HOUR], 
                      this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_DAY], 
                      this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_ALARM_WEEKDAY] );
                 
            uint32_t time = RV8523_RTC::getClockTime()+1;   // plus one to correct time
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_EPOCH_0] = (time >> 24) & 0xFF;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_EPOCH_1] = (time >> 16) & 0xFF;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_EPOCH_2] = (time >> 8) & 0xFF;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_EPOCH_3] = time & 0xFF;
            
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_DELAY_BEFORE_SEND] = 0;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_DELAY_BEFORE_SLEEP] = 0;
            this->payLoad[IMessage::DATA_POS_START+StartupConfig::pos_DELAY_AFTER_RECEIVE] = 0;            
                        
            if(!this->transmitData(255, StartupConfig::DATA_SIZE) )
               PRINT_UNREACHABLE  
       }       
       else if(cmdType == Command::APPLY_NEW_TIME) {
           uint32_t time = SerialCommandHandler::readSerial<uint32_t>(0, 4294967295);  // maximum number uin32_t can sotre    
           RV8523_RTC::setClockTime(time);
       }         
       else if (cmdType == Command::CLEANUP_EEPROM) {
           this->cleanupEEPROM();    
       }       
   }
}
