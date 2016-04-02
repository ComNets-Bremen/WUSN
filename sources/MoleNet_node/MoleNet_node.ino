/*
Program for the Prototype
read out sensor data and time/date information, store the data and transmit it
once a day at the time given by TTIME
transmission occurs directly after the read-out 
*/

#include <avr/io.h>
#include <avr/power.h>
#include <LowPower.h>			//from: https://github.com/LowPowerLab/LowPower
#include <RFM69.h>    		//from: https://www.github.com/lowpowerlab/rfm69
#include <RV8523.h>				//from: https://github.com/watterott/Arduino-Libs
#include <RV8523ALARM.h>	//own library
#include <SPIEEP.h>				//from: https://bitbucket.org/spirilis/spieep/
#include <avr/interrupt.h>
#include "./definitions.h"

/******************************************************************************/
//some definitions
/******************************************************************************/

#define NODEID        	2    			//unique for each node on same network

//#define TXDAILY
#ifdef TXDAILY
//time for the transmission of the databytes
#define TXTIME           23
#endif

#define CLOCKPRESC			clock_div_1 // reduce energy consumption, baudrate and
																		// delays need to be set accordingly!
#define EEPROM_CSPIN 		9					//EEPROM CS
#define RTC_CSPIN				8					//RTC Power
#define SENSOR_PWPIN		7					//Sensor Power
#define RXD							0					//Serial receive pin
#define TXD							1					//Seriel transmit pin (only for debugging)

#define SERIAL_BAUD 		1200			//must be 1200 baud for Decagon 5TM sensor!

#define RSETUPDELAY 		1					//delay for radio setup
#define RADIOPOWERLEVEL	31				//31: send with full power

#define INTPIN 					3					//pin for interrupt

#define STARTADDRESS    0x0010

/*******************************************************************************
Offsets for storing the data in the EEPROM
The storage string starts with an start indication character, followed by the 
date of the measurement. The values for the water content and the temperature
(three chars each) are both separated by indicators.
The string will be completed by a checksum for all previous values and the 
indicator for successful measurement and transmission 
*******************************************************************************/
#define BOFF            0         // Offset for the beginning of the string in 
                                  //EEPROM
#define YOFF						1					// Offset for the year of the measurement
#define MONOFF					2         // Offset for the month of the measurement
#define DOFF						3         // Offset for the day of the measurement
#define HOFF						4         // Offset for the hour of the measurement
#define MINOFF					5         // Offset for the minute of the measurement
#define W1OFF						6         // Offsets for the water content value
#define W2OFF						7
#define W3OFF						8
#define W4OFF           9
#define T1OFF						10        // Offsets for the temperature value
#define T2OFF						11
#define T3OFF						12
#define T4OFF           13
#define CHKOFF					14        // Offset for the checksum
#define SUCOFF					15        // Offset for the transmission success char

//set alarm to interrupt every full hour (i.e., interrupt everytime when
//xx:00:xx occurs)
#define MINUTE					00
#define HOUR						00
#define DAY							00
#define WEEKDAY         00
#define INI_ALARMTYPE		('m')

RV8523ALARM 	alarm;
RFM69 				radio;
RV8523 				rtc;
//SPIEEP 				eep(24,256,131072);	//for 1MBit model, library not suitable 
                                    //because variable type for full size
                                    //(here 131072) is int and thus too small
SPIEEP eep(16,128,65535);					//for 512kBit model, 
                                  //library author says: eep(16,128,65536)

/******************************************************************************/
//and now the variables
/******************************************************************************/
int 			crc;                    //variable for the crc-check

//arrays to store the values for the water content and the temperature
//char 		  wc[4] = {0x20,0x20,0x20,0x20}, temp[3] = {0x44,0x41,0x54};
//wt = {wc0,wc1,wc2,wc3,t0,t1,t2}
char wt[8] = {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

//indicator for the length of the water content value
uint8_t 	wclength = 0, templength = 0;

//suc : success indicator
//
//suc == '0' : crc-check and transmission failed
//suc == '1' : crc-check successful, but transmission failed
//suc == '2' : crc-check failed, transmission successful
//suc == '3' : crc-check and transmission successful
char 			suc = 0x30;

//arrays to store the sensor data
//d[] is for the data directly coming from the sensor
//in e[], the data for the crc-check are stored
char 			e[13] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                  0x20};
char 			d[15] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
									0x00,0x00,0x00};

