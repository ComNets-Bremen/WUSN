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
  Definitions for MoleNet program
*/

#define NETWORKID       100             //the same on all nodes in the network
#define GATEWAYID       1               //ID of Gateway
#define FREQUENCY       RF69_433MHZ     //Frequency to be used for communication
#define ENCRYPTKEY      "myCodeIsVeryGood"  //key for encryption exactly the same 16 characters / bytes on all nodes!

#define RADIO_POWERLEVEL  31

//#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!


// Version 1 of the data structure
typedef struct {
  uint8_t version;
  uint16_t sender_id;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t sec;
  int32_t rssi;
  float vwc;
  float temp;
  float cond;
  bool sent;
} __attribute__((packed)) NodeData_v1; // packed -> no padding. Little endian!

void printData_v1(NodeData_v1 *nd) {
  Serial.print("Version: "); Serial.println(nd->version);
  Serial.print("Sender_id: "); Serial.println(nd->sender_id);
  Serial.print("Datetime: "); Serial.print(nd->year); Serial.print("-"); Serial.print(nd->month); Serial.print("-"); Serial.print(nd->day); Serial.print(" ");
  Serial.print(nd->hour); Serial.print(":"); Serial.print(nd->minute); Serial.print(":"); Serial.println(nd->sec);
  Serial.print("RSSI: "); Serial.println(nd->rssi);
  Serial.print("VWC: "); Serial.println(nd->vwc);
  Serial.print("Temp: "); Serial.println(nd->temp);
  Serial.print("Cond: "); Serial.println(nd->cond);
  Serial.print("Sent: "); Serial.println(nd->sent);
}
