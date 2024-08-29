#include <avr/io.h>
#include <avr/power.h>
#include <LowPower.h>      //from: https://github.com/LowPowerLab/LowPower
#include <RFM69.h>         //from: https://www.github.com/lowpowerlab/rfm69
#include <SDI12.h>
#include <avr/interrupt.h>
#include "definitions.h"

#include "Wire.h"
#include <PCF8523.h>


#include <SPI.h>
#include <SPIEEP.h>        //from: https://bitbucket.org/spirilis/spieep/

#define SENSOR_PWPIN    7         // Sensor Power
#define SENSOR_PIN      8         // Sensor data pin
#define RTC_IRQ_PIN     3         // The Pin ID
#define RFM_IRQ_PIN     2         // The Pin ID

#define NODEID          5         // The ID should not be 1 -> the gateway is our number one!

#define EEPROM_CSPIN    9         // CS for EEPROM
#define CLOCKPRESC      clock_div_1 // reduce energy consumption, baudrate

#define SENDMINUTE      0         // Send every full hour

#define UTCOFFSET       2         // Time offset while programming this sketch to set the RTC to UTC

#if NODEID == 1
#error The node id is one but I am quite sure that this is not a gateway
#endif

// TODO:
// Use new flash lib?
// Use internal EEPROM (EEPROM.h) for config data


SDI12 mySDI12(SENSOR_PIN);

PCF8523 rtc;

SPIEEP eep(16, 128, 65535);

NodeData_v1 nodeData;

volatile boolean alarmTriggered = false;

uint8_t seq = 0; // A sequence number for the packets

RFM69 radio;

void setup() {
  //clock_prescale_set(CLOCKPRESC); // reduce clock to save energy. Results into problems with

  Serial.begin(115200);
  Serial.print("Sketch complied on: ");
  Serial.print(F(__DATE__));
  Serial.print(" ");
  Serial.println(F(__TIME__));
  randomSeed(analogRead(0));

  pinMode(RTC_IRQ_PIN, INPUT_PULLUP);

  Wire.begin();
  rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC not running. Setting time to sketch compile time");
    rtc.setTime(DateTime(F(__DATE__), F(__TIME__)) - TimeSpan(UTCOFFSET * 60 * 60));
  } else {
    Serial.println("The RTC is running. Doing nothing");
  }

  rtc.setBatterySwitchover();
  rtc.setTwelveTwentyFourHour(eTWENTYFOURHOUR);
  rtc.setAlarm(SENDMINUTE);
  rtc.enableAlarm(true);

  Serial.print("RTC power low: "); Serial.println(rtc.rtcBatteryLow());


  init_radio();
  radio.sleep();

  eep.begin_spi(EEPROM_CSPIN);
  eep.sleepmode();

  pinMode(RTC_IRQ_PIN, INPUT);

  // Force reading the RTC state
  irqHandler();

  seq = random(0, 250); // Init sequence number with random value.

  Serial.println("### Setup done");
}

void loop() {
  detachInterrupt(digitalPinToInterrupt(RTC_IRQ_PIN));

  if (alarmTriggered) {
    Serial.println("Sending data");
    initData(&nodeData);
    getSensorData(&nodeData);
    getTime(&nodeData);

    sendData(&nodeData);
    printData_v1(&nodeData);

    //    alarm.resetInterrupt();
    alarmTriggered = false;

  }

  rtc.ackAlarm();

  //create an interupt for the rtc alarm
  attachInterrupt(digitalPinToInterrupt(RTC_IRQ_PIN), irqHandler, FALLING);

  // Go for deep sleep
  Serial.println("Going to sleep mode");
  Serial.flush();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);

}



/**
   Init the data structure with some basic information
*/
void initData(NodeData_v1 *nd) {
  seq++;
  nd->version = 1;
  nd->sent = false;
  nd->rssi = 0;
  nd->sender_id = NODEID;
  nd->sequence = seq;
}

/**
   Get the current time and store it to the data structure
*/
void getTime(NodeData_v1 *nd) {
  DateTime now = rtc.readTime();
  nd->sec = now.second();
  nd->minute = now.minute();
  nd->hour = now.hour();
  nd->day = now.day();
  nd->month = now.month();
  nd->year = now.year();
}