//indicator for the completion of the sensor read-out
boolean 	stringcomplete = false, inttriggd = false;

//start address for writing on the eeprom
uint32_t	address = STARTADDRESS;
uint32_t  oldaddress = address;

//variable to store a checksum for the stored and transmitted values
char 	chksum = 0;

//variable to store the length of the string received from the sensor
uint8_t 	databytes;

//variable to store the time and date information of the measurement
uint8_t 	sec, minute, hour, day, weekday, month, yy;

//char determining the type of the alarm
//alarmtype can be chosen as:
//  - 'm' : alarm interrupt is triggered every hour at the specified minute
//  - 'h' : alarm interrupt is triggered every day at the specified hour
//  - 'd' : alarm interrupt is triggered every month on the specified day
char 			alarmtype;

byte 			sendSize = 30;//23; //22
char 			payload[] = {0x44,(char) yy, 0x2F, (char) month, 0x2F,(char) day,0x20,
                      0x74,(char) hour,0x3A,(char) minute,0x20,0x57,wt[0],wt[1],
                      wt[2],wt[3],0x20,0x54,wt[4],wt[5],wt[6],wt[7],0x20,
                      (char) chksum,0x20,suc,0x20,0x20,0x20};

/******************************************************************************/
//definition of interrupt routine
/******************************************************************************/
void intHandler()
{
	if (inttriggd == false)
	{
		inttriggd = true;
	}
}

/******************************************************************************/
//set up the microcontroller
/******************************************************************************/
void setup()
{
	uint16_t year;
	clock_prescale_set(CLOCKPRESC); // reduce clock to save energy

	//initialize I/O
	pinMode(INTPIN,INPUT_PULLUP);
	pinMode(RTC_CSPIN,OUTPUT);
	pinMode(SENSOR_PWPIN,OUTPUT);
	pinMode(SCL,OUTPUT);
	pinMode(RXD,INPUT);
	pinMode(TXD,OUTPUT);

  //initialize EEPROM
  eep.begin_spi(EEPROM_CSPIN);

	//send EEPROM to deep sleep
	eep.sleepmode();

  //enable serial port
	Serial.begin(SERIAL_BAUD);

  // disable ADC to save energy
  ADCSRA = 0;
	
	//send radio to sleep
	radio.sleep();
	
	//digitalWrite(RTC_CSPIN,HIGH); //not needed when jumper is set to VCC-supply

	alarm.resetCtrl();

	rtc.set24HourMode();
	rtc.start();
	rtc.batterySwitchOver(1);
	
	//set time of alarm
	alarm.setAlarmTime(MINUTE,HOUR,DAY,WEEKDAY);
	//set type of alarm
	alarm.setAlarmType(INI_ALARMTYPE);
	//make sure only the selected alarm type is activated
	switch (INI_ALARMTYPE)
	{
	  case 'm':
	    alarm.clearAlarmType('h');
	    alarm.clearAlarmType('d');
      alarm.clearAlarmType('w');
      break;
	  case 'h':
	    alarm.clearAlarmType('m');
	    alarm.clearAlarmType('d');
	    alarm.clearAlarmType('w');
	    break;
	  case 'd':
	    alarm.clearAlarmType('m');
	    alarm.clearAlarmType('h');
	    alarm.clearAlarmType('w');
	    break;
	  case 'w':
	    alarm.clearAlarmType('m');
	    alarm.clearAlarmType('h');
	    alarm.clearAlarmType('d');
	    break;
	  default:
	    break;
	}
  //activate the alarm
	alarm.activateAlarm();

	//get current time from RTC
	rtc.get(&sec, &minute, &hour, &day, &month, &year);
	alarm.getWeekday(&weekday);

	//print a welcome message
	Serial.print("\n*************\nhello!\n");

	Serial.print("Time read\n");
	delay(2000);
	//print current time and date
	Serial.print("Time: ");
	Serial.print(hour, DEC);
	Serial.print(":");
	Serial.print(minute, DEC);
	Serial.print(":");
	Serial.print(sec, DEC);
	Serial.print("\n");
  switch (weekday)
	{
	  case 0:
	    Serial.print("MON");
	    break;
	  case 1:
	    Serial.print("TUE");
	    break;
	  case 2:
	    Serial.print("WED");
	    break;
	  case 3:
	    Serial.print("THU");
	    break;
	  case 4:
	    Serial.print("FRI");
	    break;
	  case 5:
	    Serial.print("SAT");
	    break;
	  case 6:
	    Serial.print("SUN");
	    break;
	  default:
	    Serial.print("BAD");
	    break;
	}
	Serial.print(" ");
	Serial.print(year,DEC);
	Serial.print(" ");
	Serial.print(month,DEC);
	Serial.print(" ");
	Serial.print(day,DEC);
	Serial.print("\n");
	delay(1000);

  //reset alarm interrupt just in case
	alarm.resetInterrupt();
	
	//create an interupt for the rtc alarm
	attachInterrupt(1, intHandler, FALLING);
	
	getrtctime();
	delay(500);
	readsensor();
	delay(500);
	sender();
	
	//just being polite :-)
	Serial.print("good night!\n");
	delay(1000);
}

