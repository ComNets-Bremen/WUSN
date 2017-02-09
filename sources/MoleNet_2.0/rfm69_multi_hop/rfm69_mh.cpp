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
 *       Filename:  rfm69_mh.cpp
 *
 *    Description:  Extenstion of RFM69 Class to provide "Message" sending 
 *                  capability and to debug RX, TX information.        
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

#include "rfm69_mh.h"

#define RETRY_WAIT_TIME 1500
#define NUMBER_OF_RETERIES 0

RFM69mh::RFM69mh()  :
      promiscuousMode(false),
      rxPacketCount(0),
      txPacketCount(0),
      lostPacketCount(0),
      payLoad( (unsigned char*) malloc (61) )
{     
}

RFM69mh::~RFM69mh()
{
    free(payLoad);
}

void RFM69mh::extract()
{
    payLoadLength = RFM69::DATALEN;
    for(int i = 0; i < payLoadLength; ++i) {
        payLoad[i] = RFM69::DATA[i];
    }
}

void RFM69mh::initialize(const uint8_t & NODEID)  // Virual Function
{
    RFM69::promiscuous(promiscuousMode);
    RFM69::initialize(RFM69mh::FREQUENCY, NODEID, RFM69mh::NETWORKID);
    RFM69::setPowerLevel(RFM69mh::RADIOPOWERLEVEL);
    RFM69::encrypt(ENCRYPTKEY);    
    
//     // Lowering bit rate from https://lowpowerlab.com/forum/moteino/multistorey-indoor-range-performance-1-75km-range-(solution)/msg3189/#msg3189
//     RFM69::writeReg(0x03,0x68);      //RegBitrateMsb 1200 bitrate
//     RFM69::writeReg(0x04,0x2B);      //RegBitrateLsb 1200 bitrate
//     RFM69::writeReg(0x05,0x00);      //RegFdevMsb     2000 
//     RFM69::writeReg(0x06,0x52);      //RegFdevLsb     2000
//     RFM69::writeReg(0x19,0x40|0x10|0x05);      //RegRxBw  DccFreq:010, RxBw_Mant:24, RxBw_Exp:5 
//     RFM69::writeReg(0x18,0x00|0x00|0x01);      //RegLna  LnaZin:50ohm, LowPower:Off, CurrentGain:MAX 
}

bool RFM69mh::transmitData(const uint8_t &destID, const uint8_t & SIZE, const bool &PRINT_INFO)
{
    payLoadLength = SIZE;
    ++txPacketCount;
    if(PRINT_INFO) {
        PRINT_PACKET_SENT( RFM69::_address, destID) // senderID, receiverID
    }
    
    if(destID == RF69_BROADCAST_ADDR) {  // Broadcast packet without requesting ACK
        RFM69::send(destID, payLoad, SIZE, false); //No ack requested
        debugTxInfo();
        return true;
    }
    else 
    {
         // Send
        if (RFM69::sendWithRetry(destID, payLoad,SIZE,NUMBER_OF_RETERIES, RETRY_WAIT_TIME)) {   // true mean ACK received
            Serialprint("[ACK %d] ",RFM69::SENDERID);
            debugTxInfo();
            return true;      
        }
        else {
            lostPacketCount++;
        // Serialprint("[NACK %d] ",IMessage::pos_SINK_ID);
            Serialprint("[NACK]");
            debugTxInfo();
            return false;
        }  
    }
}

void RFM69mh::debugRxInfo()
{
    if(Serial) {
        Serialprint("[pktRX %d] [rxRSSI %d] ", rxPacketCount, this->RSSI);
    }
}

void RFM69mh::debugTxInfo()
{
    if(Serial) {
        Serialprint("[pktTX %d] [pktLOST %d] ", txPacketCount, lostPacketCount );  
    }
}
