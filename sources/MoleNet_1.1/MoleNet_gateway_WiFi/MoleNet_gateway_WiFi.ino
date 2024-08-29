

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

#include <NTPClient.h>

#define NODEID        1    // unique for each node on same network

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiUdp.h>

#include <ArduinoJson.h>

StaticJsonDocument<200> doc;

#define SERIAL_BAUD   115200

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the
//same network


volatile bool data_to_send = false;
volatile bool show_wifi_info = true;
volatile bool ack_with_time = false;

uint8_t rx_data_len;
uint8_t rx_data[RF69_MAX_DATA_LEN + 1];
int16_t rx_rssi;

ESP8266WiFiMulti WiFiMulti;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#define HTTP_RETRIES 3
#define LED_PIN_GPIO  2 // for WEMOS board


void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);

#ifdef LED_PIN_GPIO
  pinMode(LED_PIN_GPIO, OUTPUT);
#endif



  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFiMulti.addAP("ComNets", "From437Past");



  radio.initialize(FREQUENCY, NODEID, NETWORKID);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.setPowerLevel(RADIO_POWERLEVEL);
  radio.encrypt(ENCRYPTKEY);
  radio.spyMode(promiscuousMode);
  Serial.print("Connecting");
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");
  timeClient.begin();
}


void loop() {
  if ((WiFiMulti.run() == WL_CONNECTED)) {
#ifdef LED_PIN_GPIO
    digitalWrite(LED_PIN_GPIO, LOW);
#endif
    if (show_wifi_info) {
      Serial.println("WiFi started and connected");
      show_wifi_info = false;
    }

    timeClient.update();
    // If the timestamp is larger than the 1st if Jan 2021, we most properly have a valid timestamp
    ack_with_time = timeClient.getEpochTime() > 1609459200ul;

    if (radio.receiveDone())
    {
      noInterrupts();
      Serial.println("## RX");
      if (memcmp(rx_data, radio.DATA, radio.DATALEN) == 0) {
        Serial.println("received copy, ignore");
        data_to_send = false;
      } else {
        data_to_send = true;
      }
      memcpy(rx_data, radio.DATA, radio.DATALEN);
      rx_data_len = radio.DATALEN;
      rx_rssi = radio.RSSI;
      interrupts();
      if (radio.ACKRequested())
      {
        // Always send an ACK -- even if we received a DUP.
        if (ack_with_time) {
          unsigned long currentTime = timeClient.getEpochTime();
          radio.sendACK((byte *) &currentTime, sizeof(currentTime));
          Serial.print(" - ACK with timestamp sent: "); Serial.println(currentTime);
        } else {
          radio.sendACK();
          Serial.println(" - ACK sent.");
        }
      }
    }

    if (data_to_send) {

      if (rx_data_len == sizeof(NodeData_v1)) {
        Serial.println("correct len");
        NodeData_v1 nodeData;
        memcpy(&nodeData, rx_data, sizeof(NodeData_v1));
        nodeData.rssi = rx_rssi;
        nodeData.sent = true; // we received, so it was sent.
        printData_v1(&nodeData);
        char encoded[BASE64::encodeLength(sizeof(NodeData_v1))];
        BASE64::encode((const uint8_t*)&nodeData, sizeof(NodeData_v1), encoded);

        Serial.println("BASE64 encoded data (Version 1):");
        Serial.println(encoded);

        doc.clear();
        doc["base64_data"] = String(encoded);

        // Sending the data to the server via POST request
#ifdef LED_PIN_GPIO
        digitalWrite(LED_PIN_GPIO, HIGH);
#endif
        if (!post_data(doc.as<String>())) {
          Serial.println("TX failed, reconnecting to WiFi");
          WiFi.reconnect();

          // TODO add timeout
          while (!WiFi.isConnected()) {
            delay(10);
          };
          if (post_data(doc.as<String>())) {
            Serial.println("TX successful in 2nd try");
          } else {
            Serial.println("TX failed in 2nd try");
          }
        } else {
          Serial.println("TX done");
        }
#ifdef LED_PIN_GPIO
        digitalWrite(LED_PIN_GPIO, LOW);
#endif
        // Sending done
        data_to_send = false;


      } else {
        Serial.print("Wrong len: "); Serial.print(sizeof(NodeData_v1)); Serial.print("   "); Serial.print(radio.DATALEN);
      }

    }

  } else {
    // No WiFi
#ifdef LED_PIN_GPIO
    digitalWrite(LED_PIN_GPIO, HIGH);
#endif
  }
}

bool post_data(String post_data_string) {
  Serial.println("Start post request");

  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure();
  http.setReuse(false);

  http.begin(client, "https://data.jdede.de/storage/set/");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-HEADER-TOKEN", "8c642d91-aa7b-4800-a272-1769d24f4767");
  http.addHeader("X-HEADER-DEVICE", "2F0E85EF06");
  int statusCode = http.POST(post_data_string);
  Serial.print("Status code: ");
  Serial.println(statusCode);

  const String& payload = http.getString();
  Serial.print("received payload:\n");
  Serial.println(payload);
  http.end();
  return statusCode > 0;

}