/******************************************************************************/
//the main loop
/******************************************************************************/
void loop()
{
	if (inttriggd == true)
	{	
		getrtctime();
		delay(500);
		readsensor();
		delay(500);
		#ifdef TXDAILY
		  writeToEEPROM();
		  if (hour == TXTIME)
		    sendData();
		#else
		  sender();
		  writeToEEPROM();
		  if (suc > '1')
        sendData();
    #endif

		databytes = 0;
		wclength = 0;
		templength = 0;
		//reset the success indicator and the trigger indicator
		suc = 0x30;
		alarm.resetInterrupt();
		delay(500);
		inttriggd = false;
		//send the microcontroller back to sleep
		LowPower.powerDown(SLEEP_FOREVER, ADC_OFF,BOD_OFF);
	}
	else //make sure the microcontroller is sent back to sleep
		LowPower.powerDown(SLEEP_FOREVER, ADC_OFF,BOD_OFF);
}

/******************************************************************************/
//get time from RTC module
/******************************************************************************/
void getrtctime()
{
	uint16_t year;
	//get time from RTC
  rtc.get(&sec, &minute, &hour, &day, &month, &year);
	#ifndef TXDAILY
	  //if transmission after every read-out is wanted, prepare the payload 
	  //accordingly
		payload[DD] = (char) day;
	  payload[MM] = (char) month;
	  //save space by just using the difference for the year
	  payload[YYP] = (char) (year - 2000);	
	  payload[HPOS] = (char) hour;
	  payload[MPOS] = (char) minute;
	#endif
}

/******************************************************************************/
//set the time and type of the alarm interrupt and activate it
/******************************************************************************/
void setalarm(uint8_t *minute, uint8_t *hour, uint8_t *day, uint8_t *weekday,
              char *alarmtype)
{
	alarm.setAlarmTime(*minute,*hour,*day, *weekday);
	alarm.setAlarmType(*alarmtype);
	alarm.activateAlarm();
	return;
}