/**
   Send the data structure to the gateway
*/
boolean sendData(NodeData_v1 *nd) {
  init_radio();
  Serial.print("Size of packet "); Serial.println(sizeof(*nd));
  //nd->sent = radio.sendWithRetry(GATEWAYID, (byte*)nd, sizeof(*nd), (uint8_t)3, RFM69_ACK_TIMEOUT * 6);
  nd->sent = moleSendWithRetry(GATEWAYID, (byte*)nd, sizeof(*nd), (uint8_t)3, RFM69_ACK_TIMEOUT * 6); // MoleNet function, handle data from ACK packet -> timestamp
  nd->rssi = radio.RSSI;
  // Maybe add delay before going to slee? Was done so in previous version.
  radio.sleep();
  // Ensure uc is not woken up by radio in deep sleep
  // Will be enabled by init_radio again

}

/**
   Read out sensor data and store it to the data structure
*/
void getSensorData(NodeData_v1 *nd) {
  mySDI12.begin();
  pinMode(SENSOR_PWPIN, OUTPUT);
  digitalWrite(SENSOR_PWPIN, HIGH);
  delay(300);


  mySDI12.sendCommand("?I!");
  delay(300);

  String sensorId;

  while (mySDI12.available()) {
    sensorId = mySDI12.readStringUntil('\n');
  }

  String newCommand(sensorId[0]);
  newCommand += "M!";
  mySDI12.sendCommand(newCommand); // 0MC! -> Data from sensor 0 including checksum, exclude C for no checksum
  delay(300);

  String startMeasurements = mySDI12.readStringUntil('\n'); // 00013 -> sensor 0, 001 -> 1sec waiting for measurement, 3 data values
  while (mySDI12.available())mySDI12.read();
  delay(startMeasurements.substring(1, 4).toInt() * 1000); // SleepTime
  int numMeasurements = startMeasurements.substring(4).toInt();   // Number of measurements

  newCommand = sensorId[0];
  newCommand += "D0!";

  mySDI12.sendCommand(newCommand); // 0D0! -> Data from sensor 0 including checksum
  delay(300);

  String datastring = mySDI12.readStringUntil('\n'); // Data string
  while (mySDI12.available())mySDI12.read();

  Serial.print("Datastring from Sensor: "); Serial.println(datastring);

  // TODO: Change for sensors without conductivity, check with number of sensors variable
  nd->vwc = datastring.substring(getNIndexOf(datastring, '+', 0) + 1, getNIndexOf(datastring, '+', 1)).toFloat();
  nd->temp = datastring.substring(getNIndexOf(datastring, '+', 1) + 1, getNIndexOf(datastring, '+', 2)).toFloat();
  nd->cond = datastring.substring(getNIndexOf(datastring, '+', 2) + 1).toFloat();

  mySDI12.end();
  digitalWrite(SENSOR_PWPIN, LOW);
}

/**
   Initialize the radio and prepare for sending
*/
void init_radio()
{
  //initialize the radio with the desired frequency, nodeID and networkID
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.setPowerLevel(RADIO_POWERLEVEL);
  //enable encryption for the communication
  radio.encrypt(ENCRYPTKEY);
  //radio.setFrequency(434000000); //set frequency to some custom frequency
  //didn't work in test
  delay(1); // TODO: Check
}

/**
   MoleNet version of the sendWithRetry function. Can handle ACK timeout with more than 256 ms (should not be required after update of RFM69 lib) and timestamps sent via the ACK
*/
bool moleSendWithRetry(uint16_t toAddress, const void *buffer, uint8_t bufferSize, uint8_t retries, unsigned long timeout) {
  unsigned long sentTime;
  for (uint8_t i = 0; i <= retries; i++)
  {
    radio.send(toAddress, buffer, bufferSize, true);
    sentTime = millis();
    while (millis() - sentTime < timeout)
    {
      if (radio.ACKReceived(toAddress)) {
        Serial.print("Ack rx after ms: "); Serial.println(millis() - sentTime);
        // We got a timestamp from the remote side. Use it to sync our RTC
        if (radio.DATALEN == sizeof(unsigned long)) {
          unsigned long current_time;
          memcpy(&current_time, radio.DATA, radio.DATALEN);
          Serial.print("Timestamp received: "); Serial.println(current_time);
          rtc.setTime(DateTime(current_time));
        }
        return true;
      }
    }
  }
  return false;
}


/**
   Utility function: Find the Nth position of a given character
*/
int getNIndexOf(String s, char stopchar, int index) {
  int checkpos = -1;
  for (int i = 0; i <= index; i++) {
    checkpos = s.indexOf(stopchar, checkpos + 1);
    if (checkpos == -1) {
      return -1;
    }
  }
  return checkpos;
}

/**
   The IRQ handler for the wakeup interrupt by the RTC.
*/
void irqHandler()
{
  if (digitalRead(RTC_IRQ_PIN) == LOW)
    alarmTriggered = true;
}
