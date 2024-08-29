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
 *       Filename:  rfm69_mh.hpp
 *
 *    Description:  Extenstion of RFM69 Class to provide "Message" sending 
 *                  capability and to debug RX, TX information.
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

#ifndef MOLENET_RFM69_MULTIHOP_HPP_INCLUDED
#define MOLENET_RFM69_MULTIHOP_HPP_INCLUDED

#define DEBUG  // should not define, if debug is required

#if defined (__AVR__) || (__avr__)
    #include "serial_print.h"
    #include "RFM69.h"
    #include "Time.h"
#else
    #include "serial_print/serial_print.h"
    #include "RFM69/RFM69.h"
    #include "Time-master/Time.h"
#endif


#define ENCRYPTKEY "sampleEncryptKey"      //key for encryption

class RFM69mh : public RFM69 
{
public:
    RFM69mh();
    ~RFM69mh();
    void initialize(const uint8_t &NODEID);
    
    virtual void processReceivedData() = 0;
    bool transmitData(const uint8_t &destID, const uint8_t & SIZE, const bool &DEBUG = true);   
    
    unsigned char *payLoad;
    uint8_t payLoadLength;
    void extract();
    
    void debugRxInfo();
    void debugTxInfo();

    bool promiscuousMode;
    uint16_t rxPacketCount;
    uint16_t txPacketCount;
    uint16_t lostPacketCount;      
    int16_t RSSI;        
    
    enum Settings {  
        NETWORKID       = 100,          // Same on all nodes in the network      
        FREQUENCY       = 43, //RF69_433MHZ,  // Frequency to be used for communication
        RSETUPDELAY     = 1,            // Delay for radio setup
        RADIOPOWERLEVEL = 31,             // 31: send with full power 
        BUFFER_SIZE = 61
    };
};

#endif //MOLENET_RFM69_MULTIHOP_HPP_INCLUDED