/******************************************************************************/
//get data from sensor
/******************************************************************************/
void readsensor()//char *payload)
{
  uint8_t ckp = 0;	//position of checksum
  //cycle sensor to make a correct reading more propable
	digitalWrite(SENSOR_PWPIN,HIGH);
	delay(500);
	digitalWrite(SENSOR_PWPIN,LOW);
	delay(500);
	//switch sensor on and read the data
	digitalWrite(SENSOR_PWPIN,HIGH);
	serialEvent();
	if (stringcomplete == true)
	{
    //always use four digits for the water content value
    //if the sensor sends just two or three, add preceding zeros
		if (wclength == 3)
		{
		  wt[0] = 0x30;
			wt[1] = d[0];
			wt[2] = d[1];
			wt[3] = d[2];
		}
		else if (wclength == 2)
		{
			wt[0] = 0x30;
			wt[1] = 0x30;
			wt[2] = d[0];
			wt[3] = d[1];	
		}
		else
		{
		  strncpy(wt,d,4);
		}
    
    //the checksum is 8 or 9 places after the first space, depending on the 
    //temperature
		//if (templength == 3)
  	//	ckp = wclength + 8;
  	//else
  	//  ckp = wclength + 9;
    ckp = wclength + templength + 5;
    
    //copy all databytes before the checksum to verify the transmission
    //the data for the checksum provided by the sensor includes all chars
    //preceding the checksum
		strncpy(e,d,ckp);
    
    if (templength == 3)
    {
      wt[4] = '0x30';
      wt[5] = d[wclength+3];
		  wt[6] = d[wclength+4];
		  wt[7] = d[wclength+5];
    }
		else
		{
		  wt[4] = d[wclength+3];
      wt[5] = d[wclength+4];
		  wt[6] = d[wclength+5];
		  wt[7] = d[wclength+6];
		}
  
    //verify the checksum
		crc = checksum(e,ckp);
		chksum = checksum(wt,sizeof(wt));
		if (crc == d[ckp])
		{
			suc += 1;
		}
    #ifndef daily
		  payload[WPOS]   = wt[0];
		  payload[WPOS+1] = wt[1];
		  payload[WPOS+2] = wt[2];
		  payload[WPOS+3] = wt[3];
		  payload[TPOS]   = wt[4];
		  payload[TPOS+1] = wt[5];
		  payload[TPOS+2] = wt[6];
		  payload[TPOS+3] = wt[7];
		  payload[CHK]    = chksum;
		  payload[SPOS]   = suc;		  
		#endif

		stringcomplete = false; //reset the indicator for a complete receive
		
		//switch the sensor off again to save energy and allow a new reading
		digitalWrite(SENSOR_PWPIN,LOW);
		delay(500);
	}
	d[13] = 0x00;	//make shure no old data remains
	d[14] = 0x00;
	e[10] = 0x00;
	
}

/******************************************************************************/
//read out serial data (invoked by readsensor())
/******************************************************************************/
void serialEvent()
{
  //read the sensor data from the serial port
	uint8_t i = 0,first = 0;
	while (Serial.available())
	{	
		d[i] = (char)Serial.read();
		if (d[i] == 0x0D && first == 1)
		{
		  //save length of temperature string
		  templength = i - wclength - 3;
		  first = 2;
		}
		if (d[i] == ' ' && first == 0)
		{
		  //save the position of the first space
			wclength = i;
			first = 1;
		}

		if (d[i] == '\n')
		{
		  //line feed indicates the end of the transmission
			stringcomplete = true;
			databytes = i+1;
		}
		i++;
	}	
delay(1000);
}

/******************************************************************************/
//compute checksum of sensor data (invoked by readsensor())
//source: Decagon 5TM Integrator's Guide
/******************************************************************************/
char checksum(char * response, uint8_t length)
{
  //calculate the checksum of the received data
	int sum = 0, i;
	uint8_t crcsum;
	crcsum = 0;
	for (i = 0; i < length; i++)
		sum += response[i];
	crcsum = sum % 64 + 32;
	return crcsum;
}

/******************************************************************************/
//prepare to send the data and invoke the sending function
/******************************************************************************/
void sender()
{
  //set up the radio
  radiosetup();
  //wait a little bit so the radio can wake up
  delay(RSETUPDELAY);
  //send the data
  send();  
  delay(600); //mustn't be too low, definitely more than 500!
  //put radio back to sleep
  radio.sleep();
  //delay(500);
}

