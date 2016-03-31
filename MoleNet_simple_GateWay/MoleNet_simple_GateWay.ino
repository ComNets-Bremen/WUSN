//Simple Gateway for MoleNet

#include <RFM69.h>  //get it here: https://www.github.com/lowpowerlab/rfm69
#include <avr/power.h>
#include "./definitions.h"
#define NODEID        1    //unique for each node on same network

#define SERIAL_BAUD   115200

RFM69 radio;
bool promiscuousMode = false; //set to 'true' to sniff all packets on the 
                              //same network
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);
  char buff[50];
  sprintf(buff, "\nListening at 433 Mhz...");
  Serial.println(buff);
}

byte ackCount=0;
uint32_t packetCount = 0;

void loop() {
  if (radio.receiveDone())
  {
    Serial.print("#[");
    Serial.print(++packetCount);
    Serial.print(']');
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    if (promiscuousMode)
    {
      Serial.print("to [");Serial.print(radio.TARGETID,DEC);Serial.print("] ");
    }
    for (byte i = 0; i < radio.DATALEN; i++)
    {
      if (i != YYP && i != MM && i != DD && i != HPOS && i != MPOS && i != CHK)
        Serial.print((char)radio.DATA[i]);
      else
      	Serial.print((uint8_t)radio.DATA[i]);
    }
    Serial.print(" [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    
    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");
    }
    Serial.println();
  }
}
