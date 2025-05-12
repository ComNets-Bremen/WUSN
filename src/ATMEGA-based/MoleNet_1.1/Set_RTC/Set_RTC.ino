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
 ******************************************************************************/

/*
  RV8523 RTC (Real-Time-Clock) Example

  Uno       A4 (SDA), A5 (SCL)
  Mega      20 (SDA), 21 (SCL)
  Leonardo   2 (SDA),  3 (SCL)
  
  Note: To enable the I2C pull-up resistors on the RTC-Breakout, the jumper J1 has to be closed.
 */

//#include <Wire.h>
#include <RV8523.h>
#include "RV8523custom.h"


RV8523 rtc;
RV8523custom rtcc;


void setup()
{
  //init Serial port
  Serial.begin(9600);
  while(!Serial); //wait for serial port to connect - needed for Leonardo only

  //set 24 hour mode
  rtc.set24HourMode();

  //start RTC
  rtc.start();

  //When the power source is removed, the RTC will keep the time.
  rtc.batterySwitchOver(1); //battery switch over on
}


void loop()
{
  uint8_t sec, min, hour, day, month, weekday;
  uint16_t year;
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    //int number = Serial.read();
		if (input == 'h') 
		{
		  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  uint8_t val = Serial.parseInt();
		  rtcc.set_hour(val);
  	}	
    if (input == 'm') 
		{
		  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  uint8_t val = Serial.parseInt();
		  rtcc.set_minute(val);
  	}	
  	if (input == 's') 
		{
		  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  uint8_t val = Serial.parseInt();
		  rtcc.set_second(val);
  	}	
  	if (input == 'd') 
		{
		  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  uint8_t val = Serial.parseInt();
		  rtcc.set_day(val);
  	}	
  	if (input == 'M') 
		{
		  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  uint8_t val = Serial.parseInt();
		  rtcc.set_month(val);
  	}	
  	if (input == 'y') 
		{
		  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  int val = Serial.parseInt();
		  rtcc.set_year(val);
  	}	
  	if (input == 'w')
  	{
  	  Serial.flush(); //flush all previous received and transmitted data
      while(!Serial.available()) ; 
		  uint8_t val = Serial.parseInt();
		  rtcc.set_Weekday(val);
  	}
  	
  	
  }	

	//get time from RTC
  rtc.get(&sec, &min, &hour, &day, &month, &year);
  rtcc.get_Weekday(&weekday);
  //serial output
  Serial.print("\nTime: ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(min, DEC);
  Serial.print(":");
  Serial.print(sec, DEC);

  Serial.print("\nDate: ");
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
  Serial.print(day, DEC);
  Serial.print(".");
  Serial.print(month, DEC);
  Serial.print(".");
  Serial.print(year, DEC);

  //wait a second
  delay(1000);
}