/******************************************************************************/
//send data if it is time for it
/******************************************************************************/
void sendData()
{
  //prepare and send the data collected since the last sending process
  //first wake up and initialize the radio
  radiosetup();
  delay(RSETUPDELAY);
  
  //set the address to the start value after the last transmission
  address = oldaddress;
  
  //read the data to be send and prepare the payload
  while (eep.read(address) == 'B' && eep.read(address+SUCOFF) == ('0' || '1'))
  {
    payload[DD]     = eep.read(address+DOFF);
  	payload[MM]     = eep.read(address+MONOFF);
  	payload[YYP]    = eep.read(address+YOFF);	
  	payload[HPOS]   = eep.read(address+HOFF);
  	payload[MPOS]   = eep.read(address+MINOFF);
   	payload[WPOS]   = eep.read(address+W1OFF);
	  payload[WPOS+1] = eep.read(address+W2OFF);
	  payload[WPOS+2] = eep.read(address+W3OFF);
	  payload[WPOS+3] = eep.read(address+W4OFF);
	  payload[TPOS]   = eep.read(address+T1OFF);
	  payload[TPOS+1] = eep.read(address+T2OFF);
	  payload[TPOS+2] = eep.read(address+T3OFF);
	  payload[TPOS+3] = eep.read(address+T3OFF);
	  payload[CHK]    = eep.read(address+CHKOFF);
	  payload[SPOS]   = eep.read(address+SUCOFF);
		//send the data
	  send();
	  delay(500);
	  //write success indicator to EEPROM to indicate whether the transmission was
	  //successful or not
	  if (suc == '2' || suc == '3')
	  {
	    write2eep(address+SUCOFF,suc);
		  oldaddress = address;
		}
		address += 0x10;
  }
  
  //put the radio back to sleep
  radio.sleep();
}

/******************************************************************************/
//set up the radio (invoked by sender())
/******************************************************************************/
void radiosetup()
{
  //initialize the radio with the desired frequency, nodeID and networkID
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  //set the transmission power to the maximum
  radio.setPowerLevel(RADIOPOWERLEVEL);
  //enable encryption for the communication
  radio.encrypt(ENCRYPTKEY);
  //radio.setFrequency(434000000); //set frequency to some custom frequency
		//didn't work in test
}

/******************************************************************************/
//actually send the data (invoked by sender())
/******************************************************************************/
void send()
{
  //send the payload, if successful add 2 to the indicator 
	if (radio.sendWithRetry(GATEWAYID,payload,sendSize,0))
	{
		suc += 2;
	}
	delay(500);
}



/******************************************************************************/
//write data to eeprom
/******************************************************************************/
void writeToEEPROM()
{
  eep.wakeup();
  //prevent previous data being overwritten after a power failure
  while (eep.read(address) == 'B')
  {
    address += 0x10;
  }
  //write the data to the EEPROM
  write2eep(address,'B');
  write2eep(address+YOFF,(byte) yy);
  write2eep(address+MONOFF,(byte) month);
  write2eep(address+DOFF,(byte) day);
  write2eep(address+HOFF,(byte) hour);
  write2eep(address+MINOFF,(byte) minute);
  write2eep(address+W1OFF,wt[0]);
  write2eep(address+W2OFF,wt[1]);
  write2eep(address+W3OFF,wt[2]);
  write2eep(address+W4OFF,wt[3]);
  write2eep(address+T1OFF,wt[4]);
  write2eep(address+T2OFF,wt[5]);
  write2eep(address+T3OFF,wt[6]);
  write2eep(address+T3OFF,wt[7]);
  write2eep(address+CHKOFF,(byte) chksum);
  write2eep(address+SUCOFF,suc);
  address += 0x10;
  eep.sleepmode();
}

/******************************************************************************/
//actually write the data to the eeprom, maximum three tries for successful 
//write
/******************************************************************************/
void write2eep(uint32_t add, byte val)
{
  //write data to EEPROM, maximum three tries
  //allows error handling by returning 1 or 0
  for (uint8_t i = 0; i<3; i++)
  {
    if (eep.write(add,val))
      return;
  }
}
