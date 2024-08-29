

/******************************************************************************
   MoleNet - A Wireless Underground Sensor Network

   Copyright (C) 2016, Communication Networks, University of Bremen, Germany

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 3 of the License.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, see <http://www.gnu.org/licenses/>

   This file is part of MoleNet

 ******************************************************************************/

/*
   This code is running using the RFM69 lib by LowPowerLab. There is an ESP8266
   related issue in the lab:
   https://github.com/LowPowerLab/RFM69/issues/105
   https://github.com/LowPowerLab/RFM69/pull/150

   Instead of deriving a subclass, just change the sendACK() method in the lib for
   example to

    while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS){
      receiveDone();
    #ifdef RF69_PLATFORM_ESP8266
      delay(1); // Required to let the ESP8266 handle some background tasks
    #endif
    }

    Created PR 168, hope that helps: https://github.com/LowPowerLab/RFM69/pull/168

*/


//Simple Gateway for MoleNet

#include <RFM69.h>
#include "base64.hpp"
#include "definitions.h"
#define NODEID        1    //unique for each node on same network

#define SERIAL_BAUD   115200

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the
//same network
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.encrypt(ENCRYPTKEY);
  radio.spyMode(promiscuousMode);
}


void loop() {
  if (radio.receiveDone())
  {
    Serial.println("## RX");
    if (radio.DATALEN == sizeof(NodeData_v1)) {
      Serial.println("correct len");
      NodeData_v1 nodeData;
      memcpy(&nodeData, radio.DATA, sizeof(NodeData_v1));
      nodeData.rssi = radio.RSSI;
      nodeData.sent = true; // we received, so it was sent.
      printData_v1(&nodeData);
      char encoded[BASE64::encodeLength(sizeof(NodeData_v1))];
      BASE64::encode((const uint8_t*)&nodeData, sizeof(NodeData_v1), encoded);
      Serial.println("BASE64 encoded data (Version 1):");
      Serial.println(encoded);


    } else {
      Serial.print("Wrong len: "); Serial.print(sizeof(NodeData_v1)); Serial.print("   "); Serial.print(radio.DATALEN);
    }

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println(" - ACK sent.");
    }
// TODO: If we have an RTC on the gateway: Reply with epoch time to sync the time for all nodes
  }
}
